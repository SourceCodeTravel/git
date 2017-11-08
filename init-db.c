#include "cache.h"

int main(int argc, char **argv)
{
	char *sha1_dir, *path;//
	int len, i;//

	if (mkdir(".dircache", 0700) < 0) {
		perror("unable to create .dircache");
		exit(1);
	}

	sha1_dir = DEFAULT_DB_ENVIRONMENT;
	len = strlen(sha1_dir);
	if (mkdir(sha1_dir, 0700) < 0) {
		exit(1);//
	}

	path = malloc(len + 40);
	memcpy(path, sha1_dir, len);
	for (i = 0; i < 256; i++) {
		sprintf(path+len, "/%02x", i);
		if (mkdir(path, 0700) < 0) {
			exit(1);
		}
	}

	return 0;
}

