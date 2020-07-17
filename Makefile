TARGET=piC
all:
	g++ -pedantic -Wall -Wextra -std=c++17 -O2 -c pi_controller.cpp
	g++ pi_controller.o -o $(TARGET) -lsfml-graphics -lsfml-window -lsfml-system

clean:
	rm -f *.o $(TARGET)
