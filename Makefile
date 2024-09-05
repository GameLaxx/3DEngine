build:
	main

draw: src/draw.h
	gcc -c src/draw.c -o draw.o -lSDL2

main: src/main.c src/draw.o
	gcc src/main.c src/draw.o -o engine -lSDL2
