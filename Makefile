objects = src/main.o src/util/file.o src/setup_opengl.o src/input.o src/collision.o
lib = -Llib -lglfw3 -lGL -lGLEW
CPPFLAGS = -g -I include/ -Wall
CC = gcc
name = test

all: $(objects)
	g++ $(objects) -o build/$(name) $(lib)
src/main.o:
src/util/file.o:
src/setup_opengl.o:
src/input.o:
src/collision.o:

.PHONY: clean
clean:
	rm $(objects)
