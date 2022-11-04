#include <stdio.h>            /* for fprintf, etc. */
#include <stdlib.h>           /* for exit */
#include <string.h>           /* for strerror */
#include <errno.h>            /* for errno */
#include <curl/curl.h>        /* for curl, baby! */
#include <termios.h>          /* for the struct termios and other stuffs used in getPassword() */
#include "syscalls.h"         /* for wrapped system calls */
#include "scraping.h"         /* for the scraping functions */

#define LIMIT 255  // To write the password

int scraping (char *userid, char *password, char *server, char *pathname);
void getPassword (const char * prompt, char * buffer);

int main (int argc, char *argv[]) {
  int rc = 0;

  char userid[256];
  char password[256];
  char server[256];
  char pathname[2048];

  switch (argc) {
    case 1:
      fprintf(stdout, "\nUsage: curlFTP <userid> <password> <server> <fully-qualified filename>\n\n");
      return rc;
    case 2:
      if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
        fprintf(stdout, "\nUsage: curlFTP <userid> <password> <server> <fully-qualified filename>\n\n");
      return rc;
    case 4:
      // Ask for password
      getPassword("Write your password: ", password);
      strcpy(userid,    argv[1]);
      strcpy(server,    argv[2]);
      strcpy(pathname,  argv[3]);
      break;
    case 5:
      strcpy(userid,    argv[1]);
      strcpy(password,  argv[2]);
      strcpy(server,    argv[3]);
      strcpy(pathname,  argv[4]);
      // rc = ftpDownload(argv[1], argv[2], argv[3], argv[4]);      /* call ftpDownload function and capture return code */
      // if (rc)                                                    /* bail if transfer was unsuccessful */
      //   fprintf(stderr, "\n%s (%d) - %s from %s\n\n", curl_easy_strerror(rc), rc, argv[4], argv[3]);
      break;
    default:
      fprintf(stderr, "\nUsage: curlFTP <userid> <password> <server> <fully-qualified filename>\n\n");
      return 201;
  }

  rc = scraping(userid, password, server, pathname);
  return rc; 
}

int scraping (char *userid, char *password, char *server, char *pathname) {
  printf("userid: %s\n", userid);
  printf("password: %s\n", password);
  printf("server: %s\n", server);
  printf("pathname: %s\n", pathname);
  printf("\n");

  int count = 0;
  int rc = searchHrefs(userid, password, server, pathname, &count);
  printf("%d links found\n", count);

  return rc;
}

void getPassword (const char * prompt, char * buffer) {
  struct termios old, new;
  char c;
  int count = 0;

  // Save last configuration
  tcgetattr( STDIN_FILENO, &old);
  new = old;
  new.c_lflag &= ~ICANON;
  // Setting echo
  new.c_lflag &= ~(ECHO);

  // Change settings
  tcsetattr( STDIN_FILENO, TCSANOW, &new);

  printf(prompt);

  // Read from prompt
  while ((c = getchar()) != '\n' && c != EOF && count < LIMIT){
    buffer[count++] = c;
    printf("*");
  }
  buffer[count] = '\0';
  printf("\n");

  // Reset settings
  tcsetattr( STDIN_FILENO, TCSANOW, &old);
}
