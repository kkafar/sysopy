#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


int main(int argc, char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Invalid number of aruments: %d. Expected 2", argc - 1);
        exit(1);
    }

    char ch             = argv[1][0];
    char * pathname     = argv[2];

    FILE * fp  = fopen(pathname, "r");

    return 0;
}