Main: Main.cpp sqlite3.o Patient.o Person.o Procedure.o Doctor.o sqlite3ext.h
	g++ Main.cpp sqlite3.o Patient.o Person.o Procedure.o Doctor.o -o Main


appointments_db_test: appointments_db_test.cpp sqlite3.o sqlite3.h sqlite3ext.h
	g++ appointments_db_test.cpp sqlite3.o -o appointments_db_test

sqlite3.o: sqlite3.c sqlite3.h sqlite3ext.h
	gcc -c sqlite3.c -o sqlite3.o

Person.o: Person.cpp Person.h
	g++ -c Person.cpp -o Person.o

Patient.o: Patient.cpp Patient.h Person.h
	g++ -c Patient.cpp -o Patient.o

Procedure.o: Procedure.cpp Procedure.h
	g++ -c Procedure.cpp -o Procedure.o

Doctor.o: Doctor.cpp Doctor.h
	g++ -c Doctor.cpp -o Doctor.o

shell: shell.c sqlite3.c
	gcc -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION shell.c sqlite3.c -o shell

clean:
	rm -f sqlite3.o
	rm -f Patient.o
	rm -f Doctor.o
	rm -f Person.o
	rm -f Procedure.o
	rm -f shell
	rm -f appointments_db_test
	rm -f Main
