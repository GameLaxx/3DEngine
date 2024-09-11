#include <SDL2/SDL.h>
#include <stdio.h>
#include "draw.h"
#include "raytracing.h"
#include "objects.h"
#include "print.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
SDL_Renderer* g_renderer;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    //init
    DRAW_initSDL(800, 800);
    rgba_t red = {255,0,0,255};
    rgba_t blue = {0,0,255,255};
    rgba_t green = {0,255,0,255};
    rgba_t yellow = {255,255,0,255};
    rgba_t purple = {255,0,255,255};
    rgba_t light_gray = {160,160,160,255};
    rgba_t dark_gray = {80,80,80,255};
    rgba_t white = {255,255,255,255};
    rgba_t black = {0,0,0,255};
    DRAW_invertYAxis();
    DRAW_moveOrigin(400,400);
    DRAW_clearRenderer();
    // Init context
    point_t origin = {0,1,0};
    // // sphere 1
    // point_t center1 = {0,-1,3};
    // sphere_t sphere1 = {.center = center1, .radius = 1};
    // object_t object1 = {.content_ptr = &sphere1, .color = red, .specular = 500, .reflective=0.2, .type=OT_sphere};
    // // sphere 4
    // vector_t vmin1 = {-2, -1,5};
    // vector_t vmax1 = {2, 1,8};
    // cube_t cube1 = {.vectorMin=vmin1, .vectorMax=vmax1};
    // object_t object5 = {.content_ptr = &cube1, .color = red, .specular = 40, .reflective=0.3, .type=OT_cube};
    float tvRotationY = -45;
    float tvRotationX = 15;
    // cube 1 (ground)
    point_t cubeCenter1 = {0,-0.5,2};
    vector_t extend1 = {10,0.5,10};
    cube_t cube1 = {.center=cubeCenter1, .extendVector=extend1};
    object_t object1 = {.content_ptr = &cube1, .color = dark_gray, .specular = 40, .reflective=0.3, .type=OT_cube};
    // cube 2 (bottom)
    point_t cubeCenter2 = {0,0.1,2};
    vector_t extend2 = {0.3,0.1,0.3};
    cube_t cube2 = {.center=cubeCenter2, .extendVector=extend2, .rotateX=tvRotationX, .rotateY=tvRotationY};
    object_t object2 = {.content_ptr = &cube2, .color = light_gray, .specular = 40, .reflective=0.1, .type=OT_cube};
    // cube 3
    vector_t cubeCenter3 = {0, 0.4, 2};
    vector_t extend3 = {0.05, 0.2,0.05};
    cube_t cube3 = {.center=cubeCenter3, .extendVector=extend3, .rotateX=tvRotationX, .rotateY=tvRotationY};
    object_t object3 = {.content_ptr = &cube3, .color = black, .specular = 40, .reflective=0.1, .type=OT_cube};
    // cube 4
    vector_t cubeCenter4 = {0, 1.1,2};
    vector_t extend4 = {1, 0.5,0.2};
    cube_t cube4 = {.center=cubeCenter4, .extendVector=extend4, .rotateX=tvRotationX, .rotateY=tvRotationY};
    object_t object4 = {.content_ptr = &cube4, .color = black, .specular = 40, .reflective=0.1, .type=OT_cube};
    // cube 5
    vector_t cubeCenter5 = {0, 1.1,1.95};
    vector_t extend5 = {0.95, 0.45,0.2};
    cube_t cube5 = {.center=cubeCenter5, .extendVector=extend5, .rotateX=tvRotationX, .rotateY=tvRotationY};
    object_t object5 = {.content_ptr = &cube5, .color = black, .specular = 40, .reflective=0.3, .type=OT_cube};
    // sphere 1
    point_t center1 = {0,1.2,1.5};
    sphere_t sphere1 = {.center = center1, .radius = 0.3};
    object_t object6 = {.content_ptr = &sphere1, .color = red, .specular = 500, .reflective=0.2, .type=OT_sphere};

    // light 1
    point_t pos1 = {1,4,-4};
    lightSource_t light1 = {.type=LT_directional, .intensity=0.2, .carac=pos1};
    // light 2
    lightSource_t light2 = {.type=LT_ambiant, .intensity=0.2};
    // light 3
    point_t pos3 = {2,1,-3};
    lightSource_t light3 = {.type=LT_point, .intensity=0.6, .carac=pos3};
    RT_initScene(&origin, 2, 2, 1);
    RT_addObject(&object1);
    RT_addObject(&object2);
    RT_addObject(&object3);
    RT_addObject(&object4);
    RT_addObject(&object5);
    RT_addObject(&object6);
    RT_addLight(&light1);
    RT_addLight(&light2);
    RT_addLight(&light3);
    printContext();
    RT_drawScene();
    DRAW_showRenderer();
    // Boucle pour garder la fenêtre ouverte
    printf("Done showing !\n");
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
    }

    DRAW_cleanRenderer();
    return 0;
}
