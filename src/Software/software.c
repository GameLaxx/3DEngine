//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "software.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
sceneContext_t g_sceneContext;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
int point3DtoPixel(point_t* point_ptr, point_t* ret_ptr){
    if(!ret_ptr || !point_ptr){
        return EXIT_FAILURE;
    }
    if (point_ptr->z <= 0){
        return EXIT_FAILURE;
    }

    ret_ptr->x = (point_ptr->x * g_sceneContext.viewportDistance) / point_ptr->z *
                 ((float)g_pixelWidth / g_sceneContext.viewportWidth);
    ret_ptr->y = (point_ptr->y * g_sceneContext.viewportDistance) / point_ptr->z *
                 ((float)g_pixelHeight / g_sceneContext.viewportHeight);
    ret_ptr->z = g_sceneContext.viewportDistance;
    return EXIT_SUCCESS;
}

int clipLine(point_t* p1_ptr, point_t* p2_ptr, vector_t* plane_ptr, float offset){
    float d1 = COO_scalarProduct(p1_ptr, plane_ptr) - offset;
    float d2 = COO_scalarProduct(p2_ptr, plane_ptr) - offset;
    if(d1 < 0 && d2 < 0){
        return EXIT_FAILURE;
    }
    if(d1 >= 0 && d2 >= 0){
        return EXIT_SUCCESS;
    }
    float t = d1 / (d1 - d2); // d1 != d2 because conditions above
    point_t intersection = {};
    COO_linearTransformation(p1_ptr, 1 - t, p2_ptr, t, &intersection);
    if(d1 >= 0){ // p2 is behind the plane
        p2_ptr->x = intersection.x;
        p2_ptr->y = intersection.y;
        p2_ptr->z = intersection.z;
    }else{
        p1_ptr->x = intersection.x;
        p1_ptr->y = intersection.y;
        p1_ptr->z = intersection.z;
    }
    return EXIT_SUCCESS;
}

