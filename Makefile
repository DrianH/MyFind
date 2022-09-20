all: MyFind

MyFind: main.cpp
	g++ -g -Wall -o MyFind main.cpp
	
clean:
	rm -f MyFind
