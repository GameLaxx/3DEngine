//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "draw.h"
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
rgba_t g_backgroundColor = {.red = 100, .green = 100, .blue = 100, .alpha = 255};
SDL_Window* window;
TTF_Font* font;

int g_windowWidth = 0;
int g_windowHeight = 0;
int g_pixelWidth = 0;
int g_pixelHeight = 0;
int g_xShift = 0;
int g_yShift = 0;

int invertY = 0;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
/**
 * @brief Set the canvas to draw with a given color.
 * 
 * @param color_ptr The color.
 * @return int 
 */
static int DRAW_setDrawColor(rgba_t* color_ptr){
    if(SDL_SetRenderDrawColor(g_renderer, color_ptr->red, color_ptr->green, color_ptr->blue, color_ptr->alpha) == -1){
        printf("Failed..\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief General function to draw any type of rectangle.
 * 
 * @param x X coordinate of the origin.
 * @param y Y coordinate of the origin.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 * @param color_ptr Color of the rectangle.
 * @param func Either SDL_RenderFillRect or SDL_RenderDrawRect.
 * @return int 
 */
static int DRAW_rectangle(int x, int y, int width, int height, rgba_t* color_ptr, rectangleFunction func){
    DRAW_setDrawColor(color_ptr);
    int renderY = y + g_yShift;
    if(invertY){
        renderY = g_pixelHeight - renderY - height;
    }
    SDL_Rect rect = {x + g_xShift, renderY, width, height}; // x, y, largeur, hauteur
    return func(g_renderer, &rect);
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
/* Maintenance Functions */
int DRAW_initSDL(int width, int height){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error while initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
    g_pixelWidth = height;
    g_pixelHeight = height;
    g_windowWidth = width;
    g_windowHeight = height;
    // create the window
    window = SDL_CreateWindow(
        "3D Engine", // title
        SDL_WINDOWPOS_UNDEFINED, // X pos of the window
        SDL_WINDOWPOS_UNDEFINED, // Y pos of the window
        g_windowWidth, g_windowHeight, // Sizes of the window
        SDL_WINDOW_SHOWN // Show option
    );
    if (window == NULL) {
        printf("Error while creating the window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    g_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (g_renderer == NULL) {
        printf("Error while creating the renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    TTF_Init(); // init text operations
    font = TTF_OpenFont("./style/arial.ttf", 16);
    return 0;
}

int DRAW_showRenderer(){
    SDL_RenderPresent(g_renderer);
    return 0;
}

int DRAW_clearRenderer(){
    DRAW_rectangleFill((g_windowWidth - g_pixelWidth) / 2 - g_xShift, -g_yShift, g_pixelWidth, g_pixelHeight, &g_backgroundColor);
    return EXIT_SUCCESS;
}

int DRAW_cleanRenderer(){
    TTF_CloseFont(font);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

/* Canvas Functions */
int DRAW_moveOrigin(int x, int y){
    g_xShift = x;
    g_yShift = y;
    return 0;
}

int DRAW_invertYAxis(){
    invertY = (1 + invertY) % 2;
    return 0;
}

/* Color functions */
int DRAW_initBackgroundColor(rgba_t* ret_ptr){
    ret_ptr->red = g_backgroundColor.red;
    ret_ptr->green = g_backgroundColor.green;
    ret_ptr->blue = g_backgroundColor.blue;
    return EXIT_SUCCESS;
}

int DRAW_addIntensity(rgba_t* color_ptr, float intensity, rgba_t* ret_ptr){
    if(intensity < 0){
        return EXIT_SUCCESS;
    }
    float red = (float) color_ptr->red * intensity;
    ret_ptr->red = ((int) red > 255) ? 255 : (int) red;
    float green = (float) color_ptr->green * intensity;
    ret_ptr->green = ((int) green > 255) ? 255 : (int) green;
    float blue = (float) color_ptr->blue * intensity;
    ret_ptr->blue = ((int) blue > 255) ? 255 : (int) blue;
    return EXIT_SUCCESS;
}

void DRAW_computeReflection(rgba_t* localColor_ptr, rgba_t* recursiveColor_ptr, float reflection){
    if(recursiveColor_ptr == NULL) return;
    localColor_ptr->red = (int) (localColor_ptr->red * (1 - reflection) + recursiveColor_ptr->red * reflection);
    localColor_ptr->green = (int) (localColor_ptr->green * (1 - reflection) + recursiveColor_ptr->green * reflection);
    localColor_ptr->blue = (int) (localColor_ptr->blue * (1 - reflection) + recursiveColor_ptr->blue * reflection);
}

/* Drawing Functions */
int DRAW_line(int x1, int y1, int x2, int y2, rgba_t* color_ptr){
    DRAW_setDrawColor(color_ptr);
    int renderY1 = y1 + g_yShift;
    int renderY2 = y2 + g_yShift;
    if(invertY){
        renderY1 = g_pixelHeight - renderY1;
        renderY2 = g_pixelHeight - renderY2;
    }
    SDL_RenderDrawLine(g_renderer, x1 + g_xShift, renderY1, x2  + g_xShift, renderY2); // (x1, y1) -> (x2, y2) in the current reference 
    return 0;
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

int DRAW_texte(int x, int y, char* content_ptr, rgba_t* color_ptr){
    SDL_Color textColor = {.r = color_ptr->red, .g = color_ptr->green, .b = color_ptr->blue, .a = 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, content_ptr, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(g_renderer, textSurface);
    SDL_Rect textRect;
    textRect.x = x + g_xShift;
    textRect.y = g_windowHeight - (y + g_yShift);
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    SDL_RenderCopy(g_renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    return EXIT_SUCCESS;
}