//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "draw.h"
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
rgba_t g_whiteColor = {.red = 255, .green = 255, .blue = 255, .alpha = 255};
SDL_Window* window;

int windowWidth;
int windowHeight;

int xShift = 0;
int yShift = 0;
int invertY = 0;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
static int DRAW_setDrawColor(rgba_t* color_ptr){
    if(SDL_SetRenderDrawColor(g_renderer, color_ptr->red, color_ptr->green, color_ptr->blue, color_ptr->alpha) == -1){
        printf("Failed..\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static int DRAW_rectangle(int x, int y, int width, int height, rgba_t* color_ptr, rectangleFunction func){
    DRAW_setDrawColor(color_ptr);
    int renderY = y + yShift;
    if(invertY){
        renderY = windowHeight - renderY - height;
    }
    SDL_Rect rect = {x + xShift, renderY, width, height}; // x, y, largeur, hauteur
    return func(g_renderer, &rect);
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
/* Maintenance Functions */
int DRAW_initSDL(int width, int height){
    // Initialisation de la SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation de la SDL: %s\n", SDL_GetError());
        return 1;
    }
    windowWidth = width;
    windowHeight = height;

    // Création d'une fenêtre
    window = SDL_CreateWindow(
        "Canvas SDL",                     // Titre de la fenêtre
        SDL_WINDOWPOS_UNDEFINED,          // Position X de la fenêtre
        SDL_WINDOWPOS_UNDEFINED,          // Position Y de la fenêtre
        windowWidth, windowHeight,                         // Largeur et hauteur de la fenêtre
        SDL_WINDOW_SHOWN                 // Option pour montrer la fenêtre
    );

    if (window == NULL) {
        printf("Erreur lors de la création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Création d'un renderer pour dessiner
    g_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (g_renderer == NULL) {
        printf("Erreur lors de la création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
}

int DRAW_showRenderer(){
    SDL_RenderPresent(g_renderer);
}

int DRAW_clearRenderer(){
    DRAW_setDrawColor(&g_whiteColor);
    if(SDL_RenderClear(g_renderer) == -1){
        printf("Another fail..\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int DRAW_cleanRenderer(){
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/* Canvas Functions */
int DRAW_moveOrigin(int x, int y){
    xShift = x;
    yShift = y;
}

int DRAW_invertYAxis(){
    invertY = (1 + invertY) % 2;
}

/* Drawing Functions */
int DRAW_line(int x1, int y1, int x2, int y2, rgba_t* color_ptr){
    DRAW_setDrawColor(color_ptr);
    int renderY1 = y1 + yShift;
    int renderY2 = y2 + yShift;
    if(invertY){
        renderY1 = windowHeight - renderY1;
        renderY2 = windowHeight - renderY2;
    }
    SDL_RenderDrawLine(g_renderer, x1 + xShift, renderY1, x2  + xShift, renderY2); // (x1, y1) -> (x2, y2) in the current reference 
}

int DRAW_rectangleOutline(int x, int y, int width, int height, rgba_t* color_ptr){
    return DRAW_rectangle(x, y, width, height, color_ptr, SDL_RenderDrawRect);
}

int DRAW_rectangleFill(int x, int y, int width, int height, rgba_t* color_ptr){
    return DRAW_rectangle(x, y, width, height, color_ptr, SDL_RenderFillRect);
}

int DRAW_pixel(int x, int y, rgba_t* color_ptr){
    return DRAW_rectangle(x, y, 1, 1, color_ptr, SDL_RenderFillRect);
}