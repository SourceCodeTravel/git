#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/mman.h>

#include <string.h>//
#include <sys/types.h>//
#include <unistd.h>//

#include <openssl/sha.h>
#include <zlib.h>

struct cache_header {
	unsigned int signature;
	unsigned int version;
	unsigned int entries;
	unsigned char shal[20];
};
struct cache_time {
	unsigned int sec;
	unsigned int nsec;
};

struct cache_entry {
	struct cache_time ctime;
	struct cache_time mtime;
	unsigned int st_dev;
	unsigned int st_ino;
	unsigned int st_mode;
	unsigned int st_uid;
	unsigned int st_gid;
	unsigned int st_size;
	unsigned char sha1[20];
	unsigned short namelen;
	unsigned char name[0];
};

#define DB_ENVIRONMENT "SHA1_FILE_DIRECTORY"
#define DEFAULT_DB_ENVIRONMENT ".dircache/objects"

#define cache_entry_size(len) ((offsetof(struct cache_entry, name) + (len) + 8) & ~7)

int read_cache(void);//
int write_sha1_buffer(unsigned char *sha1, void *buf, unsigned int size);//
int write_sha1_file(char *buf, unsigned len);//

#endif
