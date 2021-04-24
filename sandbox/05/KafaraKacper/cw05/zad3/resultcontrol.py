#/usr/bin/python
from sys import argv
from typing import IO

""" 
args: <input_file> <output_file> <line_of_output_file>
"""


def main():
    if len(argv) != 4:
        print("bad arg count; args: <input_file> <output_file> <line_of_output_file>")
        return -1
    
    input_file: str     = argv[1]
    output_file: str    = argv[2]
    line: int           = int(argv[3])

    try:
        with open(input_file, 'r') as file:
            input_file_content = file.read().rstrip().splitlines()
    except Exception as err:
        print("failed to process file %s, error message: %s" % (input_file, err.strerror))
        return err.errno

    try:
        with open(output_file, 'r') as file:
            output_file_content = file.read().rstrip().splitlines()
    except Exception as err:
        print("failed to process file %s, error message: %s" % (output_file, err.strerror))
        return err.errno
    
    

if __name__ == '__main__':
    main()