#ifndef _SCRAPING
#define _SCRAPING

#include <stdio.h>
#include <stdlib.h>

struct URL {
  char * protocol;
  char * host;
  char * path;
};
typedef struct URL URL_t;

/**
 * @brief Create an struct URL_t from a given link.
 * 
 * @param url This is a pointer of struct URL_t
 * @param link The link found
 * @return int On success, zero is returned
 */
int createUrlFromLink (URL_t * url, char * link);

/**
 * @brief Get the next link from a FILE.
 * 
 * @param fd The File descriptor
 * @param buffer Buffer where the link will be written
 * @param size The buffer size
 * @return int On success, zero is returned
 */
int findNextLink (FILE * fd, char * buffer, size_t size);

/**
 * @brief Search a href, get the link, download it and search href again.
 * 
 * @param userid The userid
 * @param password The password
 * @param server The server
 * @param pathname The pathname
 * @param count Counter of links
 * @return int On success, zero is returned
 */
int searchHrefs (char *userid, char *password, char *server, char *pathname, int * count);

#endif
