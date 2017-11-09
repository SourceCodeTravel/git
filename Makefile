PROG=update-cache init-db write-tree

all: $(PROG)

LIBS=-lssl -lz -lcrypto

init-db: init-db.o

update-cache: update-cache.o read-cache.o
	gcc -o update-cache update-cache.o read-cache.o $(LIBS)

write-tree: write-tree.o

clean:
	rm -f *.o $(PROG)
