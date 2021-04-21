#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "util.h"

int main(int argc, char * argv[])
{
    if (argc != 2 && argc != 4) err("bad arg count", __FILE__, __func__, __LINE__);

    if (argc == 2) 
    {   
        if (strcmp(argv[1], "data") == 0)
        {
            printf("Not implemented yet\n");
        }
        else if (strcmp(argv[1], "nadawca") == 0)
        {
            printf("Not implemented yet\n");
        }
        else err("unknown argument", __FILE__, __func__, __LINE__);
        // jak posortować ten jebany e-mail
    }
    else 
    {
        size_t message_length = strlen(argv[3]) + strlen(argv[2]) + strlen(argv[1]);
        char buf[message_length + 40];

        if (sprintf(buf, "echo \"%s\" | mailx -s \"%s\" \"%s\"", argv[3], argv[2], argv[1]) < 0) 
            err("sprintf", __FILE__, __func__, __LINE__);

        FILE * fp;
        if ((fp = popen(buf, "r")) == NULL) 
            err("popen failed", __FILE__, __func__, __LINE__);

        if (pclose(fp) < 0) 
            err("pclose failed", __FILE__, __func__, __LINE__);
    }   
    exit(EXIT_SUCCESS);
}