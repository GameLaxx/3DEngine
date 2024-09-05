//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "print.h"
#include "draw.h"
#include "raytracing.h"
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

void printRGBA(rgba_t* color_ptr){
    printf("Color : %i %i %i %i\n", color_ptr->red, color_ptr->green, color_ptr->blue, color_ptr->alpha);
}

void printSphere(sphere_t* sphere_ptr){
    printf("Sphere : %i\n", sphere_ptr->radius);
    printRGBA(&sphere_ptr->color);
    printCoo(&sphere_ptr->center);
}

void printContext(){
    printf("Context : %i %i %i, nSphere %i\n", g_context.viewportWidth, g_context.viewportHeight, g_context.viewportDistance, g_context.numSpheres);
    printCoo(&g_context.origin);
    for(int i = 0; i < MAX_SPHERES; i++){
        if(g_context.spheres[i].radius > 0) printSphere(&g_context.spheres[i]);
    }
}