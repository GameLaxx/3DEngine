#include <SDL2/SDL.h>
#include <stdio.h>
#include "coordinates.h"
#include "draw.h"
#include "rasterization.h"
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
    DRAW_invertYAxis();
    DRAW_moveOrigin(400,400);
    DRAW_clearRenderer();
    rgba_t red = {255,0,0,255};
    rgba_t blue = {0,0,255,255};
    rgba_t green = {0,255,0,255};
    rgba_t yellow = {255,255,0,255};
    rgba_t purple = {255,0,255,255};
    rgba_t brown = {255,160,0,255};
    rgba_t light_gray = {160,160,160,255};
    rgba_t dark_gray = {80,80,80,255};
    rgba_t white = {255,255,255,255};
    rgba_t black = {0,0,0,255};
    // Define scene variables
    point_t origin = {.x = 0, .y = 0, .z = 0};
    RR_initScene(&origin, 2, 2, 1);
    // Define meshes
    mesh_t cubeMesh = {};
    OBJ_createCubeMesh(&cubeMesh);
    if(RR_addMesh(&cubeMesh) == EXIT_FAILURE){
        printf("*-* Failed while adding mesh.\n");
        return -1;
    }
    // Draw on the canvas
    point_t originCube1 = {.x = 1, .y = 2, .z = 5};
    object_t object1 = {.origin = originCube1, .meshId = 0, .materialType = MT_COLOR, .material_ptr = &red, .scale = {2,2,2}}; 
    if(RR_addObject(&object1) == EXIT_FAILURE){
        printf("*-* Failed while adding object.\n");
        return -1;
    }
    // Main loop to keep the window open
    RR_drawScene();
    DRAW_showRenderer();
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
    RR_clearScene();
    printf("End\n");
    return 0;
}