int drawGrid(point_t* p1World_ptr, point_t* p2World_ptr, vector_t* translateVector_ptr, rgba_t* color_ptr){
    vector_t frontPlan = {.z = 1};
    vector_t backPlan = {.z = -1};
    vector_t rightPlan = {.x = -0.7071, .z = 0.7071};
    vector_t leftPlan = {.x = 0.7071, .z = 0.7071};
    vector_t topPlan = {.y = -0.7071, .z = 0.7071};
    vector_t bottomPlan = {.y = 0.7071, .z = 0.7071};
    COO_translatePoint(p1World_ptr, translateVector_ptr);
    COO_translatePoint(p2World_ptr, translateVector_ptr);
    COO_rotationVectorProduct(p1World_ptr, g_sceneContext.angleRotation[0], g_sceneContext.angleRotation[1], g_sceneContext.angleRotation[2]);
    COO_rotationVectorProduct(p2World_ptr, g_sceneContext.angleRotation[0], g_sceneContext.angleRotation[1], g_sceneContext.angleRotation[2]);
    // clip front plan
    if(clipLine(p1World_ptr, p2World_ptr, &frontPlan, g_sceneContext.viewportDistance) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // clip back plan
    if(clipLine(p1World_ptr, p2World_ptr, &backPlan, -g_sceneContext.renderDistance) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // clip left plan
    if(clipLine(p1World_ptr, p2World_ptr, &leftPlan, 0) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // clip right plan
    if(clipLine(p1World_ptr, p2World_ptr, &rightPlan, 0) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // clip top plan
    if(clipLine(p1World_ptr, p2World_ptr, &topPlan, 0) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // clip bottom plan
    if(clipLine(p1World_ptr, p2World_ptr, &bottomPlan, 0) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    point_t p1Pixel = {};
    point_t p2Pixel = {};
    if(point3DtoPixel(p1World_ptr, &p1Pixel) == EXIT_FAILURE || 
    point3DtoPixel(p2World_ptr, &p2Pixel) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    DRAW_line(p1Pixel.x, p1Pixel.y, p2Pixel.x, p2Pixel.y, color_ptr);
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int SW_initScene(point_t* origin, int vW, int vH, int vD){
    g_sceneContext.origin = *origin;
    g_sceneContext.viewportWidth = vW;
    g_sceneContext.viewportHeight = vH;
    g_sceneContext.viewportDistance = vD;
    g_sceneContext.renderDistance = 20;
    return EXIT_SUCCESS;
}

int SW_clearScene(){
    return EXIT_SUCCESS;
}

int SW_drawScene(){
    vector_t translateVector = {};
    COO_linearTransformation(&g_sceneContext.origin, -1, NULL, 0, &translateVector);
    rgba_t gray = {211,211,211,0};
    rgba_t red = {.red = 255};
    rgba_t green = {.green = 255};
    for(int z = g_sceneContext.origin.z - g_sceneContext.renderDistance; z <= g_sceneContext.origin.z + g_sceneContext.renderDistance; z++){
        // XZ
        point_t p1WorldXZ = {.x = g_sceneContext.origin.x - g_sceneContext.renderDistance, .y = 0, .z = z};
        point_t p2WorldXZ = {.x = g_sceneContext.origin.x + g_sceneContext.renderDistance, .y = 0, .z = z};
        drawGrid(&p1WorldXZ, &p2WorldXZ, &translateVector, &gray);
        // // YZ
        // point_t p1WorldYZ = {.x = 0, .y = g_sceneContext.origin.y - g_sceneContext.renderDistance, .z = z};
        // point_t p2WorldYZ = {.x = 0, .y = g_sceneContext.origin.y + g_sceneContext.renderDistance, .z = z};
        // drawGrid(&p1WorldYZ, &p2WorldYZ, &translateVector, &black);
    }
    // for(int y = g_sceneContext.origin.y - g_sceneContext.renderDistance; y <= g_sceneContext.origin.y + g_sceneContext.renderDistance; y++){
    //     // YZ plan
    //     point_t p1WorldYZ = {.x = 0, .y = y, .z = g_sceneContext.origin.z - g_sceneContext.renderDistance};
    //     point_t p2WorldYZ = {.x = 0, .y = y, .z = g_sceneContext.origin.z + g_sceneContext.renderDistance};
    //     drawGrid(&p1WorldYZ, &p2WorldYZ, &translateVector, &black);
    //     // XY plan
    //     point_t p1WorldXY = {.x = g_sceneContext.origin.x - g_sceneContext.renderDistance, .y = y, .z = 0};
    //     point_t p2WorldXY = {.x = g_sceneContext.origin.x + g_sceneContext.renderDistance, .y = y, .z = 0};
    //     drawGrid(&p1WorldXY, &p2WorldXY, &translateVector, (y == 0) ? &red : &black);
    // }
    int y = 0;
    point_t p1WorldXY = {.x = g_sceneContext.origin.x - g_sceneContext.renderDistance, .y = y, .z = 0};
    point_t p2WorldXY = {.x = g_sceneContext.origin.x + g_sceneContext.renderDistance, .y = y, .z = 0};
    drawGrid(&p1WorldXY, &p2WorldXY, &translateVector, (y == 0) ? &red : &gray);
    for(int x = g_sceneContext.origin.x - g_sceneContext.renderDistance; x <= g_sceneContext.origin.x + g_sceneContext.renderDistance; x++){
        // XZ plan
        point_t p1WorldXZ = {.x = x, .y = 0, .z = g_sceneContext.origin.z - g_sceneContext.renderDistance};
        point_t p2WorldXZ = {.x = x, .y = 0, .z = g_sceneContext.origin.z + g_sceneContext.renderDistance};
        drawGrid(&p1WorldXZ, &p2WorldXZ, &translateVector, (x == 0) ? &green : &gray);
        // // XY plan
        // point_t p1WorldXY = {.x = x, .y = g_sceneContext.origin.y - g_sceneContext.renderDistance, .z = 0};
        // point_t p2WorldXY = {.x = x, .y = g_sceneContext.origin.y + g_sceneContext.renderDistance, .z = 0};
        // drawGrid(&p1WorldXY, &p2WorldXY, &translateVector, &black);
    }
    
    return EXIT_SUCCESS;
}