CC=g++

FLAGS=-std=c++17 -fPIC

SOURCE=./json.cpp

OBJ=./json.o

LIB=libjson

all: $(LIB)


$(LIB): $(OBJ)
	$(CC) -shared -o $(LIB).so $(OBJ) 
	
$(OBJ): $(SOURCE)
	$(CC) -c -o $(OBJ) $(SOURCE) $(FLAGS)

clean:
	rm -fr $(OBJ) $(LIB).so
