#include <SDL2/SDL.h>
#include <stdio.h>
#include "draw.h"

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
    rgba_t black = {0,0,0,255};
    // Afficher le canvas
    DRAW_clearRenderer();
    DRAW_line(0,400,800,400, &black);
    DRAW_line(400,0,400,800, &black);
    DRAW_rectangleFill(0,0,50,50, &black);
    DRAW_invertYAxis();
    DRAW_rectangleFill(0,0,50,50, &black);
    DRAW_moveOrigin(400,400);
    DRAW_rectangleFill(0,0,50,50, &black);
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
