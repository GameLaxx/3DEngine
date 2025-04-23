#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include "draw.h"

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
    DRAW_initSDL(1080, 1080);
    DRAW_invertYAxis();
    DRAW_moveOrigin(1080 / 2, 1080 / 2);
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
    char cubePath[] = "ressources/cube.obj";
    OBJ_readObjFile(cubePath, 0, &cubeMesh);
    if(RR_addMesh(&cubeMesh) == EXIT_FAILURE){
        printf("*-* Failed while adding mesh.\n");
        return -1;
    }
    mesh_t pyramidMesh = {};
    char pyramidPath[] = "ressources/pyramid.obj";
    OBJ_readObjFile(pyramidPath, 1, &pyramidMesh);
    if(RR_addMesh(&pyramidMesh) == EXIT_FAILURE){
        printf("*-* Failed while adding mesh.\n");
        return -1;
    }
    mesh_t sphereMesh = {};
    char spherePath[] = "ressources/sphere.obj";
    OBJ_readObjFile(spherePath, 3, &sphereMesh);
    if(RR_addMesh(&sphereMesh) == EXIT_FAILURE){
        printf("*-* Failed while adding mesh.\n");
        return -1;
    }
    mesh_t alMesh = {};
    char alPath[] = "ressources/al_f.obj";
    OBJ_readObjFile(alPath, 2, &alMesh);
    if(RR_addMesh(&alMesh) == EXIT_FAILURE){
        printf("*-* Failed while adding mesh.\n");
        return -1;
    }
    mesh_t diamondMesh = {};
    char diamondPath[] = "ressources/diamond.obj";
    OBJ_readObjFile(diamondPath, 4, &diamondMesh);
    if(RR_addMesh(&diamondMesh) == EXIT_FAILURE){
        printf("*-* Failed while adding mesh.\n");
        return -1;
    }
    mesh_t squareMesh = {};
    char squarePath[] = "ressources/square.obj";
    OBJ_readObjFile(squarePath, 5, &squareMesh);
    if(RR_addMesh(&squareMesh) == EXIT_FAILURE){
        printf("*-* Failed while adding mesh.\n");
        return -1;
    }
    // Add lights
    point_t pos1 = {1,4,-4};
    lightSource_t light1 = {.type=LT_directional, .intensity=0.2, .carac=pos1};
    lightSource_t light2 = {.type=LT_ambiant, .intensity=0.7};
    point_t pos3 = {2,1,-3};
    lightSource_t light3 = {.type=LT_point, .intensity=0.1, .carac=pos3};
    RR_addLight(&light1);
    RR_addLight(&light2);
    RR_addLight(&light3);
    // Draw on the canvas
    point_t originCube1 = {.x = 0, .y = 0, .z = 4.5};
    rgba_t colors1 = red;
    object_t object1 = {
        .origin = originCube1, .meshId = 0, 
        .materialType = MT_COLOR_UNIFORM, .material_ptr = &colors1, 
        .scale = {1,1,1}, .angleRotation = {25,25,0}
    }; 
    if(RR_addObject(&object1) == EXIT_FAILURE){
        printf("*-* Failed while adding object.\n");
        return -1;
    }
    point_t originDiam1 = {.x = 3, .y = 0, .z = 4.5};
    rgba_t colors4 = purple;
    object_t object4 = {
        .origin = originDiam1, .meshId = 4, 
        .materialType = MT_COLOR_UNIFORM, .material_ptr = &colors4, 
        .scale = {1,2,1}, .angleRotation = {0,0,0}
    }; 
    if(RR_addObject(&object4) == EXIT_FAILURE){
        printf("*-* Failed while adding object.\n");
        return -1;
    }
    // point_t originPyramid1 = {.x = 0, .y = 0, .z = 4.5};
    // rgba_t colors2 = white;
    // object_t object2 = {
    //     .origin = originPyramid1, .meshId = 1, 
    //     .materialType = MT_COLOR_UNIFORM, .material_ptr = &colors2, 
    //     .scale = {1,1,1}, .angleRotation = {-70, 45, 0}};
    // if(RR_addObject(&object2) == EXIT_FAILURE){
    //     printf("*-* Failed while adding object.\n");
    //     return -1;
    // }
    // point_t originSphere1 = {.x = 0, .y = 0, .z = 4.5};
    // rgba_t colors4 = white;
    // object_t object4 = {
    //     .origin = originSphere1, .meshId = 3, 
    //     .materialType = MT_COLOR_UNIFORM, .material_ptr = &colors4, 
    //     .scale = {1,1,1}, .angleRotation = {0,-10,0}};
    // if(RR_addObject(&object4) == EXIT_FAILURE){
    //     printf("*-* Failed while adding object.\n");
    //     return -1;
    // }
    // point_t originAl1 = {.x = 0, .y = 0, .z = 4.5};
    // rgba_t colors3 = white;
    // object_t object3 = {
    //     .origin = originAl1, .meshId = 2, 
    //     .materialType = MT_COLOR_UNIFORM, .material_ptr = &colors3, 
    //     .scale = {1,1,1}, .angleRotation = {0,180,0}}; 
    // if(RR_addObject(&object3) == EXIT_FAILURE){
    //     printf("*-* Failed while adding object.\n");
    //     return -1;
    // }
    // point_t originSquare1 = {.x = 0, .y = 0, .z = 4.5};
    // rgba_t colors5 = yellow;
    // object_t object5 = {
    //     .origin = originSquare1, .meshId = 5, 
    //     .materialType = MT_COLOR_UNIFORM, .material_ptr = &colors5, 
    //     .scale = {1,1,1}, .angleRotation = {0,50,0}
    // }; 
    // if(RR_addObject(&object5) == EXIT_FAILURE){
    //     printf("*-* Failed while adding object.\n");
    //     return -1;
    // }
    // Main loop to keep the window open
    clock_t start = clock();
    RR_drawScene();
    clock_t end = clock();
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Elapsed time : %f secondes\n", elapsed_time);
    DRAW_showRenderer();
    SDL_Event e;
    int quit = 0;
    float speed = 2.0f / TARGET_FPS; // x unit per second, n fps => x/n per frame
    while (!quit) {
        Uint32 frameStart = SDL_GetTicks();
        DRAW_clearRenderer();
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        g_context.origin.y += speed;
                        break;
                    case SDLK_DOWN:
                        g_context.origin.y -= speed;
                        break;
                    case SDLK_LEFT:
                        g_context.origin.x -= speed;
                        break;
                    case SDLK_RIGHT:
                        g_context.origin.x += speed;
                        break;
                    case SDLK_s:
                        g_context.origin.z -= speed;
                        break;
                    case SDLK_z:
                        g_context.origin.z += speed;
                        break;
                }
            }
            
            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int newWidth = e.window.data1;
                    int newHeight = e.window.data2;
                    printf("New size window : %d x %d\n", newWidth, newHeight);
                }
            }
        }
        g_context.objects[0].angleRotation[1] += 1;
        g_context.objects[1].angleRotation[1] -= 1;
        RR_drawScene();
        DRAW_showRenderer();
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    DRAW_cleanRenderer();
    RR_clearScene();
    printf("End\n");
    return 0;
}