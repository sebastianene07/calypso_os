#ifndef __STDIO_H
#define __STDIO_H

int printf(const char * format, ... );

int open(const char *, int, ...);

int sprintf(char *out, const char *format, ...);

int sscanf(const char *str, const char *format, ...);

#endif /* __STDIO_H */
