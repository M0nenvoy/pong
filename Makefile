objects = src/main.o
lib = -Llib -lglfw3 -lGL -lGLEW
CPPFLAGS = -g -I include/
name = test

all: $(objects)
	g++ $(objects) -o build/$(name) $(lib)
src/main.o:

.PHONY: clean
clean:
	rm $(objects)
