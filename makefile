game: game.o engine.o
	g++ game.o engine.o -o game.exe

game.o: game.cpp engine.hpp
	g++ -static -static-libgcc -static-libstdc++ -c game.cpp

engine.o: engine.hpp engine.cpp
	g++ -c engine.cpp

clean:
	rm *.o
