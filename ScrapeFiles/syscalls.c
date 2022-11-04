#include "syscalls.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>

struct ftpfile {
  const char *filename;         /* name to store the file as */
  FILE *stream;                 /* file pointer/handle */
};
typedef struct ftpfile ftpFile_t;

FILE *Fopen (const char *filename, const char *mode) {
  FILE *fp;
  fp = fopen (filename, mode);
  if (fp == NULL)
    fprintf (stderr, "fopen error (%d): %s\n", errno, strerror (errno));
  return fp;
}

void *Malloc (size_t size) {
  void *ptr;
  ptr = malloc (size);
  if (ptr != NULL)
    return ptr;
  else {
    fprintf (stderr, "malloc error (%d): %s\n", errno, strerror (errno));
    return ptr;
  }
}

pid_t Fork (void) {
  pid_t pid;
  pid = fork();
  if (pid == -1)
  {
    fprintf (stderr, "Fork error (%d): %s\n", errno, strerror (errno));
  }
  return (pid);
}

int Pipe (int fd[2]) {
  int p;
  p = pipe (fd);
  if (p == -1)
  {
    fprintf (stderr, "Pipe error (%d): %s\n", errno, strerror (errno));
  }
  return (p);
}

size_t Fread (void *bufptr, size_t bufsize, size_t numbuf, FILE *fp) {
  if(fp != NULL) {
    return fread(bufptr, bufsize, numbuf, fp);
  } else {
    fprintf (stderr, "fwrite error (%d): %s\n", errno, strerror (errno));
    return bufsize;
  }
}

/* in: buffer ptr, buffer size, num buffers, output file ptr; return: bytes written */
size_t Fwrite (const void *bufptr, size_t bufsize, size_t numbuf, FILE * fp) {
  if (fp != NULL) {
    return fwrite(bufptr, bufsize, numbuf, fp);
  } else {
   fprintf (stderr, "fwrite error (%d): %s\n", errno, strerror (errno));
   return bufsize;
  }
}

/* in: file ptr */
void Fclose (FILE *fp) {
  if (fp != NULL) {
    fclose(fp);
  } else {
	  fprintf(stderr, "fclose error (%d): %s\n", errno, strerror (errno));
  }
}

ssize_t Read (int fd, void *buf, size_t nbyte) {
  int r;
  r = read (fd, buf, nbyte);
  if (r == -1)
  {
    fprintf(stderr, "Read error: (%d): %s\n", errno, strerror (errno));
  }
  return (r);
}

ssize_t Write (int fd, const void *buf, size_t nbyte) {
  int nb;
  nb = write (fd, buf, nbyte);
  if (nb == -1)
  {
    fprintf (stderr, "Write error: (%d): %s\n", errno, strerror (errno));
  }
  return (nb);
}

int Close (int fd) {
  int c = close(fd);
  if (c == -1)
  {
    fprintf (stderr, "Close error (%d): %s\n", errno, strerror (errno));
  }
  return (c);
}

size_t ftpWrite (void *buffer, size_t size, size_t nmemb, void *stream) {
  ftpFile_t *oFile = (ftpFile_t *)stream;
  if(!oFile->stream)
    oFile->stream = Fopen(oFile->filename, "w");  /* open file for writing */
  return Fwrite(buffer, size, nmemb, oFile->stream);
}

int ftpDownload (char *userid, char *password, char *server, char *pathname, char *filename) {
  char str[128];                                                 /* character string for setting curl settings, call values, or bail messages */
  CURLcode crc;                                                  /* variable to hold CURL function recturn codes */
  CURL *cp = NULL;                                               /* pointer/handle to curl session/transfer */
  ftpFile_t FTPfile = { filename, NULL };              /* declare and define an ftpFile_t object, you can change filename or make it a variable */

  curl_global_init(CURL_GLOBAL_ALL);                                   /*1    */ 
  cp = curl_easy_init();                                              /* 2 - CHANGE initialize curl global state; setup and initialize underlying libraries */

  if (cp) {                                                       /* check if curl handle creation was successful, and if so, proceed */
    sprintf(str, "%s:%s", userid, password);                      /* set string str to "<userid>:<password>" */

    curl_easy_setopt(cp, CURLOPT_USERPWD, str);
    
    sprintf(str, "sftp://%s/%s", server, pathname);                /* change string str to "sftp://<server>/<pathname>" */

    curl_easy_setopt(cp, CURLOPT_URL, str);                    /* 4 - set protocol, server, directory, and file */

    curl_easy_setopt(cp, CURLOPT_WRITEFUNCTION, ftpWrite);          /* 5 - declare our callback to be called when there's data to be written */

    curl_easy_setopt(cp, CURLOPT_WRITEDATA, &FTPfile);              /* 6 - set a pointer to our struct to pass to the callback */
    curl_easy_setopt(cp, CURLOPT_VERBOSE, 1L);                      /* switch on full protocol/debug output */

    crc = curl_easy_perform(cp);                                  /* 7 - perform the curl call and capture the return value */

    curl_easy_cleanup(cp);                                           /* 8 - cleanup local session/transfer */
  
    Fclose(FTPfile.stream);                                        /* 9 - close the local file */

    curl_global_cleanup();                                         /* 10 - free and clean up associated global resources init call allocated */

  } else {                                                          /* bail in the unlikely event curl handle creation failed */
    fprintf(stderr, "Initialization of curl session/transfer handle was unsuccessful\n");
    exit(202);
  }

  return (int) crc;                                             /* return curl return code */
}
