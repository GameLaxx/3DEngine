#include "../../src/coordinates.h"
#include "../../src/print.h"
#include "../../src/draw.h"
#include "../../src/raytracing.h"
#include "../../src/objects.h"
#include <stdlib.h>

SDL_Renderer* g_renderer;

int main(){
    rgba_t red = {.red=255, .green = 0, .blue = 0, .alpha = 0};
    point_t center = {0, 0, 0};
    vector_t extend = {2, 1,8};
    cube_t cube1 = {.center=center, .extendVector=extend, .rotateX=-15, .rotateY=10, .rotateZ=15};
    object_t object1 = {.content_ptr = &cube1, .color = red, .specular = 40, .reflective=0.3, .type=OT_cube};
    OBJ_initObject(&object1);
    point_t origin = {1,2,3};
    point_t* f_ptr = COO_matrixVectorProduct(cube1.invertRotationMatrice, &origin);
    printf("Invert\n");
    printCoo(f_ptr);
    point_t* s_ptr = COO_matrixVectorProduct(cube1.rotationMatrice, f_ptr);
    printf("Normal\n");
    printCoo(s_ptr);
}
