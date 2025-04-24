#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <time.h>
#include "draw.h"
#include "coordinates.h"
#include "software.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
SDL_Renderer* g_renderer;
#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    //init
    DRAW_initSDL(840, 840);
    DRAW_invertYAxis();
    DRAW_moveOrigin(g_windowWidth / 2, g_windowHeight / 2);
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
    point_t origin = {.x = 0, .y = 1, .z = 0};
    SW_initScene(&origin, 2, 2, 1);
    clock_t start = clock();
    SW_drawScene(&black);
    clock_t end = clock();
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Elapsed time : %f secondes\n", elapsed_time);
    // start main loop
    SDL_Event e;
    int quit = 0;
    float speedTurning = 30.0f / TARGET_FPS; // x° per second, n fps => x/n per frame
    float speedMoving = 4.0f / TARGET_FPS; // x unit per second, n fps => x/n per frame
    vector_t cameraSpeed = {};
    DRAW_showRenderer();
    while (!quit) {
        Uint32 frameStart = SDL_GetTicks();
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            
            if (e.type == SDL_KEYDOWN) {
                DRAW_clearRenderer();
                // clip speed
                cameraSpeed.x = 0;
                cameraSpeed.y = 0;
                cameraSpeed.z = 0;
                if(e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_SPACE){
                    cameraSpeed.y += speedMoving;
                }
                if(e.key.keysym.sym == SDLK_DOWN){
                    cameraSpeed.y -= speedMoving;
                }
                if(e.key.keysym.sym == SDLK_LEFT){
                    cameraSpeed.x -= speedMoving;
                }
                if(e.key.keysym.sym == SDLK_RIGHT){
                    cameraSpeed.x += speedMoving;
                }
                if(e.key.keysym.sym == SDLK_s){
                    cameraSpeed.z -= speedMoving;
                }
                if(e.key.keysym.sym == SDLK_z){
                    cameraSpeed.z += speedMoving;
                }
                if(e.key.keysym.sym == SDLK_q){
                    g_context.angleRotation[1] += speedTurning;
                }
                if(e.key.keysym.sym == SDLK_f){
                    g_context.angleRotation[1] -= speedTurning;
                }
                // rotate and move
                vector_t rotatedSpeed = cameraSpeed;
                COO_rotationVectorProduct(&rotatedSpeed, -g_context.angleRotation[0], -g_context.angleRotation[1], -g_context.angleRotation[2]);
                g_context.origin.x += rotatedSpeed.x;
                g_context.origin.y += rotatedSpeed.y;
                g_context.origin.z += rotatedSpeed.z;
                SW_drawScene(&black);
                DRAW_showRenderer();
            }
            
            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int newWidth = e.window.data1;
                    int newHeight = e.window.data2;
                    g_windowWidth = newHeight;
                    g_windowHeight = newHeight;
                    DRAW_moveOrigin(newWidth / 2, g_windowHeight / 2);
                    DRAW_clearRenderer();
                    SW_drawScene(&black);
                    DRAW_showRenderer();
                }
            }
        }
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    DRAW_cleanRenderer();
    SW_clearScene();
    printf("End\n");
    return 0;
}