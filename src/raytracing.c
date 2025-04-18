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
    // compute the vector from Camera to Viewport Point.
    ret_ptr->x = x * (float)g_context.viewportWidth / (float)g_windowWidth;
    ret_ptr->y = y * (float)g_context.viewportHeight / (float)g_windowHeight;
    ret_ptr->z = g_context.viewportDistance;
    return EXIT_SUCCESS;
}

int computeLight(point_t* pointOnObject_ptr, vector_t* normal_ptr, vector_t* leavingLightVector_ptr, int specular, float* intensity){
    // leavingLightVector is the ray of light leaving the object and going to the camera. Named V.
    // commingLightVector is the ray of light leaving the object and going to the source. Named L.
    // normal vector is a unitary vector.
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
            if(COO_copyCoordinates(&g_context.lights[i].carac, &commingLightVector) == EXIT_FAILURE){ //! I think its in the wrond direction
                printf("*-* Error : Problem occured while getting carac of light %i.\n", i);
                return EXIT_FAILURE;
            }
        }else if(g_context.lights[i].type == LT_point){
            if(COO_vectorizePoints(pointOnObject_ptr, &g_context.lights[i].carac, &commingLightVector) == EXIT_FAILURE){
                printf("*-* Error : Problem occured while getting carac of light %i.\n", i);
                return EXIT_FAILURE;
            }
        }else{                
            printf("*-* Warning : Unknwon type of light %i : type %i.\n", i, g_context.lights[i].type);
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
        if(closestObject_ptr != NULL){ // other object between source and current object ==> no light from this source
            continue;
        }
        // precalculations
        float angleNormalLight = COO_scalarProduct(normal_ptr, &commingLightVector);
        // coeff applied to the intensity of the current light. Might be over 1.
        float coeff = 0;
        //------------- Diffuse reflection
        if(angleNormalLight >= 0){
            coeff += angleNormalLight / sqrt(COO_scalarProduct(&commingLightVector, &commingLightVector));
        }
        //------------- Specular reflection
        // Check if object is matte or shiny
        if(specular <= 0){
            *intensity += g_context.lights[i].intensity * coeff;
            continue;
        }
        vector_t reflectionVector = {};
        // compute R = 2 (N.L) N - L
        if(COO_linearTransformation(normal_ptr, 2 * angleNormalLight, &commingLightVector, -1, &reflectionVector) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        float angleReflectionLight = COO_scalarProduct(&reflectionVector, leavingLightVector_ptr);
        if(angleReflectionLight >= 0){
            angleReflectionLight /= sqrt(COO_scalarProduct(&reflectionVector, &reflectionVector));
            angleReflectionLight /= sqrt(COO_scalarProduct(leavingLightVector_ptr, leavingLightVector_ptr));
            coeff += pow(angleReflectionLight, specular);
        }
        // add to intensity
        *intensity += g_context.lights[i].intensity * coeff;
    }
    return EXIT_SUCCESS;
}

int getPixelColor(point_t* origin_ptr, vector_t* rayDirectionVector_ptr, double tmin, double tmax, int recursiveDepth, rgba_t* ret_ptr){
    // rayDirection is the vector from the camera to the viewport. Named D. It is used to calculate intersections.
    // rayVector is the vector from the viewport / the point on an object to the camera. Named V. It is used to calculate lights.
    float closestValue = tmax + 1;
    float currentValue = tmax + 1;
    object_t* closestObject_ptr = NULL;
    vector_t rayVector = {};
    if(COO_linearTransformation(rayDirectionVector_ptr, -1, NULL, 0, &rayVector) == EXIT_FAILURE){
        printf("*-* Error while trying to transform ray vector.\n");
        return DRAW_initBackgroundColor(ret_ptr);
    }
    // get closest object
    for(int i = 0; i < MAX_OBJECTS; i++){
        currentValue = OBJ_intersectObject(origin_ptr, rayDirectionVector_ptr, &g_context.objects[i], tmin, tmax);
        if(currentValue < closestValue && currentValue > tmin && currentValue < tmax){
            closestObject_ptr = &g_context.objects[i];
            closestValue = currentValue;
        }
    }
    if(closestObject_ptr == NULL){ // no object found so return background color
        return DRAW_initBackgroundColor(ret_ptr);
    }
    // point on the object that intersected the ray <=> point on the ray that intersected the object. Named P.
    point_t pointOnObject = {};
    if(COO_linearTransformation(origin_ptr, 1, rayDirectionVector_ptr, closestValue, &pointOnObject) == EXIT_FAILURE){
        printf("*-* Error while trying to transform point on object.\n");
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
    // vector coming from P and going on the point of the viewport. Mainly D. Named V.
    vector_t lightVector = {};
    if(COO_copyCoordinates(&rayVector, &lightVector) == EXIT_FAILURE){
        printf("*-* Error while trying to copy coordinate for lightVector.\n");
        return DRAW_initBackgroundColor(ret_ptr);
    }
    // compute intensity
    float intensity = 0;
    if(computeLight(&pointOnObject, &normalVector, &lightVector, closestObject_ptr->specular, &intensity) == EXIT_FAILURE){
        printf("*-* Error during light computation.\n");
        return DRAW_initBackgroundColor(ret_ptr);
    }
    // add intensity
    DRAW_addIntensity(&closestObject_ptr->color, intensity, ret_ptr);
    // add recursive intensity
    if(recursiveDepth > 0 && closestObject_ptr->reflective != 0){
        vector_t reflectionVector = {};
        if(COO_linearTransformation(&normalVector, 2 * COO_scalarProduct(&normalVector, &rayVector), &rayVector, -1, &reflectionVector) == EXIT_FAILURE){
            printf("*-* Error while transforming reflection vector.\n");
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
            g_context.objects[i] = *object_ptr; // TODO : remove copy of the object instead of using the pointer
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
    vector_t rayDirectionVector = {};
    rgba_t pixelColor = {};
    for(int x = -g_windowWidth / 2; x < g_windowWidth / 2; x++){
        for(int y = -g_windowHeight / 2; y < g_windowHeight / 2; y++){
            // Vector that goes from one pixel on the canvas to the camera. Named D.
            if(canvasToViewport(x, y, &rayDirectionVector) == EXIT_FAILURE){
                continue;
            }
            COO_rotationVectorProduct(&rayDirectionVector,0,0,0);
            if(getPixelColor(&g_context.origin, &rayDirectionVector, 0.00001, TMAX_ALL, 3, &pixelColor) == EXIT_FAILURE){
                continue;
            }
            DRAW_pixel(x, y, &pixelColor);
        }
    }
    return 0;
}