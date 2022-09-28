all: MyFind

MyFind: main.cpp
	g++ -std=c++17 -g -Wall -o MyFind main.cpp
	
clean:
	rm -f MyFind
