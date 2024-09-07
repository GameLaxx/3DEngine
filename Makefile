build:
	main

draw: src/draw.h
	gcc -c src/draw.c -o src/draw.o -lSDL2

coo: src/coordinates.h
	gcc -c src/coordinates.c -o src/coordinates.o

ray: src/draw.o src/coordinates.o src/raytracing.h
	gcc src/raytracing.c src/coordinates.o src/draw.o -o src/raytracing.o

print: src/draw.o src/raytracing.o src/coordinates.o src/print.h
	gcc src/draw.o src/raytracing.o src/coordinates.o src/print.c -o src/print.o

main: src/main.c src/draw.o src/raytracing.o src/coordinates.o src/print.o
	gcc src/main.c src/coordinates.o src/draw.o src/raytracing.o src/print.o -o engine -lSDL2 -lm
	./engine

test: tests/unitary/test_coordinates.c src/coordinates.o src/print.o src/raytracing.o src/draw.o
	gcc tests/unitary/test_coordinates.c src/raytracing.o src/coordinates.o src/draw.o src/print.o -o test -lm -lSDL2
	./test