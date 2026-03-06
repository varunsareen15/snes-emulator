snes: src/main.o src/cartridge.o src/cpu.o
	g++ -o snes src/main.o src/cartridge.o src/cpu.o

src/main.o: src/main.cpp
	g++ -std=c++17 -Wall -Wextra -Iincludes -c src/main.cpp -o src/main.o

src/cartridge.o: src/cartridge.cpp
	g++ -std=c++17 -Wall -Wextra -Iincludes -c src/cartridge.cpp -o src/cartridge.o

src/cpu.o: src/cpu.cpp
	g++ -std=c++17 -Wall -Wextra -Iincludes -c src/cpu.cpp -o src/cpu.o

clean:
	rm -rf src/*.o snes
