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

void printRGBA(rgba_t* color_ptr){
    printf("Color : %i %i %i %i\n", color_ptr->red, color_ptr->green, color_ptr->blue, color_ptr->alpha);
}

void printSphere(sphere_t* sphere_ptr){
    printf("Sphere : %f\n", sphere_ptr->radius);
    printCoo(&sphere_ptr->center);
}

void printObject(object_t* object_ptr){
    if(object_ptr->type == OT_sphere){
        printSphere(object_ptr->content_ptr);
    }
}

void printLight(lightSource_t* light_ptr){
    printf("Light : %i %f\n", light_ptr->type, light_ptr->intensity);
    if(light_ptr->type != LT_ambiant) printCoo(&light_ptr->carac);
}

void printContext(){
    sphere_t* sphere_ptr = NULL;
    printf("Context : %i %i %i, nObjects %i, nLights %i\n",
        g_context.viewportWidth, g_context.viewportHeight, g_context.viewportDistance,
        g_context.numObjects, g_context.numLights);
    printCoo(&g_context.origin);
    printf("----- Objects\n");
    for(int i = 0; i < MAX_ELEMENTS; i++){
        switch (g_context.objects[i].type){
        case OT_sphere:
            sphere_ptr = (sphere_t*) g_context.objects[i].content_ptr;
            printSphere(sphere_ptr);
            break;
        default:
            printf("Unknown type : %i\n", g_context.objects[i].type);
            break;
        }
    }
    printf("----- Lights\n");
    for(int i = 0; i < MAX_ELEMENTS; i++){
        if(g_context.lights[i].intensity > 0) printLight(&g_context.lights[i]);
    }
}