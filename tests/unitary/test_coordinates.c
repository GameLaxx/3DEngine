#include "../../src/coordinates.h"
#include "../../src/print.h"
#include "../../src/draw.h"
#include "../../src/raytracing.h"
#include <stdlib.h>

SDL_Renderer* g_renderer;

int main(){
    vector_t vec = {.x = 1, .y = 1, .z = 0};
    float rotate[3][3] = {{0.99985,-0.01745,0},{0.01745,0.99985,0},{0,0,1}};
    printCoo(COO_matrixVectorProduct(rotate, &vec));
}