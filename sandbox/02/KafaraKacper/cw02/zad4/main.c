#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char * argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Invalid number of arguments: %d. Expectd 4.\n", argc);
        exit(1);
    }

    const char * input_pathname     = argv[1];
    const char * output_pathname    = argv[2];
    const char * str1               = argv[3];
    const char * str2               = argv[4];

    size_t str1_len = strlen(str1);
    size_t str2_len = strlen(str2);
    
    char * buf = (char *) calloc(str1_len, sizeof(char));
    char ch;
    bool matched = false;

    FILE * input_file = fopen(input_pathname, "r");
    FILE * output_file = fopen(output_pathname, "r+");

    if (!input_file || !output_file) {
        int errnum = errno;
        fprintf(stderr, "%d, errno: %d, %s\n", __LINE__, errnum, strerror(errnum));
        if (input_file) fclose(input_file);
        if (output_file) fclose(output_file);
        exit(errnum);
    }


    while (fread(&ch, sizeof(char), 1, input_file)) {
        matched = false;
        if (ch == str1[0]) {
            fread(buf, sizeof(char), str1_len - 1, input_file);
            matched = true;
            for (int i = 1; i < str1_len; ++i) {
                if (str1[i] != buf[i - 1]) {
                    int retcode = fseek(input_file, 1 - i, SEEK_CUR);
                    matched = false;
                    if (retcode == -1) {
                        int errnum = errno;
                        fprintf(stderr, "%d: errno: %d, %s\n", __LINE__, errnum, strerror(errnum));
                        exit(errnum);
                    }
                    break;
                }
            }            
            if (matched) {
                
            }
        }


    if (ferror(input_file)) {
        int errnum = errno;
        fprintf(stderr, "%d, errno: %d, %s\n", __LINE__, errnum, strerror(errnum));
        if (input_file) fclose(input_file);
        if (output_file) fclose(output_file);
        exit(errnum);
    }
    
    fclose(input_file);
    fclose(output_file);

    return 0;
}