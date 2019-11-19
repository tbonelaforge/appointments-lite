main: main.cpp Interface.o Resource.o Spider.o Time.o Date.o Patient.o sqlite3.o Appointment.h
	g++ main.cpp Interface.o Resource.o Spider.o Time.o Date.o Patient.o sqlite3.o -o main

Date.o: Date.cpp Date.h
	g++ -c Date.cpp -o Date.o

Interface.o: Interface.cpp Interface.h
	g++ -c Interface.cpp -o Interface.o

Resource.o: Resource.cpp Resource.h
	g++ -c Resource.cpp -o Resource.o

Spider.o: Spider.cpp Spider.h Resource.h Patient.h Appointment.h 
	g++ -c Spider.cpp -o Spider.o

Time.o: Time.cpp Time.h
	g++ -c Time.cpp -o Time.o

Date.o: Date.cpp Date.h
	g++ -c Date.cpp -o Date.o

Patient.o: Patient.cpp Patient.h
	g++ -c Patient.cpp -o Patient.o

sqlite3.o: sqlite3.c sqlite3.h sqlite3ext.h
	gcc -c sqlite3.c -o sqlite3.o

shell: shell.c sqlite3.c
	gcc -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION shell.c sqlite3.c -o shell

clean:
	rm -f *.o
	rm -f main
