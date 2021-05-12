#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>

/**
 * @brief prints given message to stderr along with message 
 *        provided by strerror(errnum) and exit with current 
 *        errno value
 * @param errmsg additional message to be printed 
 * @param file name of file from which the procedure is called 
 * @param func name of function from which the procedure is called
 * @param line number of line of file in which the error occured
 */
void syserr(const char errmsg[], const char file[], const char func[], int line);

/**
 * @brief print given message to stderr and exit with current errno value
 * @param errmsg message to be printed
 * @param file name of file from which the procedure is called
 * @param func name of function from which the procedure is called
 * @param line number of line of file in which the error occured 
 */
void err(const char errmsg[], const char file[], const char func[], int line);

/**
 * @brief prints given message to stderr
 * @param errmsg message to be printed
 * @param file name of file from which the procedure is called
 * @param func name of function from which the procedure is called
 * @param line number of line of file in which the error occured 
 */
void err_noexit(const char errmsg[], const char file[], const char func[], int line);

/**
 * @brief prints given message to stderr along with message provided
 *        by strerror(errnum)
 * @param errmsg message to be printed
 * @param file name of file from which the procedure is called
 * @param func name of function from which the procedure is called
 * @param line number of line of file in which the error occured 
 */
void syserr_noexit(const char errmsg[], const char file[], const char func[], int line);

/**
 * @brief fills given array with 0s
 * 
 * @param buf pointer to to-be-zeroed array
 * @param size size of buffer
 * 
 * @return 0 indicates success, 
 *         -1 indicates that buf==NULL || size <= 0
 */
int clearbuf(char buf[], size_t size);
#endif