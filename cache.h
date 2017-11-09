#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>//
#include <fcntl.h>//
#include <stddef.h>//
#include <sys/types.h>//
#include <unistd.h>//
#include <errno.h>//

#include <openssl/sha.h>//
#include <zlib.h>//

#define DEFAULT_DB_ENVIRONMENT ".dircache/objects"

int read_cache(void);//
int write_sha1_buffer(unsigned char *sha1, void *buf, unsigned int size);//
int write_sha1_file(char *buf, unsigned len);//

#endif
