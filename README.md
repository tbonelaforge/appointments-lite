# appointments-lite
Appointment Scheduling Application For Hypothetical Doctors Office

# To build using make and g++ (Tested on MacOSX 10.9.5)

make

# To build on Windows

1. Open command prompt
2. Navigate to project
3. Compile the sqlite3.o object file using gcc:

gcc -c sqlite3.c -o sqlite3.o

4. Compile the appointments_db_test program, using g++:

g++ appointments_db_test.cpp sqlite3.o -o appointments_db_test

# To build the interactive Database Shell (using make):

make shell