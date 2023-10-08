test: run
	rm bin/*

run: build
	./mapathon

build: game.o main.o
	g++ bin/game.o bin/main.o -o mapathon -ID:\SFML-2.6.0\include -LD:\SFML-2.6.0\lib -lsfml-graphics -lsfml-window -lsfml-system -lsqlite3

main.o: main.cpp
	g++ -c main.cpp -o bin/main.o -ID:\SFML-2.6.0\include -LD:\SFML-2.6.0\lib

game.o: game.cpp
	g++ -c game.cpp -o bin/game.o -ID:\SFML-2.6.0\include -LD:\SFML-2.6.0\lib

clean:
	rm bin/*.o mapathon

