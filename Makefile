CC=g++

FLAGS=-std=c++17 -I./json -L./json -ljson

SOURCE=./sample.cpp

PROC=sample

all: $(PROC)


$(PROC): 
	$(CC) -o $(PROC) $(SOURCE) $(FLAGS) 
	
clean:
	rm -fr $(PROC) 
