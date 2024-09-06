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
coordinate_t* coordinateCopy(coordinate_t* c1){
    coordinate_t *ret = calloc(1, sizeof(coordinate_t));
    ret->x = c1->x;
    ret->y = c1->y;
    ret->z = c1->z;
    return ret;
}

rgba_t* rgbaCopy(rgba_t* color_ptr){
    rgba_t* ret = calloc(1, sizeof(rgba_t));
    ret->red = color_ptr->red;
    ret->green = color_ptr->green;
    ret->blue = color_ptr->blue;
    ret->alpha = color_ptr->alpha;
    return ret;
}

vector_t* vectorize(point_t* p1, point_t* p2){
    vector_t *ret = calloc(1, sizeof(vector_t));
    ret->x = p2->x - p1->x;
    ret->y = p2->y - p1->y;
    ret->z = p2->z - p1->z;
    return ret;
}

point_t* extendPoint(point_t* p, float coeff, vector_t* vector){
    point_t *ret = calloc(1, sizeof(point_t));
    ret->x = p->x + vector->x * coeff;
    ret->y = p->y + vector->y * coeff;
    ret->z = p->z + vector->z * coeff;
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

void computeLight(point_t* spherePoint_ptr, vector_t* normal_ptr, float* intensity){
    for(int i = 0; i < MAX_ELEMENTS; i++){
        if(g_context.lights[i].intensity <= 0) continue;
        if(g_context.lights[i].type == LT_ambiant){
            *intensity += g_context.lights[i].intensity;
            continue;
        }
        vector_t* lightVector_ptr = coordinateCopy(&g_context.lights[i].carac);
        if(g_context.lights[i].type == LT_point){
            free(lightVector_ptr);
            lightVector_ptr = vectorize(&g_context.lights[i].carac, spherePoint_ptr);
        }
        float direction = dot(normal_ptr, lightVector_ptr);
        if(direction < 0) continue;
        *intensity += g_context.lights[i].intensity * direction / sqrt(dot(normal_ptr, normal_ptr) * dot(lightVector_ptr, lightVector_ptr));
        free(lightVector_ptr);
    }
}

rgba_t* addLightToColor(rgba_t* color_ptr, float intensity){
    rgba_t* ret = calloc(1, sizeof(rgba_t));
    float red = (float) color_ptr->red * intensity;
    ret->red = ((int) red > 255) ? 255 : (int) red;
    float green = (float) color_ptr->green * intensity;
    ret->green = ((int) green > 255) ? 255 : (int) green;
    float blue = (float) color_ptr->blue * intensity;
    ret->blue = ((int) blue > 255) ? 255 : (int) blue;
    return ret;
}

rgba_t* getPixelColor(point_t* origin_ptr, point_t* viewportPixel_ptr, double tmin, double tmax){
    double closestValue = tmax;
    rgba_t* ret = NULL;
    for(int i = 0; i < MAX_ELEMENTS; i++){
        if(g_context.spheres[i].radius <= 0){
            continue;
        }
        double t = intersectLineSphere(origin_ptr, viewportPixel_ptr, &g_context.spheres[i], tmin);
        if(t < tmin){
            continue;
        }
        if(t > closestValue){
            continue;
        }
        closestValue = t;
        point_t* spherePoint_ptr = extendPoint(origin_ptr, closestValue, viewportPixel_ptr);
        vector_t* normal_ptr = vectorize(spherePoint_ptr, &g_context.spheres[i].center);
        float intensity = 0;
        computeLight(spherePoint_ptr, normal_ptr, &intensity);
        if(intensity > 0.3) printf("I : %f\n", intensity);
        if(ret != NULL){
            free(ret);
        }
        ret = addLightToColor(&g_context.spheres[i].color, intensity);
        free(spherePoint_ptr);
        free(normal_ptr);
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
    if(g_context.numSpheres == MAX_ELEMENTS) return EXIT_FAILURE;
    if(sphere->radius <= 0) return EXIT_FAILURE;
    for(int i = 0; i < MAX_ELEMENTS; i++){
        if(g_context.spheres[i].radius <= 0){
            g_context.spheres[i] = *sphere;
            break;
        }
    }
    g_context.numSpheres += 1;
}

int RT_addLight(lightSource_t* light){
    if(g_context.numLights == MAX_ELEMENTS) return EXIT_FAILURE;
    if(light->intensity <= 0) return EXIT_FAILURE;
    for(int i = 0; i < MAX_ELEMENTS; i++){
        if(g_context.lights[i].intensity <= 0){
            g_context.lights[i] = *light;
            break;
        }
        if(g_context.lights[i].type == LT_ambiant && light->type == LT_ambiant){
            g_context.lights[i].intensity += light->intensity;
            if(g_context.lights[i].intensity > 1){
                g_context.lights[i].intensity = 1;
            }
            g_context.numLights -= 1; //compensate the fact that we did not add a light
            break;
        }
    }
    g_context.numLights += 1;
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
            DRAW_pixel(i, j, color_ptr);
            free(color_ptr);
        }
    }
}