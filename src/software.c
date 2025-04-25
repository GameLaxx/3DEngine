//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "software.h"
#include "interface.h"
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
                 ((float)g_pixelWidth / g_context.viewportWidth);
    ret_ptr->y = (point_ptr->y * g_context.viewportDistance) / point_ptr->z *
                 ((float)g_pixelHeight / g_context.viewportHeight);
    ret_ptr->z = g_context.viewportDistance;
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
    translatePoint(p1World_ptr, translateVector_ptr);
    translatePoint(p2World_ptr, translateVector_ptr);
    COO_rotationVectorProduct(p1World_ptr, g_context.angleRotation[0], g_context.angleRotation[1], g_context.angleRotation[2]);
    COO_rotationVectorProduct(p2World_ptr, g_context.angleRotation[0], g_context.angleRotation[1], g_context.angleRotation[2]);
    // clip front plan
    if(clipLine(p1World_ptr, p2World_ptr, &frontPlan, g_context.viewportDistance) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // clip back plan
    if(clipLine(p1World_ptr, p2World_ptr, &backPlan, -g_context.renderDistance) == EXIT_FAILURE){
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
    g_context.origin = *origin;
    g_context.viewportWidth = vW;
    g_context.viewportHeight = vH;
    g_context.viewportDistance = vD;
    g_context.renderDistance = 20;
    return EXIT_SUCCESS;
}

int SW_clearScene(){
    return EXIT_SUCCESS;
}

int SW_drawScene(){
    vector_t translateVector = {};
    COO_linearTransformation(&g_context.origin, -1, NULL, 0, &translateVector);
    rgba_t gray = {211,211,211,0};
    rgba_t red = {.red = 255};
    rgba_t green = {.green = 255};
    for(int z = g_context.origin.z - g_context.renderDistance; z <= g_context.origin.z + g_context.renderDistance; z++){
        // XZ
        point_t p1WorldXZ = {.x = g_context.origin.x - g_context.renderDistance, .y = 0, .z = z};
        point_t p2WorldXZ = {.x = g_context.origin.x + g_context.renderDistance, .y = 0, .z = z};
        drawGrid(&p1WorldXZ, &p2WorldXZ, &translateVector, &gray);
        // // YZ
        // point_t p1WorldYZ = {.x = 0, .y = g_context.origin.y - g_context.renderDistance, .z = z};
        // point_t p2WorldYZ = {.x = 0, .y = g_context.origin.y + g_context.renderDistance, .z = z};
        // drawGrid(&p1WorldYZ, &p2WorldYZ, &translateVector, &black);
    }
    // for(int y = g_context.origin.y - g_context.renderDistance; y <= g_context.origin.y + g_context.renderDistance; y++){
    //     // YZ plan
    //     point_t p1WorldYZ = {.x = 0, .y = y, .z = g_context.origin.z - g_context.renderDistance};
    //     point_t p2WorldYZ = {.x = 0, .y = y, .z = g_context.origin.z + g_context.renderDistance};
    //     drawGrid(&p1WorldYZ, &p2WorldYZ, &translateVector, &black);
    //     // XY plan
    //     point_t p1WorldXY = {.x = g_context.origin.x - g_context.renderDistance, .y = y, .z = 0};
    //     point_t p2WorldXY = {.x = g_context.origin.x + g_context.renderDistance, .y = y, .z = 0};
    //     drawGrid(&p1WorldXY, &p2WorldXY, &translateVector, (y == 0) ? &red : &black);
    // }
    int y = 0;
    point_t p1WorldXY = {.x = g_context.origin.x - g_context.renderDistance, .y = y, .z = 0};
    point_t p2WorldXY = {.x = g_context.origin.x + g_context.renderDistance, .y = y, .z = 0};
    drawGrid(&p1WorldXY, &p2WorldXY, &translateVector, (y == 0) ? &red : &gray);
    for(int x = g_context.origin.x - g_context.renderDistance; x <= g_context.origin.x + g_context.renderDistance; x++){
        // XZ plan
        point_t p1WorldXZ = {.x = x, .y = 0, .z = g_context.origin.z - g_context.renderDistance};
        point_t p2WorldXZ = {.x = x, .y = 0, .z = g_context.origin.z + g_context.renderDistance};
        drawGrid(&p1WorldXZ, &p2WorldXZ, &translateVector, (x == 0) ? &green : &gray);
        // // XY plan
        // point_t p1WorldXY = {.x = x, .y = g_context.origin.y - g_context.renderDistance, .z = 0};
        // point_t p2WorldXY = {.x = x, .y = g_context.origin.y + g_context.renderDistance, .z = 0};
        // drawGrid(&p1WorldXY, &p2WorldXY, &translateVector, &black);
    }
    
    return EXIT_SUCCESS;
}