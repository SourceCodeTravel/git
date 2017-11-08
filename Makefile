PROG=init-db

init-db: init-db.o

clean:
	rm -f *.o $(PROG)
