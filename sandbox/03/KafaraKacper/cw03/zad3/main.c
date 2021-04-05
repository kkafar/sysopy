#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <regex.h>

// #define LINE_MAX 255

int regex_compile(regex_t * r, const char * pattern);
char * getpattern(const char * raw_pattern);

int main(int argc, char * argv[]) 
{
    if (argc != 4) 
    {
        fprintf(stderr, "Bad arg count. Usage: <dirpath> <pattern> <max_depth>\n");
        exit(EXIT_FAILURE);
    }

    // kompilowanie regexów
    regex_t file_extension_regex;
    regex_t pattern;

    int ret;
    if ((ret = regcomp(&file_extension_regex, "\.txt$", REG_NEWLINE | REG_EXTENDED)) != 0)
    {
        char errmssg[LINE_MAX];
        regerror(ret, &file_extension_regex, errmssg, LINE_MAX);
        fprintf(stderr, "%s: %d: File extension regex: %s\n", __func__, __LINE__, errmssg);
        exit(EXIT_FAILURE);
    }

    char * regex_pattern = getpattern(argv[2]);
    printf("Regex pattern: %s\n", regex_pattern);

    if ((ret = regcomp(&pattern, regex_pattern, REG_NEWLINE | REG_EXTENDED)) != 0) 
    {
        char errmssg[LINE_MAX];
        regerror(ret, &file_extension_regex, errmssg, LINE_MAX);
        fprintf(stderr, "%s: %d: Pattern regex: %s\n", __func__, __LINE__, errmssg);
        exit(EXIT_FAILURE);
    }

    size_t search_depth;

    if ((search_depth = strtol(argv[3], NULL, 10)) < 0) 
    {
        fprintf(stderr, "%s: %d: Invalid max_depth number provided.\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }


    DIR * dirp;

    if ((dirp = opendir(argv[1])) == NULL)
    {
        int errnum = errno;
        fprintf(stderr, "%s: %d: %s", __func__, __LINE__, strerror(errnum));
        exit(errnum);
    }
    
    struct dirent * dir;

    errno = 0;
    while ((dir = readdir(dirp)) != NULL) 
    {
        if (dir->d_type == DT_DIR) 
        {

        } 
        else if (dir->d_type == DT_REG) 
        {

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