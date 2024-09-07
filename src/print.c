//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "print.h"
#include "draw.h"
#include "raytracing.h"
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

void printRGBA(rgba_t* color_ptr){
    printf("Color : %i %i %i %i\n", color_ptr->red, color_ptr->green, color_ptr->blue, color_ptr->alpha);
}

void printSphere(sphere_t* sphere_ptr){
    printf("Sphere : %f\n", sphere_ptr->radius);
    printRGBA(&sphere_ptr->color);
    printCoo(&sphere_ptr->center);
}

void printLight(lightSource_t* light_ptr){
    printf("Light : %i %f\n", light_ptr->type, light_ptr->intensity);
    if(light_ptr->type != LT_ambiant) printCoo(&light_ptr->carac);
}

void printContext(){
    printf("Context : %i %i %i, nSphere %i, nLights %i\n",
        g_context.viewportWidth, g_context.viewportHeight, g_context.viewportDistance,
        g_context.numSpheres, g_context.numLights);
    printCoo(&g_context.origin);
    printf("----- Spheres\n");
    for(int i = 0; i < MAX_ELEMENTS; i++){
        if(g_context.spheres[i].radius > 0) printSphere(&g_context.spheres[i]);
    }
    printf("----- Lights\n");
    for(int i = 0; i < MAX_ELEMENTS; i++){
        if(g_context.lights[i].intensity > 0) printLight(&g_context.lights[i]);
    }
}