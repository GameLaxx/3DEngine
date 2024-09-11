//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#include "raytracing.h"
#include "objects.h"
#include "print.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
sceneContext_t g_context;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
point_t* canvasToViewport(int x, int y){
    point_t* ret = calloc(1, sizeof(point_t));
    ret->x = x * (float)g_context.viewportWidth / (float)windowWidth;
    ret->y = y * (float)g_context.viewportHeight / (float)windowHeight;
    ret->z = g_context.viewportDistance;
    return ret;
}

int computeLight(point_t* pointOnObject_ptr, vector_t* normal_ptr, vector_t* leavingLightVector_ptr, int specular, float* intensity){
    // leavingLightVector is the ray of light leaving the sphere and going to the camera. Named V.
    // commingLightVector is the ray of light comming from the source and going on the sphere. Named L.
    for(int i = 0; i < MAX_ELEMENTS; i++){
        if(g_context.lights[i].intensity <= 0) continue;
        // ambiant light just add intensity
        if(g_context.lights[i].type == LT_ambiant){
            *intensity += g_context.lights[i].intensity;
            continue;
        }
        // get direction of light
        vector_t* commingLightVector_ptr; 
        if(g_context.lights[i].type == LT_directional){
            commingLightVector_ptr = COO_copyCoordinates(&g_context.lights[i].carac);
        }else if(g_context.lights[i].type == LT_point){
            commingLightVector_ptr = COO_vectorizePoints(pointOnObject_ptr, &g_context.lights[i].carac);
        }else{
            continue;
        }
        // get for point if shadow or not
        float tmin = 0.000001;
        float tmax = (g_context.lights[i].type == LT_directional) ? TMAX_ALL : TMAX_POINT;
        object_t* closestObject_ptr = NULL;
        float closestValue = tmax + 1;
        float currentValue = tmax + 1;
        
        for(int i = 0; i < MAX_ELEMENTS; i++){
            currentValue = OBJ_intersectObject(pointOnObject_ptr, commingLightVector_ptr, &g_context.objects[i], tmin, tmax);
            if(currentValue < closestValue && currentValue > tmin && currentValue < tmax){
                closestObject_ptr = &g_context.objects[i];
                closestValue = currentValue;
            }
        }
        if(closestObject_ptr != NULL){
            continue;
        }
        // Transform L into a unitary vector.
        COO_lambdaProduct(commingLightVector_ptr, sqrt(COO_scalarProduct(commingLightVector_ptr, commingLightVector_ptr)), FT_DIV);
        // coeff applied to the intensity of the current light. Might be over 1.
        float coeff = 0;
        // Diffuse reflection
        float direction = -COO_scalarProduct(normal_ptr, commingLightVector_ptr); // TODO : I had to use - but I'm not sure why
        if(direction >= 0){
            coeff += direction;
        }
        // Check if object if matte or shiny
        if(specular <= 0){
            continue;
        }
        // Specular reflection
        vector_t* reflectionVector_ptr = COO_linearTransformation(normal_ptr, 2 * COO_scalarProduct(normal_ptr, commingLightVector_ptr), commingLightVector_ptr, -1);
        float reflection = COO_scalarProduct(reflectionVector_ptr, leavingLightVector_ptr);
        if(reflection >= 0){
            reflection /= sqrt(COO_scalarProduct(reflectionVector_ptr, reflectionVector_ptr));
            coeff += pow(reflection, specular);
        }
        // add to intensity
        *intensity += g_context.lights[i].intensity * coeff;
        // free unneeded vectors
        free(commingLightVector_ptr);
        free(reflectionVector_ptr);
    }
    return 0;
}

