#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "util.h"

#define LINE_MAX 8096

bool compare_lines(const char line1[], const char line2[]);


int main(int argc, char * argv [])
{
    if (argc != 4) err("bad arg count", __FILE__, __func__, __LINE__);

    char * input_pathname = argv[1];
    char * output_pathname = argv[2];
    long line = strtol(argv[3], NULL, 10);

    FILE * input_file, * output_file;
    if ((input_file = fopen(input_pathname, "r")) == NULL) syserr("fopen", __FILE__, __func__, __LINE__);
    if ((output_file = fopen(output_pathname, "r")) == NULL) syserr("fopen", __FILE__, __func__, __LINE__);


    char buf_input[LINE_MAX], buf_output[LINE_MAX];

    if (fgets(buf_input, LINE_MAX - 1, input_file) == NULL) err("fgets", __FILE__, __func__, __LINE__);
    if (fclose(input_file) < 0) syserr("fclose", __FILE__, __func__, __LINE__);
    
    int line_num = 0;
    while (fgets(buf_output, LINE_MAX - 1, output_file) != NULL && line_num <= line)
    {
        if (line_num == line)
        {
            if (compare_lines(buf_input, buf_output))
                printf("[OK] %s == %s (line %ld)\n", input_pathname, output_pathname, line);
            else
                printf("[ERROR] %s != %s (line %ld)\n", input_pathname, output_pathname, line);
            break;
        }
        ++line_num;
    }
    if (ferror(output_file)) syserr("fgets", __FILE__, __func__, __LINE__);

    if (fclose(output_file) < 0) syserr("fclose", __FILE__, __func__, __LINE__);

    exit(EXIT_SUCCESS);
}


bool compare_lines(const char line1[], const char line2[])
{
    size_t line1_len = strlen(line1);
    size_t line2_len = strlen(line2);
    size_t i = 0, j = 0;

    while (i < line1_len && j < line2_len && line1[i] != ' ' && line2[j] != ' ')
    {
        if (line1[i] != line2[j]) return false;
        ++i; 
        ++j;
    }
    if (i < line1_len - 1 && j < line2_len && (line1[i] != ' ' || line2[j] != ' ')) return false;
    return true;    
}