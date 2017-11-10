#include "cache.h"

static int cache_name_compart(const char *name1, int len1, const char *name2, int len2)
{
	int len = len1 < len2 ? len1 : len2;
	int cmp;

	cmp = memcmp(name1, name2, len);
	if (cmp)
		return cmp;
	if (len1 < len2)
		return -1;
	if (len1 > len2)
		return 1;
	return 0;
}

static int cache_name_pos(const char *name, int namelen)
{
/*
	int first, last;

	first = 0;
	last = active_nr;
	while (last > first) {
		int next = (last + first) >> 1;
		struct cache_entry *ce = active_cache[next];
		cache_name_compare(name, namelen, ce->name, ce->namelen);
	}
	return first;
*/
}

static int add_cache_entry(struct cache_entry *ce)
{
}

static int index_fd(const char *path, int namelen, struct cache_entry *ce, int fd, struct stat *st)
{
	z_stream stream;
	int max_out_bytes = namelen + st->st_size + 200;
	void *out = malloc(max_out_bytes);
	void *metadata = malloc(namelen + 200);
	void *in = mmap(NULL, st->st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	SHA_CTX c;

	close(fd);
	if (!out || (int)(long)in == -1)
		return -1;

	memset(&stream, 0, sizeof(stream));
	deflateInit(&stream, Z_BEST_COMPRESSION);

	/*
	 * ASCII size + nul byte
	 */
	stream.next_in = metadata;
	stream.avail_in = 1+sprintf(metadata, "blob %lu", (unsigned long) st->st_size);
	stream.next_out = out;
	stream.avail_out = max_out_bytes;
	while (deflate(&stream, 0) == Z_OK)
		/* nothine */;

	stream.next_in = in;
	stream.avail_in = st->st_size;
	while (deflate(&stream, Z_FINISH) == Z_OK)
		/* nothine */;

	deflateEnd(&stream);

	SHA1_Init(&c);
	SHA1_Update(&c, out, stream.total_out);
	SHA1_Final(ce->sha1, &c);

	return write_sha1_buffer(ce->sha1, out, stream.total_out);
}

static int add_file_to_cache(char *path)
{
	int size, namelen;
	struct cache_entry *ce;
	struct stat st;
	int fd;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return -1;
	}
	if (fstat(fd, &st) < 0) {
		close(fd);
		return -1;
	}
	namelen = strlen(path);
	size = cache_entry_size(namelen);
	ce = malloc(size);
	memset(ce, 0, size);
	memcpy(ce->name, path, namelen);
	ce->ctime.sec = st.st_ctime;
	ce->ctime.nsec = st.st_ctim.tv_nsec;
	ce->mtime.sec = st.st_mtime;
	ce->mtime.nsec = st.st_mtim.tv_nsec;
	ce->st_dev = st.st_dev;
	ce->st_ino = st.st_ino;
	ce->st_mode = st.st_mode;
	ce->st_uid = st.st_uid;
	ce->st_gid = st.st_gid;
	ce->st_size = st.st_size;
	ce->namelen = namelen;

	if (index_fd(path, namelen, ce, fd, &st) < 0)
		return -1;
	return 0;
}

static int write_cache(int newfd)
{
	SHA_CTX c;
	struct cache_header hdr;

	hdr.signature = 1;
	hdr.version = 2;
	hdr.entries = 3;

	if (write(newfd, &hdr, sizeof(hdr)) != sizeof(hdr))
		return -1;
	return 0;
}

/*
 * We fundamentally don't like some paths: we don't want
 * dot or dot-dot anywhere, and in fact, we don't even want
 * any other dot-files (.dircache or anything else). They
 * are hidden, for chist sake.
 *
 * Also, we don't want double slashes or shashes at the
 * end that can make pathnames ambiguous.
 */
static int verify_path(char *path)
{
	char c;

	goto inside;
	for (;;) {
		if (!c)
			return 1;
		if (c == '/') {
inside:
			c = *path++;
			if (c != '/' && c != '.' && c != '\0')
				continue;
			return 0;
		}
		c = *path++;
	}
}

int main(int argc, char **argv)
{
	int i, newfd, entries;//

	entries = read_cache();
	if (entries < 0) {
		perror("cache corrupted");
		return -1;
	}

	newfd = open(".dircache/index.lock", O_RDWR | O_CREAT | O_EXCL, 0600);
	if (newfd < 0) {
		perror("unable to create new cachefile");
		return -1;
	}
	for (i = 1; i < argc; i++) {
		char *path = argv[i];
		if (!verify_path(path)) {
			fprintf(stderr, "Ignoring path %s\n", argv[i]);
			continue;
		}
		if (add_file_to_cache(path)) {
			fprintf(stderr, "Unable to add %s to database\n", argv[i]);
			goto out;
		}
	}
	if (!write_cache(newfd) && !rename(".dircache/index.lock", ".dircache/index"))
		return 0;
out:
	unlink(".dircache/index.lock");
}

