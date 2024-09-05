build:
	main

draw: src/draw.h
	gcc -c src/draw.c -o src/draw.o -lSDL2

ray: src/draw.o src/raytracing.h
	gcc src/raytracing.c src/draw.o -o src/raytracing.o

print: src/draw.o src/raytracing.o src/print.h
	gcc src/draw.o src/raytracing.o src/print.c -o src/print.o

main: src/main.c src/draw.o src/raytracing.o src/print.o
	gcc src/main.c src/draw.o src/raytracing.o src/print.o -o engine -lSDL2 -lm
