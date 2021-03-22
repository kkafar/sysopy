#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


#define MAX_LINE 256

bool contains(char * str, size_t len, char ch);
int readline(FILE * fp, char * buf, size_t bufsize);


int main(int argc, char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Invalid number of aruments: %d. Expected 2\n", argc - 1);
        exit(1);
    }

    char ch             = argv[1][0];
    char * pathname     = argv[2];

    char buf[MAX_LINE];
    int i = 0;
    bool printflag = false;

#ifdef LIB
    FILE * fp  = fopen(pathname, "r");

    if (!fp) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        fclose(fp);
        exit(1);
    }

    while (fread(buf + i, sizeof(char), 1, fp)) {
        if (buf[i] == ch) printflag = true;
        
        if (buf[i] == '\n') {
            if (printflag) {
                for (int j = 0; j <= i; ++j)
                    fwrite(buf + j, sizeof(char), 1, stdout);

                printflag = false;
            }
            i = -1;
        }
        ++i;
    }   
    if (ferror(fp)) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        fclose(fp);
        exit(2);
    }

    fclose(fp);
#endif 

#ifdef SYS
    int fd = open(pathname, O_RDONLY);

    if (fd == -1) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        exit(2);
    }

    int read_ret;

    while ((read_ret = read(fd, buf + i, sizeof(char))) > 0) {
        if (buf[i] == ch) printflag = true;

        if (buf[i] == '\n') {
            if (printflag) {
                for (int j = 0; j <= i; ++j)
                    write(STDOUT_FILENO, buf + j, sizeof(char));

                printflag = false;
            }
            i = -1;
        }
        ++i;
    }

    if (read_ret == -1) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        close(fd);
        exit(3);
    }

    close(fd);
#endif

    return 0;
}