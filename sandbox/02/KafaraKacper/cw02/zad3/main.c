#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_NUM 32


int generate_random_numbers(size_t n, const char * pathname);

bool is_perfect_square(long n);


int main(void) {
    const char * data = "dane.txt";
    const char * a_out = "a.txt";
    const char * b_out = "b.txt";
    const char * c_out = "c.txt";

    // generate_random_numbers(100, data);

    char num[MAX_NUM];
    int i = 0, even_count = 0;
    long number; 

    FILE * file = fopen(data, "r");
    FILE * afile = fopen(a_out, "w");
    FILE * bfile = fopen(b_out, "w");
    FILE * cfile = fopen(c_out, "w");

    if (!file || !afile || !bfile || !cfile) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        fclose(file);
        fclose(afile);
        fclose(bfile);
        fclose(cfile);
        exit(1);
    }
 
    while (fread(num + i, sizeof(char), 1, file)) {
        if (num[i] == '\n') {
            num[i + 1] = 'X';
            number = strtol(num, NULL, 10);
            
            if (!(number & 1))                                      
                ++even_count;
            if (i > 2 && (num[i - 2] == '7' || num[i - 2] == '0')) { 
                fwrite(num, sizeof(char), i, bfile);
                fwrite("\n", sizeof(char), 1, bfile);
            }
            if (is_perfect_square(number)) { 
                fwrite(num, sizeof(char), i, cfile);
                fwrite("\n", sizeof(char), 1, cfile);
            }
            i = -1;
        }  
        ++i;
    }

    if (feof(file)) {
        int n = sprintf(num, "%d", even_count);
        printf("liczba: %s, liczba znakow: %d\n", num, n);
        char line[] = "Liczb parzystych jest ";
        fwrite(line, sizeof(char), 23, afile);
        fwrite(num, sizeof(char), n, afile);
        fwrite("\n", sizeof(char), 1, afile);
    } else if (ferror(file)) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        fclose(file);
        fclose(afile);
        fclose(bfile);
        fclose(cfile);
        exit(2);
    }
    return 0;
}


int generate_random_numbers(size_t n, const char * pathname) {
    if (n <= 0 || !pathname) return -1;

    srand(time(NULL));

    FILE * file = fopen(pathname, "w");

    if (!file) {
        fclose(file);
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        exit(3);
    }

    for (size_t i = 0; i < n; ++i)
        fprintf(file, "%d\n", rand());

    fclose(file);
    return 0;
}

bool is_perfect_square(long n) {
    long sqn = (long)(ceil(sqrt(n)));
    return sqn * sqn == n;
}