#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include "util.h"

#define FIFO_PATH   1
#define FILE_PATH   2
#define CHAR_NUM    3
#define BUFSIZE     8096
#define MAX_LINE    8096
#define MAX_PROD    1000    /* maximum number of producers */


typedef struct WriterId
{
    int id;
    char * id_as_string;
}   WriterId;

typedef struct FileBuffer
{
    char * producer_buf[MAX_PROD];
    int max_id;
}   FileBuffer;


int parse(WriterId * wi, char buf[], int bufsize);
int fb_append(FileBuffer * fb, WriterId * wi, const char buf[]);
void fb_init(FileBuffer * fb);
void fb_clear(FileBuffer * fb);
WriterId * wi_alloc();
void wi_init(WriterId * wi, int id, char * id_as_str);
void wi_delete(WriterId * wi);
void clear_buf(char buf[], int size);

int main(int argc, char * argv[])
{    
    if (argc != 4) err("bad arg count; usage: konsument fifo_path file_path char_num", __FILE__, __func__, __LINE__);

    FILE * file_fifo;
    if ((file_fifo = fopen(argv[FIFO_PATH], "r")) == NULL) syserr("fopen", __FILE__, __func__, __LINE__);

    FILE * file;
    if ((file = fopen(argv[FILE_PATH], "w")) == NULL) syserr("fopen", __FILE__, __func__, __LINE__);

    int bytes_read;
    long N = strtol(argv[CHAR_NUM], NULL, 10);
    
    if (N <= 0) err("invalid N parameter", __FILE__, __func__, __LINE__);
    char buf[BUFSIZE]; 
    char newline_buf[2] = "\n";
    clear_buf(buf, BUFSIZE);

    WriterId * wi;
    FileBuffer fb; fb_init(&fb);

    while ((bytes_read = fread(buf, sizeof(char), N+2, file_fifo)) > 0)
    {
        wi = wi_alloc();
        if (parse(wi, buf, BUFSIZE) < 0) err("parse error", __FILE__, __func__, __LINE__);
        // printf("%s: read from pipe: %s, from writer %d\n", __FILE__, buf, wi->id);
        if (fb_append(&fb, wi, buf) < 0) err ("fb_append failed", __FILE__, __func__, __LINE__);
        clear_buf(buf, BUFSIZE);
        wi_delete(wi);
    }
    if (ferror(file_fifo)) syserr("fread", __FILE__, __func__, __LINE__);
    for (int i = 0; i <= fb.max_id; ++i)
    {
        if (fb.producer_buf[i] != NULL) 
        {
            // printf("%s: saving for writer %d\n", __FILE__, i);
            if (fwrite(strcat(fb.producer_buf[i], "\n"), sizeof(char), strlen(fb.producer_buf[i]) + 1, file) <= 0)
                err("fwrite", __FILE__, __func__, __LINE__);
        }
        else
        {
            if (fwrite(newline_buf, sizeof(char), 1, file) == 0) 
                err("fwrite", __FILE__, __func__, __LINE__);         
        }
    }
    fb_clear(&fb);
    fclose(file);
    fclose(file_fifo);
    exit(EXIT_SUCCESS);
}


void clear_buf(char buf[], int size)
{
    for (int i = 0; i < size; ++i) buf[i] = 0;
}

void wi_delete(WriterId * wi)
{
    if (!wi) return;
    free(wi->id_as_string);
    free(wi);
}

void wi_init(WriterId * wi, int id, char * id_as_str)
{
    if (!wi || !id_as_str) return;
    wi->id_as_string = (char *) calloc(strlen(id_as_str) + 1, sizeof(char));
    strcpy(wi->id_as_string, id_as_str);
    wi->id = id;
}

WriterId * wi_alloc()
{
    WriterId * wi = (WriterId *) malloc(sizeof(WriterId));
    if (!wi) return NULL;
    return wi;
}

int parse(WriterId * wi, char buf[], int bufsize)
{
    if (!wi || !buf || buf[0] == ' ') 
        err("invalid arguments for parser", __FILE__, __func__, __LINE__);
    int start = 0, end = 0;
    while (buf[end] != ' ') ++end;

    int id_length = end - start;
    char localbuf[id_length]; 
    clear_buf(localbuf, id_length);
    for (; start < end; ++start) localbuf[start] = buf[start];

    long id = strtol(localbuf, NULL, 10);
    wi_init(wi, id, localbuf);

    for (int i = 0; i + id_length + 1 < bufsize; ++i)
        buf[i] = buf[i + id_length + 1];
    buf[bufsize - 1 - id_length] = 0;
    return 0;
}

int fb_append(FileBuffer * fb, WriterId * wi, const char buf[])
{
    if (!fb || !wi || !buf) return -1;
    if (fb->producer_buf[wi->id] == NULL)
        if ((fb->producer_buf[wi->id] = (char *) calloc(MAX_LINE, sizeof(char))) == NULL)
            err("calloc error", __FILE__, __func__, __LINE__);
    if (wi->id > fb->max_id) fb->max_id = wi->id;
    strcat(fb->producer_buf[wi->id], buf);
    return 0;
}

void fb_init(FileBuffer * fb)
{
    if (!fb) return;
    for (int i = 0; i < MAX_PROD; ++i) fb->producer_buf[i] = NULL;
    fb->max_id = -1;
}

void fb_clear(FileBuffer * fb)
{
    if (!fb) return;
    for (int i = 0; i < MAX_PROD; ++i)
        if (fb->producer_buf[i] != NULL) free(fb->producer_buf[i]);
}