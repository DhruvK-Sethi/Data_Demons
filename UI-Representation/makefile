all: compile link

compile: 
	g++ -c main.cpp -I "D:\SFML-2.5.1\include" -DSFML_STATIC

link: 
	g++ main.o -o main -L"D:\SFML-2.5.1\lib" -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 -lsfml-main -mwindows

clean:
	rm -f main *.o
	