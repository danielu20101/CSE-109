#ifndef _SYSCALLS
#define _SYSCALLS

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>   /* for curl, baby! */

void    *Malloc(size_t);                             /* in: bytes to allocate for the hashtable; return: ptr of type void to allocated memory */
FILE    *Fopen(const char *, const char *);          /* in: path/file, mode; return: file ptr */
void     Fclose(FILE *);  			                /* in: file ptr */
size_t   Fread(void *, size_t, size_t, FILE *);       /* in: buffer ptr, buffer size, num buffers, input file ptr; return: bytes read */
size_t   Fwrite(const void *, size_t, size_t, FILE *);  /* in: buffer ptr, buffer size, num buffers, output file ptr; return: bytes written */
void     Fclose(FILE *);
ssize_t  Read(int fd, void *buf, size_t count);
ssize_t  Write(int fd, const void *buf, size_t count);
int      Close(int fildes);
int      Pipe(int [2]);
pid_t    Fork(void);
int      ftpDownload(char *, char *, char *, char *, char *);
size_t   ftpWrite(void *, size_t, size_t, void *);

#endif
