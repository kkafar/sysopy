#ifndef __UTIL_H__
#define __UTIL_H__

void syserr(const char errmsg[], const char file[], const char func[], int line);

void err(const char errmsg[], const char file[], const char func[], int line);

void err_noexit(const char errmsg[], const char file[], const char func[], int line);

void syserr_noexit(const char errmsg[], const char file[], const char func[], int line);

#endif