rgba_t* getPixelColor(point_t* origin_ptr, vector_t* rayVector_ptr, double tmin, double tmax, int recursiveDepth){
    float closestValue = tmax + 1;
    float currentValue = tmax + 1;
    rgba_t* localRet = DRAW_initBackgroundColor();
    object_t* closestObject_ptr = NULL;
    // get closest object
    for(int i = 0; i < MAX_ELEMENTS; i++){
        currentValue = OBJ_intersectObject(origin_ptr, rayVector_ptr, &g_context.objects[i], tmin, tmax);
        if(currentValue < closestValue && currentValue > tmin && currentValue < tmax){
            closestObject_ptr = &g_context.objects[i];
            closestValue = currentValue;
        }
    }
    if(closestObject_ptr == NULL){
        return localRet;
    }
    free(localRet);
    // point on the object that intersected the ray <=> point on the ray that intersected the object. Named P.
    point_t* pointOnObject_ptr = COO_linearTransformation(origin_ptr, 1, rayVector_ptr, closestValue);
    // normal vector for the point P. Named N.
    vector_t* normal_ptr = OBJ_normalObject(closestObject_ptr, pointOnObject_ptr);
    // Transform N into a unitary vector.
    COO_lambdaProduct(normal_ptr, sqrt(COO_scalarProduct(normal_ptr, normal_ptr)), FT_DIV);
    // vector coming from P and going on the point of the viewport. Mainly -D. Named V.
    vector_t* lightVector_ptr = COO_linearTransformation(rayVector_ptr, -1, NULL, 0);
    // Transform V into a unitary vector.
    COO_lambdaProduct(lightVector_ptr, sqrt(COO_scalarProduct(lightVector_ptr, lightVector_ptr)), FT_DIV);
    float intensity = 0;
    computeLight(pointOnObject_ptr, normal_ptr, lightVector_ptr, closestObject_ptr->specular, &intensity);
    localRet = DRAW_addIntensity(&closestObject_ptr->color, intensity);
    if(recursiveDepth > 0 && closestObject_ptr->reflective != 0){
        vector_t* reflectionVector_ptr = COO_linearTransformation(normal_ptr, -2 * COO_scalarProduct(normal_ptr, rayVector_ptr), rayVector_ptr, 1);
        rgba_t* recursiveRet_ptr = getPixelColor(pointOnObject_ptr, reflectionVector_ptr, tmin, tmax, recursiveDepth - 1);
        DRAW_computeReflection(localRet, recursiveRet_ptr, closestObject_ptr->reflective);
        free(reflectionVector_ptr);
        free(recursiveRet_ptr);
    }
    free(pointOnObject_ptr);
    free(normal_ptr);
    free(lightVector_ptr);
    return localRet;
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int RT_initScene(point_t* origin, int vW, int vH, int vD){
    g_context.origin = *origin;
    g_context.viewportWidth = vW;
    g_context.viewportHeight = vH;
    g_context.viewportDistance = vD;
    return 0;
}

int RT_addObject(object_t* object_ptr){
    if(g_context.numObjects == MAX_ELEMENTS) return EXIT_FAILURE;
    if(object_ptr->type <= OT_NAO) return EXIT_FAILURE;
    if(OBJ_checkObject(object_ptr) == 0) return EXIT_FAILURE;
    OBJ_initObject(object_ptr);
    for(int i = 0; i < MAX_ELEMENTS; i++){
        if(g_context.objects[i].type <= OT_NAO){
            g_context.objects[i] = *object_ptr;
            break;
        }
    }
    g_context.numObjects += 1;
    return 0;
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
    return 0;
}

int RT_drawScene(){
    for(int x = -windowWidth / 2; x < windowWidth / 2; x++){
        for(int y = -windowWidth / 2; y < windowWidth / 2; y++){
            // Vector that goes from one pixel on the canvas to one point of the view port. Named D.
            vector_t* rayVector_ptr = canvasToViewport(x, y);
            COO_rotationVectorProduct(rayVector_ptr,0,0,0);
            rgba_t* color_ptr = getPixelColor(&g_context.origin, rayVector_ptr, 0.00001, TMAX_ALL, 3);
            free(rayVector_ptr);
            if(color_ptr == NULL){
                continue;
            }
            DRAW_pixel(x, y, color_ptr);
            free(color_ptr);
        }
    }
    return 0;
}