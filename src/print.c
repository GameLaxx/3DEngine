//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "print.h"
#include "draw.h"
#include "raytracing.h"
#include "objects.h"
#include "coordinates.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
void printCoo(point_t* point_ptr){
    printf("Point : %f %f %f\n", point_ptr->x, point_ptr->y, point_ptr->z);
}

void printVector(vector_t* vector_ptr){
    printf("Vector : %f %f %f\n", vector_ptr->x, vector_ptr->y, vector_ptr->z);
}

void printRGBA(rgba_t* color_ptr){
    printf("Color : %i %i %i %i\n", color_ptr->red, color_ptr->green, color_ptr->blue, color_ptr->alpha);
}

void printSphere(sphere_t* sphere_ptr){
    printf("Sphere : %f\n", sphere_ptr->radius);
    printCoo(&sphere_ptr->center);
}

void printCube(cube_t* cube_ptr){
    printf("Cube : Rx %f° Ry %f° Rz %f°\n", cube_ptr->rotateX, cube_ptr->rotateY, cube_ptr->rotateZ);
    printCoo(&cube_ptr->center);
    printVector(&cube_ptr->extendVector);
}

void printCylinder(cylinder_t* cylinder_ptr){
    printf("Cylinder : Rx %f° Rz %f°\n", cylinder_ptr->rotateX, cylinder_ptr->rotateZ);
    printf("Carac : radius %f height %f\n", cylinder_ptr->radius, cylinder_ptr->height);
}

void printObject(object_t* object_ptr){
    if(object_ptr->type == OT_sphere){
        printSphere(object_ptr->content_ptr);
        return;
    }
    if(object_ptr->type == OT_cube){
        printCube(object_ptr->content_ptr);
        return;
    }
    if(object_ptr->type != OT_NAO){
        printf("Unknown type : %i\n", object_ptr->type);
        return;
    }
}

void printLight(lightSource_t* light_ptr){
    printf("Light : %i %f\n", light_ptr->type, light_ptr->intensity);
    if(light_ptr->type != LT_ambiant) printCoo(&light_ptr->carac);
}

void printContext(){
    printf("Context : %i %i %i, nObjects %i, nLights %i\n",
        g_context.viewportWidth, g_context.viewportHeight, g_context.viewportDistance,
        g_context.numObjects, g_context.numLights);
    printCoo(&g_context.origin);
    printf("----- Objects\n");
    for(int i = 0; i < MAX_OBJECTS; i++){
        printObject(&g_context.objects[i]);
    }
    printf("----- Lights\n");
    for(int i = 0; i < MAX_LIGHTS; i++){
        if(g_context.lights[i].intensity > 0) printLight(&g_context.lights[i]);
    }
}