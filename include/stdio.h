#ifndef __STDIO_H
#define __STDIO_H

int printf(const char * format, ... );

int open(const char *, int, ...);

int ioctl(int fd, unsigned long request, unsigned long arg);

int mount(const char *type, const char *dir, int flags, void *data);

int umount(const char *dir, int flags);

int sprintf(char *out, const char *format, ...);

int sscanf(const char *str, const char *format, ...);

int snprintf(char *out, unsigned int len, const char *format, ...);

#endif /* __STDIO_H */
