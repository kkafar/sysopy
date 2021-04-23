#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "util.h"

int main(int argc, char * argv[])
{
    if (argc != 2 && argc != 4) err("bad arg count; usage: main nadawca | data or main <email> <subject> <email body>", __FILE__, __func__, __LINE__);

    FILE * fp = NULL;
    if (argc == 2) 
    {   
        char buf[2048];
        if (strcmp(argv[1], "data") == 0)
        {
            if (sprintf(buf, "mailx | sed '1d;$d' | sort -k5M -k6 -k7") < 0)
                syserr("sprintf", __FILE__, __func__, __LINE__);
                
            if ((fp = popen(buf, "w")) == NULL) 
                syserr("popen failed", __FILE__, __func__, __LINE__);
        }
        else if (strcmp(argv[1], "nadawca") == 0)
        {
            if (sprintf(buf, "mailx | sed '1d;$d' | sort -k3") < 0)
                syserr("sprintf", __FILE__, __func__, __LINE__);
                
            if ((fp = popen(buf, "w")) == NULL) 
                syserr("popen failed", __FILE__, __func__, __LINE__);
        }
        else err("unknown argument", __FILE__, __func__, __LINE__);
        // jak posortować ten jebany e-mail
    }
    else 
    {
        size_t message_length = strlen(argv[3]) + strlen(argv[2]) + strlen(argv[1]);
        char buf[message_length + 40];

        if (sprintf(buf, "echo \"%s\" | mailx -s \"%s\" \"%s\"", argv[3], argv[2], argv[1]) < 0) 
            syserr("sprintf", __FILE__, __func__, __LINE__);

        FILE * fp;
        if ((fp = popen(buf, "r")) == NULL) 
            syserr("popen failed", __FILE__, __func__, __LINE__);

        if (pclose(fp) < 0) 
            syserr("pclose failed", __FILE__, __func__, __LINE__);
    }   
    
    if (fp && pclose(fp) < 0) 
        syserr("pclose failed", __FILE__, __func__, __LINE__);

    exit(EXIT_SUCCESS);
}