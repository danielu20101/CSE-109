#include "scraping.h"
#include "syscalls.h"
#include <libgen.h>      /* for basename */
#include <unistd.h>      /* for unlink */

int createUrlFromLink (URL_t * url, char * link) {
  size_t size = strlen(link);
  size_t index = 0;

  char protocol[256] = {0};
  char host[253] = {0};  // The maximum length of full domain name
  char path[2048] = {0};
  char c;

  //
  // Find the protocol
  //

  while (index < size && (c = link[index]) != ':') {
    strncat(protocol, &c, 1);
    index++;
  }

  if (index >= size) return 1;
  if (index >= size || link[index++] != ':') return 1;
  if (index >= size || link[index++] != '/') return 1;
  if (index >= size || link[index++] != '/') return 1;

  // Check if the protocol is valid
  if (strcmp(protocol, "http") != 0 && strcmp(protocol, "https") != 0) {
    printf("Bad protocol found: %s\n", protocol);
    return 1;
  }
  url->protocol = (char *) Malloc(strlen(protocol));
  if (url->protocol == NULL) return 1;
  strcpy(url->protocol, protocol);

  //
  // Find the host
  //

  while (index < size && (c = link[index]) != '/') {
    strncat(host, &c, 1);
    index++;
  }

  url->host = (char *) Malloc(strlen(host));
  if (url->host == NULL) return 1;
  strcpy(url->host, host);

  //
  // Copy the rest chars as path
  //
  while (index < size) {
    c = link[index];
    strncat(path, &c, 1);
    index++;
  }
  url->path = (char *) Malloc(strlen(path));
  if (url->path == NULL) return 1;
  strcpy(url->path, path);


  return 0;
}

int findNextLink (FILE * fd, char * buffer, size_t size) {
  char c;
  char quote;
  while ((c = fgetc(fd)) != EOF) {
    if (c != '<') continue;

    c = fgetc(fd);
    if (c == '!') {
      // This is a comment. Ignore all chars until get the char '>'
      while (c != '>') {
        c = fgetc(fd);
      }
    }

    if (c == 'a') {
      // Ignore spaces
      c = fgetc(fd);
      while (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
        c = fgetc(fd);
      }

      if (c == 'h' && fgetc(fd) == 'r' && fgetc(fd) == 'e' && fgetc(fd) == 'f') {
        // Ignore spaces
        c = fgetc(fd);
        while (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
          c = fgetc(fd);
        }

        // We need a "="
        if (c != '=') continue;

        // Ignore spaces
        c = fgetc(fd);
        while (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
          c = fgetc(fd);
        }

        // Determine the quote char
        if (c != '"' && c != '\'') continue;
        quote = c;

        // Save all chars until get quote char again
        while ((c = fgetc(fd)) != quote) {
          *buffer++ = c;
        }
        *buffer++ = '\0';
        // Break for now
        return 0;
      }
    }
  }
  return 1;
}

int searchHrefs (char *userid, char *password, char *server, char *pathname, int * count) {
  // Set the filename to save the file
  char filename[256];
  sprintf(filename, "/tmp/%s-%s", basename(pathname), userid);
  printf("Will save in %s\n", filename);

  int rc = ftpDownload(userid, password, server, pathname, filename);  /* call ftpDownload function and capture return code */
  if (rc) {                                                  /* bail if transfer was unsuccessful */
      fprintf(stderr, "\n%s (%d) - %s from %s\n\n", curl_easy_strerror(rc), rc, pathname, server);
      return rc;
  }

  // Open the file
  FILE * fd = Fopen(filename, "r");
  if (fd == NULL) {
    fprintf(stderr, "Cannot open %s\n", filename);
    return 201;
  }

  char linkBuffer[2048];  // Max for URL size

  while (findNextLink(fd, linkBuffer, 2048) != 1) {
    printf("Link: %s\n", linkBuffer);

    URL_t url;
    if (createUrlFromLink(&url, linkBuffer)) {
      Fclose(fd);
      // Delete the file
      if (unlink(filename) != 0) {
        fprintf(stderr, "Cannot delete %s\n", filename);
      }
      printf("Delete %s\n", filename);
      return 201;  // Bad format
    }

    // That is a good link
    (*count)++;

    rc = searchHrefs(userid, password, url.host, url.path, count);
    if (rc) break;
  }

  Fclose(fd);
  // Delete the file
  if (unlink(filename) != 0) {
    fprintf(stderr, "Cannot delete %s\n", filename);
  }
  printf("Delete %s\n", filename);

  return rc;
}
