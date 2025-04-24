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
int updateSpeeds(const Uint8* keystates, vector_t* cameraMoving_ptr, vector_t* cameraTurning_ptr, float speedMoving, float speedTurning){
    if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_SPACE]) {
        cameraMoving_ptr->y += speedMoving;
    }
    if (keystates[SDL_SCANCODE_DOWN]) {
        cameraMoving_ptr->y -= speedMoving;
    }
    if (keystates[SDL_SCANCODE_LEFT]) {
        cameraMoving_ptr->x -= speedMoving;
    }
    if (keystates[SDL_SCANCODE_RIGHT]) {
        cameraMoving_ptr->x += speedMoving;
    }
    if (keystates[SDL_SCANCODE_S]) {
        cameraMoving_ptr->z -= speedMoving;
    }
    if (keystates[SDL_SCANCODE_W]) {
        cameraMoving_ptr->z += speedMoving;
    }
    if (keystates[SDL_SCANCODE_A]) {
        cameraTurning_ptr->y += speedTurning;
    }
    if (keystates[SDL_SCANCODE_F]) {
        cameraTurning_ptr->y -= speedTurning;
    }
    return EXIT_SUCCESS;
}

int updateScene(vector_t* cameraMoving_ptr, vector_t* cameraTurning_ptr, float speedMoving, float speedTurning, rgba_t* color_ptr){
    DRAW_clearRenderer();
    cameraMoving_ptr->x = fmaxf(fminf(2 * speedMoving, cameraMoving_ptr->x), -2 * speedMoving);
    cameraMoving_ptr->y = fmaxf(fminf(2 * speedMoving, cameraMoving_ptr->y), -2 * speedMoving);
    cameraMoving_ptr->z = fmaxf(fminf(2 * speedMoving, cameraMoving_ptr->z), -2 * speedMoving);
    cameraTurning_ptr->y = fmaxf(fminf(2 * speedTurning, cameraTurning_ptr->y), -2 * speedTurning);
    // rotate and move
    vector_t rotatedSpeed = *cameraMoving_ptr;
    COO_rotationVectorProduct(&rotatedSpeed, -g_context.angleRotation[0], -g_context.angleRotation[1], -g_context.angleRotation[2]);
    g_context.origin.x += rotatedSpeed.x;
    g_context.origin.y += rotatedSpeed.y;
    g_context.origin.z += rotatedSpeed.z;
    g_context.angleRotation[1] += cameraTurning_ptr->y;
    SW_drawScene(color_ptr);
    DRAW_showRenderer();
    // clip speed
    cameraMoving_ptr->x /= 1.05;
    cameraMoving_ptr->y /= 1.05;
    cameraMoving_ptr->z /= 1.05;
    cameraTurning_ptr->y /= 1.05;
    if(fabs(cameraMoving_ptr->x) < 0.0001){
        cameraMoving_ptr->x = 0;
    }
    if(fabs(cameraMoving_ptr->y) < 0.0001){
        cameraMoving_ptr->y = 0;
    }
    if(fabs(cameraMoving_ptr->y) < 0.0001){
        cameraMoving_ptr->y = 0;
    }
    if(fabs(cameraTurning_ptr->y) < 0.0001){
        cameraTurning_ptr->y = 0;
    }
    return EXIT_SUCCESS;
}
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
    vector_t cameraMovingSpeed = {};
    vector_t cameraTurningSpeed = {};
    point_t lastMousePos = {};
    DRAW_showRenderer();        
    const Uint8* keystates = SDL_GetKeyboardState(NULL); // get keys pressed in real time
    while (!quit) {
        Uint32 frameStart = SDL_GetTicks();
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            
            if(e.type == SDL_MOUSEBUTTONDOWN){
                if (e.button.button == SDL_BUTTON_LEFT) {
                    lastMousePos.x = e.button.x;
                    lastMousePos.y = e.button.y;
                }
            }

            if(e.type == SDL_MOUSEMOTION){
                if (e.motion.state & SDL_BUTTON_LMASK) {
                    if(fabs(e.motion.x - lastMousePos.x) <= 1 && fabs(e.motion.y - lastMousePos.y) >= 1){
                        cameraMovingSpeed.y += speedMoving * (e.motion.y - lastMousePos.y);
                    }else if(fabs(e.motion.x - lastMousePos.x) > 1){
                        cameraMovingSpeed.z += speedMoving * (e.motion.y - lastMousePos.y);
                        cameraMovingSpeed.x -= speedMoving * (e.motion.x - lastMousePos.x); // -= to go against mouse
                    }
                    lastMousePos.x = e.motion.x;
                    lastMousePos.y = e.motion.y;
                }
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
        updateSpeeds(keystates, &cameraMovingSpeed, &cameraTurningSpeed, speedMoving, speedTurning);
        if(cameraMovingSpeed.x != 0 || cameraMovingSpeed.y != 0 || cameraMovingSpeed.z != 0||
           cameraTurningSpeed.x != 0 || cameraTurningSpeed.y != 0 || cameraTurningSpeed.z != 0
        ){
            updateScene(&cameraMovingSpeed, &cameraTurningSpeed, speedMoving, speedTurning, &black);
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