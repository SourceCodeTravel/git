PROG=init-db write-tree

all: $(PROG)

init-db: init-db.o

write-tree: write-tree.o

clean:
	rm -f *.o $(PROG)
