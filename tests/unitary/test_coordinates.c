#include "../../src/coordinates.h"
#include "../../src/print.h"
#include "../../src/draw.h"
#include "../../src/raytracing.h"
#include <stdlib.h>

SDL_Renderer* g_renderer;

int main(){
    vector_t vec = {.x = 5, .y = 4, .z = 3};
    vector_t* inv_ptr = COO_linearTransformation(NULL, 0, &vec, -1);
    printCoo(inv_ptr);
}