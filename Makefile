appointments_db_test: appointments_db_test.cpp sqlite3.o sqlite3.h sqlite3ext.h
	g++ appointments_db_test.cpp sqlite3.o -o appointments_db_test

sqlite3.o: sqlite3.c sqlite3.h sqlite3ext.h
	gcc -c sqlite3.c -o sqlite3.o


shell: shell.c sqlite3.c
	gcc -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION shell.c sqlite3.c -o shell

clean:
	rm -f sqlite3.o
	rm -f shell
	rm -f appointments_db_test
