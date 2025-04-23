//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "software.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
sceneContext_t g_context;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
int translatePoint(point_t* point_ptr, vector_t* vector_ptr){
    point_ptr->x += vector_ptr->x;
    point_ptr->y += vector_ptr->y;
    point_ptr->z += vector_ptr->z;
    return EXIT_SUCCESS;
}

int point3DtoPixel(point_t* point_ptr, point_t* ret_ptr){
    if(!ret_ptr || !point_ptr){
        return EXIT_FAILURE;
    }
    if (point_ptr->z <= 0){
        return EXIT_FAILURE;
    }

    ret_ptr->x = (point_ptr->x * g_context.viewportDistance) / point_ptr->z *
                 ((float)g_windowWidth / g_context.viewportWidth);
    ret_ptr->y = (point_ptr->y * g_context.viewportDistance) / point_ptr->z *
                 ((float)g_windowHeight / g_context.viewportHeight);
    ret_ptr->z = g_context.viewportDistance;
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int SW_initScene(point_t* origin, int vW, int vH, int vD){
    g_context.origin = *origin;
    g_context.viewportWidth = vW;
    g_context.viewportHeight = vH;
    g_context.viewportDistance = vD;
    g_context.renderDistance = 5;
    return EXIT_SUCCESS;
}

int SW_clearScene(){
    return EXIT_SUCCESS;
}

int SW_drawScene(rgba_t* color_ptr){
    int xBound = 2 * g_context.renderDistance / g_context.viewportWidth;
    int zBound = g_context.renderDistance * g_context.viewportDistance;
    vector_t translateVector = {};
    COO_linearTransformation(&g_context.origin, -1, NULL, 0, &translateVector);
    for(int x = g_context.origin.x - xBound; x <= g_context.origin.x + xBound; x++){
        // float z = fmax(g_context.origin.z + g_context.viewportDistance, 2 * g_context.viewportDistance * abs(g_context.origin.x - abs(x)) / g_context.viewportWidth);
        point_t p1World = {.x = x, .y = 0, .z = g_context.origin.z + 0.1};
        point_t p2World = {.x = x, .y = 0, .z = g_context.origin.z + zBound};
        translatePoint(&p1World, &translateVector);
        translatePoint(&p2World, &translateVector);
        point_t p1Pixel = {};
        point_t p2Pixel = {};
        if(point3DtoPixel(&p1World, &p1Pixel) == EXIT_FAILURE || 
        point3DtoPixel(&p2World, &p2Pixel) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        DRAW_line(p1Pixel.x, p1Pixel.y, p2Pixel.x, p2Pixel.y, color_ptr);
    }
    return EXIT_SUCCESS;
}