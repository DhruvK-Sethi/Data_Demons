test: run
	rm bin/*

run: build
	./mapathon

build: game.o main.o
	g++ bin/game.o bin/main.o -o mapathon -lsfml-graphics -lsfml-window -lsfml-system -lsqlite3

main.o: main.cpp
	g++ -c main.cpp -o bin/main.o

game.o: game.cpp
	g++ -c game.cpp -o bin/game.o

clean:
	rm bin/*.o mapathon

