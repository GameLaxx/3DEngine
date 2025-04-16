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
int canvasToViewport(int x, int y, point_t* ret_ptr){
    ret_ptr->x = x * (float)g_context.viewportWidth / (float)windowWidth;
    ret_ptr->y = y * (float)g_context.viewportHeight / (float)windowHeight;
    ret_ptr->z = g_context.viewportDistance;
    return EXIT_SUCCESS;
}

int computeLight(point_t* pointOnObject_ptr, vector_t* normal_ptr, vector_t* leavingLightVector_ptr, int specular, float* intensity){
    // leavingLightVector is the ray of light leaving the object and going to the camera. Named V.
    // commingLightVector is the ray of light comming from the source and going on the object. Named L.
    for(int i = 0; i < MAX_LIGHTS; i++){
        if(g_context.lights[i].intensity <= 0) continue;
        // ambiant light just add intensity
        if(g_context.lights[i].type == LT_ambiant){
            *intensity += g_context.lights[i].intensity;
            continue;
        }
        // get direction of light
        vector_t commingLightVector = {}; 
        if(g_context.lights[i].type == LT_directional){
            if(COO_copyCoordinates(&g_context.lights[i].carac, &commingLightVector) == EXIT_FAILURE){
                return EXIT_FAILURE;
            }
        }else if(g_context.lights[i].type == LT_point){
            if(COO_vectorizePoints(pointOnObject_ptr, &g_context.lights[i].carac, &commingLightVector) == EXIT_FAILURE){
                return EXIT_FAILURE;
            }
        }else{
            continue;
        }
        // get for point if shadow or not
        float tmin = 0.000001;
        float tmax = (g_context.lights[i].type == LT_directional) ? TMAX_ALL : TMAX_POINT;
        object_t* closestObject_ptr = NULL;
        float closestValue = tmax + 1;
        float currentValue = tmax + 1;
        
        for(int i = 0; i < MAX_OBJECTS; i++){
            currentValue = OBJ_intersectObject(pointOnObject_ptr, &commingLightVector, &g_context.objects[i], tmin, tmax);
            if(currentValue < closestValue && currentValue > tmin && currentValue < tmax){
                closestObject_ptr = &g_context.objects[i];
                closestValue = currentValue;
            }
        }
        if(closestObject_ptr != NULL){
            continue;
        }
        // Transform L into a unitary vector.
        COO_lambdaProduct(&commingLightVector, sqrt(COO_scalarProduct(&commingLightVector, &commingLightVector)), FT_DIV);
        // coeff applied to the intensity of the current light. Might be over 1.
        float coeff = 0;
        // Diffuse reflection
        float direction = -COO_scalarProduct(normal_ptr, &commingLightVector); // TODO : I had to use - but I'm not sure why
        if(direction >= 0){
            coeff += direction;
        }
        // Check if object if matte or shiny
        if(specular <= 0){
            continue;
        }
        // Specular reflection
        vector_t reflectionVector = {};
        if(COO_linearTransformation(normal_ptr, 2*COO_scalarProduct(normal_ptr, &commingLightVector), &commingLightVector, -1, &reflectionVector) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        float reflection = COO_scalarProduct(&reflectionVector, leavingLightVector_ptr);
        if(reflection >= 0){
            reflection /= sqrt(COO_scalarProduct(&reflectionVector, &reflectionVector));
            coeff += pow(reflection, specular);
        }
        // add to intensity
        *intensity += g_context.lights[i].intensity * coeff;
    }
    return EXIT_SUCCESS;
}

int getPixelColor(point_t* origin_ptr, vector_t* rayVector_ptr, double tmin, double tmax, int recursiveDepth, rgba_t* ret_ptr){
    float closestValue = tmax + 1;
    float currentValue = tmax + 1;
    object_t* closestObject_ptr = NULL;
    // get closest object
    for(int i = 0; i < MAX_OBJECTS; i++){
        currentValue = OBJ_intersectObject(origin_ptr, rayVector_ptr, &g_context.objects[i], tmin, tmax);
        if(currentValue < closestValue && currentValue > tmin && currentValue < tmax){
            closestObject_ptr = &g_context.objects[i];
            closestValue = currentValue;
        }
    }
    if(closestObject_ptr == NULL){
        return DRAW_initBackgroundColor(ret_ptr);
    }
    // point on the object that intersected the ray <=> point on the ray that intersected the object. Named P.
    point_t pointOnObject = {};
    if(COO_linearTransformation(origin_ptr, 1, rayVector_ptr, closestValue, &pointOnObject) == EXIT_FAILURE){
        return DRAW_initBackgroundColor(ret_ptr);
    }
    // normal vector for the point P. Named N.
    vector_t normalVector = {};
    if(OBJ_normalObject(closestObject_ptr, &pointOnObject, &normalVector) == EXIT_FAILURE){
        printf("*-* ! Be careful : missing normal function for object of type %i\n",  closestObject_ptr->type);
        return DRAW_initBackgroundColor(ret_ptr);
    }
    // Transform N into a unitary vector.
    COO_lambdaProduct(&normalVector, sqrt(COO_scalarProduct(&normalVector, &normalVector)), FT_DIV);
    // vector coming from P and going on the point of the viewport. Mainly -D. Named V.
    vector_t lightVector = {};
    if(COO_linearTransformation(rayVector_ptr, -1, NULL, 0, &lightVector) == EXIT_FAILURE){
        return DRAW_initBackgroundColor(ret_ptr);
    }
    // Transform V into a unitary vector.
    COO_lambdaProduct(&lightVector, sqrt(COO_scalarProduct(&lightVector, &lightVector)), FT_DIV);
    float intensity = 0;
    if(computeLight(&pointOnObject, &normalVector, &lightVector, closestObject_ptr->specular, &intensity) == EXIT_FAILURE){
        return DRAW_initBackgroundColor(ret_ptr);
    }
    DRAW_addIntensity(&closestObject_ptr->color, intensity, ret_ptr);
    if(recursiveDepth > 0 && closestObject_ptr->reflective != 0){
        vector_t reflectionVector = {};
        if(COO_linearTransformation(&normalVector, -2 * COO_scalarProduct(&normalVector, rayVector_ptr), rayVector_ptr, 1, &reflectionVector) == EXIT_FAILURE){
            return DRAW_initBackgroundColor(ret_ptr);
        }
        rgba_t recursiveRet = {};
        getPixelColor(&pointOnObject, &reflectionVector, tmin, tmax, recursiveDepth - 1, &recursiveRet);
        DRAW_computeReflection(ret_ptr, &recursiveRet, closestObject_ptr->reflective); //! problem here
    }
    return EXIT_SUCCESS;
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
    if(g_context.numObjects == MAX_OBJECTS) return EXIT_FAILURE;
    if(object_ptr->type <= OT_NAO) return EXIT_FAILURE;
    if(OBJ_checkObject(object_ptr) == 0) return EXIT_FAILURE;
    OBJ_initObject(object_ptr);
    for(int i = 0; i < MAX_OBJECTS; i++){
        if(g_context.objects[i].type <= OT_NAO){
            g_context.objects[i] = *object_ptr;
            break;
        }
    }
    g_context.numObjects += 1;
    return 0;
}

int RT_addLight(lightSource_t* light){
    if(g_context.numLights == MAX_LIGHTS) return EXIT_FAILURE;
    if(light->intensity <= 0) return EXIT_FAILURE;
    for(int i = 0; i < MAX_LIGHTS; i++){
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
    return EXIT_SUCCESS;
}

int RT_drawScene(){
    vector_t rayVector_ptr = {};
    rgba_t pixelColor = {};
    for(int x = -windowWidth / 2; x < windowWidth / 2; x++){
        for(int y = -windowWidth / 2; y < windowWidth / 2; y++){
            // Vector that goes from one pixel on the canvas to one point of the view port. Named D.
            if(canvasToViewport(x, y, &rayVector_ptr) == EXIT_FAILURE){
                continue;
            }
            COO_rotationVectorProduct(&rayVector_ptr,0,0,0);
            if(getPixelColor(&g_context.origin, &rayVector_ptr, 0.00001, TMAX_ALL, 3, &pixelColor) == EXIT_FAILURE){
                continue;
            }
            DRAW_pixel(x, y, &pixelColor);
        }
    }
    return 0;
}