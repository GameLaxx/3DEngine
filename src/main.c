#include <SDL2/SDL.h>
#include <stdio.h>
#include "draw.h"
#include "raytracing.h"
#include "print.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
SDL_Renderer* g_renderer;
// sceneContext_t g_context;
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
    rgba_t yellow = {255,0,255,255};
    rgba_t gray = {120,120,120,255};
    DRAW_invertYAxis();
    DRAW_moveOrigin(400,400);
    DRAW_clearRenderer();
    // Init context
    point_t origin = {0,0,0};
    // sphere 1
    point_t center1 = {0,-1,3};
    sphere_t sphere1 = {.center = center1, .color = red, .radius = 1};
    // sphere 2
    point_t center2 = {2,0,4};
    sphere_t sphere2 = {.center = center2, .color = yellow, .radius = 1};
    // sphere 3
    point_t center3 = {-2,0,4};
    sphere_t sphere3 = {.center = center3, .color = blue, .radius = 1};
    // sphere 4
    point_t center4 = {0,-5001,0};
    sphere_t sphere4 = {.center = center4, .color = gray, .radius = 5000};

    // light 1
    point_t pos1 = {1,4,4};
    lightSource_t light1 = {.type=LT_directional, .intensity=0.2, .carac=pos1};
    // light 2
    lightSource_t light2 = {.type=LT_ambiant, .intensity=0.2};
    // light 3
    point_t pos3 = {2,1,0};
    lightSource_t light3 = {.type=LT_point, .intensity=0.6, .carac=pos3};
    RT_initScene(&origin, 2, 2, 1);
    RT_addSphere(&sphere1);
    RT_addSphere(&sphere2);
    RT_addSphere(&sphere3);
    RT_addSphere(&sphere4);
    RT_addLight(&light1);
    RT_addLight(&light2);
    RT_addLight(&light3);
    printContext();
    RT_drawScene();
    DRAW_showRenderer();
    // Boucle pour garder la fenêtre ouverte
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
