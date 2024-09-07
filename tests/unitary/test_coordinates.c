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
    rgba_t col1 = {.red = 255, .blue = 255, .green = 255};
    rgba_t col2 = {.red = 0, .blue = 0, .green = 0};
    DRAW_computeReflection(&col1, NULL, 0.0001);
    printRGBA(&col1);
}