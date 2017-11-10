#include "cache.h"//

const char *sha1_file_directory = NULL;

char *sha1_to_hex(unsigned char *sha1)
{
	static char buffer[50];
	static char hex[] = "01234567890abcdef";
	char *buf = buffer;
	int i;

	for (i = 0; i < 20; i++) {
		unsigned int val = *sha1++;
		*buf++ = hex[val >> 4];
		*buf++ = hex[val & 0xf];
	}
	return buffer;
}

/*
 * NOTE! This returns a statically allocated buffer, so you have to be
 * careful about using it. Do a "strdup()" if you need to save the
 * filename.
 */
char *sha1_file_name(unsigned char *sha1)
{
	int i;
	static char *name, *base;

	if (!base) {
		char *sha1_file_directory = DEFAULT_DB_ENVIRONMENT;//
		int len = strlen(sha1_file_directory);
		base = malloc(len + 60);
		memcpy(base, sha1_file_directory, len);
		memset(base+len, 0, 60);
		base[len] = '/';
		base[len+3] = '/';
		name = base + len + 1;
	}
	for (i = 0; i < 20; i++) {
		static char hex[] = "0123456789abcdef";
		unsigned int val = sha1[i];
		char *pos = name + i*2 + (i > 0);
		*pos++ = hex[val >> 4];
		*pos = hex[val & 0xf];
	}
	return base;
}

int write_sha1_file(char *buf, unsigned len)
{
	int size;
	char *compressed;
	z_stream stream;
	unsigned char sha1[20];
	SHA_CTX c;

	/* Set it up */
	memset(&stream, 0, sizeof(stream));
	deflateInit(&stream, Z_BEST_COMPRESSION);
	size = deflateBound(&stream, len);
	compressed = malloc(size);

	/* Compress it */
	stream.next_in = buf;
	stream.avail_in = len;
	stream.next_out = compressed;
	stream.avail_out = size;

	while (deflate(&stream, Z_FINISH) ==  Z_OK)
		/* nothing */;
	deflateEnd(&stream);
	size = stream.total_out;

	/* Sha1.. */
	SHA1_Init(&c);
	SHA1_Update(&c, compressed, size);
	SHA1_Final(sha1, &c);

	if (write_sha1_buffer(sha1, compressed, size) < 0)
		return -1;
	printf("%s\n", sha1_to_hex(sha1));
	return 0;
}

int write_sha1_buffer(unsigned char *sha1, void *buf, unsigned int size)
{
	char *filename = sha1_file_name(sha1);
	int i, fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd < 0)
		return (errno == EEXIST) ? 0 : -1;
	write(fd, buf, size);
	close(fd);
	return 0;
}

int error(const char * string)
{
	fprintf(stderr, "error: %s\n", string);
	return -1;
}

int read_cache(void)
{
	int fd;//
	//void *map;

	sha1_file_directory = getenv(DB_ENVIRONMENT);
	if (!sha1_file_directory)
		sha1_file_directory = DEFAULT_DB_ENVIRONMENT;
	if (access(sha1_file_directory, X_OK) < 0)
		return error("no access to SHA1 file directory");
	fd = open(".dircache/index", O_RDONLY);
	if (fd < 0)
		return (errno == ENOENT) ? 0 : error("open failed");
	//map = mmap(NULL, 256, PORT_READ, MAP_PRIVATE, fd, 0);//
	//close(fd);
	return 0;
}

