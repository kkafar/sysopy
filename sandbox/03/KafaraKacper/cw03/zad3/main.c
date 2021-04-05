#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <regex.h>
#include <stdbool.h>

// #define LINE_MAX 255
#define READ_SIZE 4096

char * getpattern(const char * raw_pattern);
int normalize_path(char * path, size_t max_length);
int extend_path(char path[], char appendix[], size_t max_length);
void clearbuff(char buf[], size_t size);


int main(int argc, char * argv[]) 
{
    if (argc != 4) 
    {
        fprintf(stderr, "Bad arg count. Usage: <dirpath> <pattern> <max_depth>\n");
        exit(EXIT_FAILURE);
    }

    pid_t rootid = getpid();


    /////////////////////////////////////////////////////////////////////////////
    /// compiling regexs
    /////////////////////////////////////////////////////////////////////////////
    regex_t file_extension_regex;
    regex_t pattern;
    size_t raw_pattern_length = strlen(argv[2]);

    int ret;
    if ((ret = regcomp(&file_extension_regex, "\.txt$", REG_NEWLINE | REG_EXTENDED)) != 0)
    {
        char errmssg[LINE_MAX];
        regerror(ret, &file_extension_regex, errmssg, LINE_MAX);
        fprintf(stderr, "%s: %d: File extension regex: %s\n", __func__, __LINE__, errmssg);
        exit(EXIT_FAILURE);
    }

    char * regex_pattern = getpattern(argv[2]);
    // printf("Regex pattern: %s\n", regex_pattern);

    if ((ret = regcomp(&pattern, regex_pattern, REG_NEWLINE | REG_EXTENDED)) != 0) 
    {
        char errmssg[LINE_MAX];
        regerror(ret, &file_extension_regex, errmssg, LINE_MAX);
        fprintf(stderr, "%s: %d: Pattern regex: %s\n", __func__, __LINE__, errmssg);
        exit(EXIT_FAILURE);
    }
    /////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////
    /// determining search depth
    /////////////////////////////////////////////////////////////////////////////
    int search_depth;
    size_t current_search_level = 0;
    
    if ((search_depth = strtol(argv[3], NULL, 10)) < 0) 
    {
        fprintf(stderr, "%s: %d: Invalid max_depth number provided.\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    /////////////////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////////////////
    /// opening intial catalogue 
    /////////////////////////////////////////////////////////////////////////////
    DIR * dirp;

    char current_path[PATH_MAX];
    strcpy(current_path, argv[1]);

    if (normalize_path(current_path, PATH_MAX) < 0) 
    {
        fprintf(stderr, "pid: %d: %s: %d: Failed to normalize path due to exceeding max_length", getpid(), __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    if ((dirp = opendir(current_path)) == NULL)
    {
        int errnum = errno;
        fprintf(stderr, "%s: %d: %s", __func__, __LINE__, strerror(errnum));
        exit(errnum);
    }
    /////////////////////////////////////////////////////////////////////////////
    
    struct dirent * dir;
    pid_t cpid;
    char buf[READ_SIZE];
    regmatch_t match;

    while ((dir = readdir(dirp)) != NULL) 
    {
        clearbuff(buf, READ_SIZE);
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;

        errno = 0;
        if (dir->d_type == DT_DIR && current_search_level < search_depth) 
        {
            if ((cpid = fork()) == 0) 
            {
                ++current_search_level;
                extend_path(current_path, dir->d_name, PATH_MAX);
                closedir(dirp);

                if ((dirp = opendir(current_path)) == NULL)
                {
                    int errnum = errno;
                    fprintf(stderr, "pid: %d: %s: %d: %s", getpid(), __func__, __LINE__, strerror(errnum));
                    exit(errnum);
                }
            }
            else if (cpid == -1) 
            {
                int errnum = errno;
                fprintf(stderr, "pid: %d: %s: %d: %s. Failed for %s\n", getpid(), __func__, __LINE__, strerror(errnum), dir->d_name);
            }
        } 
        else if (dir->d_type == DT_REG) 
        {
            // checking if filename has .txt extension
            const char * tmp = dir->d_name;
            regmatch_t m;
            if (regexec(&file_extension_regex, tmp, 1, &m, 0) != 0) continue;

            char filepath[PATH_MAX];
            strcpy(filepath, current_path);
            strcat(filepath, dir->d_name);

            FILE * filestream;
            if ((filestream = fopen(filepath, "r")) == NULL)
            {
                fprintf(stderr, "pid: %d: %s: %d: %s", getpid(), __func__, __LINE__, strerror(errno));
                continue;
            }

            size_t bytes_read; 
            bool shifted = false;

            while ((bytes_read = fread(buf, sizeof(char), READ_SIZE, filestream)) > raw_pattern_length 
                  || 
                  (bytes_read == raw_pattern_length && !shifted))
            {
                if (regexec(&pattern, buf, 1, &match, 0) == 0)
                {
                    fprintf(stdout, "pid: %d: %s <- pattern found\n", getpid(), filepath);
                    break;
                }  
                fseek(filestream, -raw_pattern_length, SEEK_CUR);
                shifted = true;
            }   
            if (bytes_read == 0) 
            {
                if (ferror(filestream)) 
                {
                    fprintf(stderr, "pid: %d: %s: %d: %s", getpid(), __func__, __LINE__, strerror(errno));
                }
            }         

            fclose(filestream);
        }
    }

    if (errno) 
    {
        int errnum = errno;
        fprintf(stderr, "%s: %d: %s", __func__, __LINE__, strerror(errnum));
        exit(errnum);
    }


    closedir(dirp);
    free(regex_pattern);
    regfree(&file_extension_regex);
    regfree(&pattern);

    if (getpid() == rootid)
    {
        // waiting for all children
        while (waitpid(-1, NULL, 0) != -1);
    }

    exit(EXIT_SUCCESS);
}


char * getpattern(const char * raw_pattern) 
{
    size_t pattern_length = strlen(raw_pattern);
    char * new_pattern = (char *) calloc(pattern_length + 5, sizeof(char));

    if (!new_pattern) return NULL;

    new_pattern[0] = '.';
    new_pattern[1] = '*';
    strcat(new_pattern, raw_pattern);
    new_pattern[pattern_length + 2] = '.';
    new_pattern[pattern_length + 3] = '*';

    return new_pattern;
}


int normalize_path(char * path, size_t max_length)
{
    if (!path) return -2;
    size_t current_length = strlen(path);
    if (current_length >= max_length - 1) return -1;

    if (path[current_length - 1] != '/')
    {
        path[current_length] = '/';
        path[current_length + 1] = 0;
        return current_length + 2;
    }
    return current_length;
}


int extend_path(char path[], char appendix[], size_t max_length)
{
    if (!path || !appendix) return -3;

    size_t path_length      = strlen(path);
    size_t appendix_length  = strlen(appendix);

    if (path_length + appendix_length + 1 >= max_length) return -4;

    strcat(path, appendix);

    return normalize_path(path, max_length);
}

void clearbuff(char buf[], size_t size)
{
    for (char * p = buf; p < buf + size; ++p) *p = 0;
}