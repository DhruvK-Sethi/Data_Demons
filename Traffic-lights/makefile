all: compile execute
recompile: clean all

compile:
	g++ main.cpp -o main -l sqlite3
execute:
	./main.exe
clean:
	rm -f main *.o
	rm main.db
