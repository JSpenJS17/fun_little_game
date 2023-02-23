game: game.o engine.o
	g++ -optl-static -static-libgcc -static-libstdc++ game.o engine.o -o game.exe

game.o: game.cpp engine.hpp
	g++ -c game.cpp

engine.o: engine.hpp engine.cpp
	g++ -c engine.cpp

clean:
	rm *.o
