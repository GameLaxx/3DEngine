#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <time.h>
#include "draw.h"
#include "coordinates.h"
#include "interface.h"
#include "software.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
SDL_Renderer* g_renderer;
#define TARGET_FPS 32
#define FRAME_DELAY (1000 / TARGET_FPS)
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
int updateSpeeds(const Uint8* keystates, vector_t* cameraMoving_ptr, vector_t* cameraTurning_ptr, float speedMoving, float speedTurning){
    if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_SPACE]) {
        cameraMoving_ptr->y += speedMoving;
    }
    if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_LSHIFT]) {
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

int updateScene(vector_t* cameraMoving_ptr, vector_t* cameraTurning_ptr, 
    float speedMoving, float speedTurning, float speedFade, float speedCeil){
    DRAW_clearRenderer();
    cameraMoving_ptr->x = fmaxf(fminf(2 * speedMoving, cameraMoving_ptr->x), -2 * speedMoving);
    cameraMoving_ptr->y = fmaxf(fminf(2 * speedMoving, cameraMoving_ptr->y), -2 * speedMoving);
    cameraMoving_ptr->z = fmaxf(fminf(2 * speedMoving, cameraMoving_ptr->z), -2 * speedMoving);
    cameraTurning_ptr->y = fmaxf(fminf(2 * speedTurning, cameraTurning_ptr->y), -2 * speedTurning);
    // rotate and move
    vector_t rotatedSpeed = *cameraMoving_ptr;
    COO_rotationVectorProduct(&rotatedSpeed, -g_sceneContext.angleRotation[0], -g_sceneContext.angleRotation[1], -g_sceneContext.angleRotation[2]);
    g_sceneContext.origin.x += rotatedSpeed.x;
    g_sceneContext.origin.y += rotatedSpeed.y;
    g_sceneContext.origin.z += rotatedSpeed.z;
    g_sceneContext.angleRotation[1] += cameraTurning_ptr->y;
    SW_drawScene();
    DRAW_showRenderer();
    // clip speed
    cameraMoving_ptr->x /= speedFade;
    cameraMoving_ptr->y /= speedFade;
    cameraMoving_ptr->z /= speedFade;
    cameraTurning_ptr->y /= speedFade;
    if(fabs(cameraMoving_ptr->x) < speedCeil){
        cameraMoving_ptr->x = 0;
    }
    if(fabs(cameraMoving_ptr->y) < speedCeil){
        cameraMoving_ptr->y = 0;
    }
    if(fabs(cameraMoving_ptr->y) < speedCeil){
        cameraMoving_ptr->y = 0;
    }
    if(fabs(cameraTurning_ptr->y) < speedCeil){
        cameraTurning_ptr->y = 0;
    }
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    //init
    DRAW_initSDL(1080, 600);
    DRAW_invertYAxis();
    DRAW_moveOrigin(g_windowWidth / 2, g_windowHeight / 2);
    DRAW_clearRenderer();
    // rgba_t red = {255,0,0,255};
    rgba_t blue = {0,0,255,255};
    // rgba_t green = {0,255,0,255};
    // rgba_t yellow = {255,255,0,255};
    // rgba_t purple = {255,0,255,255};
    // rgba_t brown = {255,160,0,255};
    rgba_t light_gray = {160,160,160,255};
    // rgba_t dark_gray = {80,80,80,255};
    // rgba_t white = {255,255,255,255};
    // rgba_t black = {0,0,0,255};
    // Define scene variables
    point_t origin = {.x = 0, .y = 1, .z = 0};
    SW_initScene(&origin, 2, 2, 1);
    IF_initInterface();
    clock_t start = clock();
    SW_drawScene();
    IF_drawInterface();
    clock_t end = clock();
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Elapsed time : %f secondes\n", elapsed_time);
    // define cursors
    SDL_Cursor *arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_Cursor *clickCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    // define speeds
    float speedCeil = 0.0001;
    float speedTurning = 30.0f / TARGET_FPS; // x° per second, n fps => x/n per frame
    float speedMoving = 4.0f / TARGET_FPS; // x unit per second, n fps => x/n per frame
    float speedFade = pow(2.0f * speedMoving / speedCeil, 1.0f / TARGET_FPS); // from 2 speedMoving to 0 in n seconds
    vector_t cameraMovingSpeed = {};
    vector_t cameraTurningSpeed = {};
    point_t lastMousePos = {};
    // start main loop
    SDL_Event e;
    int quit = 0;
    int sliding = 0; // allow to know if currently sliding with mouse
    DRAW_showRenderer();        
    const Uint8* keystates = SDL_GetKeyboardState(NULL); // get keys pressed in real time
    while (!quit) {
        Uint32 frameStart = SDL_GetTicks();
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            
            if(e.type == SDL_MOUSEBUTTONUP){
                if(e.button.button == SDL_BUTTON_LEFT){
                    sliding = 0;
                }
            }
            if(e.type == SDL_MOUSEBUTTONDOWN){
                if (e.button.button == SDL_BUTTON_LEFT) {
                    if(e.button.x > (g_windowWidth - g_pixelWidth) / 2 && e.button.x < (g_windowWidth + g_pixelWidth) / 2){
                        sliding = 1;
                    }
                    lastMousePos.x = e.button.x;
                    lastMousePos.y = e.button.y;
                    int meshId = IF_clickMeshBox(lastMousePos.x, lastMousePos.y);
                    if(meshId > -1){
                        object_t object = {.meshId = meshId, .materialType = MT_COLOR_UNIFORM, .material_ptr = &light_gray, .scale = {1,1,1}};
                        SW_addObject(&object);
                    }
                }
            }

            if(e.type == SDL_MOUSEMOTION){
                if (e.motion.state & SDL_BUTTON_LMASK && sliding) {
                    if(fabs(e.motion.x - lastMousePos.x) <= 1 && fabs(e.motion.y - lastMousePos.y) >= 1){
                        cameraMovingSpeed.y += speedMoving * (e.motion.y - lastMousePos.y);
                    }else if(fabs(e.motion.x - lastMousePos.x) > 1){
                        cameraMovingSpeed.z += speedMoving * (e.motion.y - lastMousePos.y);
                        cameraMovingSpeed.x -= speedMoving * (e.motion.x - lastMousePos.x); // -= to go against mouse
                    }
                    lastMousePos.x = e.motion.x;
                    lastMousePos.y = e.motion.y;
                }
                SDL_SetCursor((IF_hoverMeshBox(e.motion.x, e.motion.y) == EXIT_SUCCESS) ? clickCursor : arrowCursor);
            }
            
            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int newWidth = e.window.data1;
                    int newHeight = e.window.data2;
                    g_windowWidth = newWidth;
                    g_windowHeight = newHeight;
                    g_pixelWidth = newHeight;
                    g_pixelHeight = newHeight;
                    DRAW_moveOrigin(g_windowWidth / 2, g_windowHeight / 2);
                    DRAW_clearRenderer();
                    IF_updateInterface();
                    IF_drawInterface();
                    SW_drawScene();
                    DRAW_showRenderer();
                }
            }
        }
        updateSpeeds(keystates, &cameraMovingSpeed, &cameraTurningSpeed, speedMoving, speedTurning);
        if(cameraMovingSpeed.x != 0 || cameraMovingSpeed.y != 0 || cameraMovingSpeed.z != 0||
           cameraTurningSpeed.x != 0 || cameraTurningSpeed.y != 0 || cameraTurningSpeed.z != 0
        ){
            updateScene(&cameraMovingSpeed, &cameraTurningSpeed, speedMoving, speedTurning, speedFade, speedCeil);
        }
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    DRAW_cleanRenderer();
    SW_clearScene();
    IF_cleanInterface();
    printf("End\n");
    return 0;
}