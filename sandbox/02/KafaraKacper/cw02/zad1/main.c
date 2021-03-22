#include <stdio.h>

#define MAX_FNAME 512


int main(int argc, char * argv[])
{
    char * file1, * file2;
    
    if (argc > 3 || argc == 2)
    {
        printf("Invalid number of arguments.");
        exit(1);
    }
    else if (argc == 3)
    {
        file1 = argv[1]; 
        file2 = argv[2];
    }
    else
    {
        
    }

    return 0;
}