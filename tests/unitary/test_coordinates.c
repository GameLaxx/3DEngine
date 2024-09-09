#include "../../src/coordinates.h"
#include "../../src/print.h"
#include "../../src/draw.h"
#include "../../src/raytracing.h"
#include "../../src/objects.h"
#include <stdlib.h>

SDL_Renderer* g_renderer;

int main(){
    rgba_t red = {.red=255, .green = 0, .blue = 0, .alpha = 0};
    vector_t vmin1 = {-2, -1,5};
    vector_t vmax1 = {2, 1,8};
    cube_t cube1 = {.vectorMin=vmin1, .vectorMax=vmax1};
    object_t object1 = {.content_ptr = &cube1, .color = red, .specular = 40, .reflective=0.3, .type=OT_cube};

    point_t origin = {0,0,0};
    vector_t rayVector = {0.995,0.4975,1};
    vector_t outsideVector = {0,1,0};
    // 4.975 2.4875 5
    float t = OBJ_intersectObject(&origin, &rayVector, &object1, 0.00001, 50);
    printf("Ret : %f\n", t);
    t = OBJ_intersectObject(&origin, &outsideVector, &object1, 0.00001, 50);
    printf("Ret : %f\n", t);
}
