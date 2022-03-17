objects = src/main.o src/util/file.o
lib = -Llib -lglfw3 -lGL -lEGL -lX11 -lGLEW
CPPFLAGS = -g -I include/
CC = gcc
name = test

all: $(objects)
	g++ $(objects) -o build/$(name) $(lib)
src/main.o:
src/util/file.o:

.PHONY: clean
clean:
	rm $(objects)
