//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#include "raytracing.h"
#include "print.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
sceneContext_t g_context;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
/* Vector Functions */
// Returns p2 - p1
vector_t* vectorize(point_t* p1, point_t* p2){
    vector_t *ret = calloc(1, sizeof(vector_t));
    ret->x = p2->x - p1->x;
    ret->y = p2->y - p1->y;
    ret->z = p2->z - p1->z;
    return ret;
}

float dot(vector_t* v1, vector_t* v2){
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

/* Canvas Functions*/
point_t* canvasToViewport(int x, int y){
    point_t* ret = calloc(1, sizeof(point_t));
    ret->x = x * (float)g_context.viewportWidth / (float)windowWidth;
    ret->y = y * (float)g_context.viewportHeight / (float)windowHeight;
    ret->z = g_context.viewportDistance;
    return ret;
}

double intersectLineSphere(point_t* origin_ptr, point_t* viewportPixel_ptr, sphere_t* sphere_ptr, int tmin){
    vector_t *vector = vectorize(&sphere_ptr->center, origin_ptr);
    float a = dot(viewportPixel_ptr, viewportPixel_ptr);
    float b = 2 * dot(vector, viewportPixel_ptr);
    float c = dot(vector, vector) - sphere_ptr->radius * sphere_ptr->radius;
    free(vector);
    float delta = b * b - 4 * a * c;
    if(delta < 0){
        return tmin - 1;
    }
    double t1 = (- (float)b - sqrt(delta)) / (2.f * (float)a);
    double t2 = (-(float)b + sqrt(delta)) / (2.f * (float)a);
    if(t1 > tmin && t1 < t2){
        return t1;
    }
    return t2;
}

rgba_t* getPixelColor(point_t* origin_ptr, point_t* viewportPixel_ptr, double tmin, double tmax){
    double closestValue = tmax;
    rgba_t* ret = NULL;
    for(int i = 0; i < MAX_SPHERES; i++){
        if(g_context.spheres[i].radius <= 0){
            continue;
        }
        double t = intersectLineSphere(origin_ptr, viewportPixel_ptr, &g_context.spheres[i], tmin);
        if(t < tmin){
            continue;
        }
        if(t < closestValue){
            closestValue = t;
            ret = &g_context.spheres[i].color;
        }
    }
    return ret;
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int RT_initScene(point_t* origin, int vW, int vH, int vD){
    g_context.origin = *origin;
    g_context.viewportWidth = vW;
    g_context.viewportHeight = vH;
    g_context.viewportDistance = vD;
}

int RT_addSphere(sphere_t* sphere){
    if(g_context.numSpheres == MAX_SPHERES) return EXIT_FAILURE;
    if(sphere->radius <= 0) return EXIT_FAILURE;
    for(int i = 0; i < MAX_SPHERES; i++){
        if(g_context.spheres[i].radius <= 0){
            g_context.spheres[i] = *sphere;
            break;
        }
    }
    g_context.numSpheres += 1;
}

int RT_drawScene(){
    for(int i = -windowWidth / 2; i < windowWidth / 2; i++){
        for(int j = -windowWidth / 2; j < windowWidth / 2; j++){
            point_t* viewportPixel_ptr = canvasToViewport(i, j);
            rgba_t* color_ptr = getPixelColor(&g_context.origin, viewportPixel_ptr, 1, 50);
            free(viewportPixel_ptr);
            if(color_ptr == NULL){
                continue;
            }
            // printf("Color : %i %i %i\n", color->red, color->green, color->blue);
            DRAW_pixel(i, j, color_ptr);
        }
    }
}