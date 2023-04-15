all: compile_main_db execute_main_db compile_main  execute_main 
recompile: clean all
db: compile_main_db execute_main_db


compile_main:
	g++ main.cpp -o main -l sqlite3
execute_main:
	./main.exe
compile_main_db:
	g++ main_db.cpp -o main_db -l sqlite3
execute_main_db:
	./main_db.exe
clean:
	rm -f main *.o
	rm -f main_db *.o
	rm main.db
	
	

