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
    // Draw on the canvas
    point_t p1 = {.x = -1000, .y = -1000, .z = 5};
    point_t p2 = {.x = 0, .y = 1000, .z = 5};
    point_t p3 = {.x = 1000, .y = -1000, .z = 5};
    triangle_t triangle = {.p1 = p1, .p2 = p2, .p3 = p3, .color = black};
    RR_fillTriangle(&triangle);
    // Main loop to keep the window open
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
    return 0;
}
