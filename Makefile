objects = src/main.o src/util/file.o src/setup_opengl.o
lib = -Llib -lglfw3 -lGL -lGLEW
CPPFLAGS = -g -I include/
CC = gcc
name = test

all: $(objects)
	g++ $(objects) -o build/$(name) $(lib)
src/main.o:
src/util/file.o:
src/setup_opengl.o:

.PHONY: clean
clean:
	rm $(objects)
