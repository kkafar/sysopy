#ifndef __UTIL_H__
#define __UTIL_H__

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

#endif