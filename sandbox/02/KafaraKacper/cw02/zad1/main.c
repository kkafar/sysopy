#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char * argv[]) {
    char * file1, * file2;
    bool falloc = false;
    
    if (argc > 3 || argc == 2) {
        printf("Invalid number of arguments.");
        exit(1);
    } else if (argc == 3) {
        file1 = argv[1]; 
        file2 = argv[2];
    } else {
        file1 = (char *) calloc(FILENAME_MAX, sizeof(char));
        
        if (!file1) {
            printf("Failed to allocate memory for file1\n");
            exit(3);
        }

        file2 = (char *) calloc(FILENAME_MAX, sizeof(char));

        if (!file2) {
            printf("Failed to allocate memory for file2\n");
            free(file1);
            exit(3);
        } 

        falloc = !falloc;

        printf("Insert file 1 pathname: ");
        scanf("%s", file1);

        printf("Insert file 2 pathname: ");
        scanf("%s", file2);
    }

#ifdef LIB
    FILE * fp1 = fopen(file1, "r");
    FILE * fp2 = fopen(file2, "r");

    if (!fp1 || !fp2) {
        printf("Failed to open file!\n");
        if (falloc) {
            free(file1);
            free(file2);
        } 
        exit(2);
    }

    FILE * cfp = fp1;
    char ch;

    while (fread(&ch, sizeof(char), 1, cfp)) {
        fwrite(&ch, sizeof(char), 1, stdout);
        if (ch == '\n') cfp = (cfp == fp1) ? fp2 : fp1;
    }

    if (feof(cfp))          cfp = (cfp == fp1) ? fp2 : fp1;
    else if (ferror(cfp))   fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));

    while (fread(&ch, sizeof(char), 1, cfp)) 
        fwrite(&ch, sizeof(char), 1, stdout);

    if (feof(cfp))          cfp = (cfp == fp1) ? fp2 : fp1;
    else if (ferror(cfp))   fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));


    fclose(fp1);
    fclose(fp2);
#endif

#ifdef SYS
    int fd1, fd2;

    fd1 = open(file1, O_RDONLY);
    fd2 = open(file2, O_RDONLY);

    if (fd1 == -1 || fd2 == -1) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        if (falloc) {
            free(file1);
            free(file2);
        }
        exit(2);
    }

    int cfd = fd1;
    ssize_t read_ret;
    char ch;
    while ((read_ret = read(cfd, &ch, sizeof(char))) > 0) {
        write(STDOUT_FILENO, &ch, sizeof(char));
        if (ch == '\n') cfd = (cfd == fd1) ? fd2 : fd1;
    }

    if (read_ret == 0) cfd = (cfd == fd1) ? fd2 : fd1;
    else if (read_ret == -1) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
    }

    while ((read_ret = read(cfd, &ch, sizeof(char))) > 0)
        write(STDOUT_FILENO, &ch, sizeof(char));

    if (read_ret == 0) cfd = (cfd == fd1) ? fd2 : fd1;
    else if (read_ret == -1) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
    }

    close(fd1);
    close(fd2);
#endif 

    if (falloc) {
        free(file1);
        free(file2);
    }

    return 0;
}