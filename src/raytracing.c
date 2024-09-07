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
point_t* canvasToViewport(int x, int y){
    point_t* ret = calloc(1, sizeof(point_t));
    ret->x = x * (float)g_context.viewportWidth / (float)windowWidth;
    ret->y = y * (float)g_context.viewportHeight / (float)windowHeight;
    ret->z = g_context.viewportDistance;
    return ret;
}

sphere_t* intersectLineSphere(point_t* origin_ptr, point_t* lightVector_ptr, int tmin, float* closestValue){
    sphere_t* closestSphere_ptr = NULL;
    for(int i = 0; i < MAX_ELEMENTS; i++){
        // not a valid sphere
        if(g_context.spheres[i].radius <= 0){
            continue;
        }
        // value that determine where is the intersection between the ray and a sphere
        
        vector_t *vector = COO_vectorizePoints(&g_context.spheres[i].center, origin_ptr);
        float a = COO_scalarProduct(lightVector_ptr, lightVector_ptr);
        float b = 2 * COO_scalarProduct(vector, lightVector_ptr);
        float c = COO_scalarProduct(vector, vector) - g_context.spheres[i].radius * g_context.spheres[i].radius;
        free(vector);
        float delta = b * b - 4 * a * c;
        if(delta < 0){
            continue;
        }
        double t1 = (- (float)b - sqrt(delta)) / (2.f * (float)a);
        double t2 = (-(float)b + sqrt(delta)) / (2.f * (float)a);
        // unvalid t values for the current sphere
        if(t1 < tmin && t2 < tmin){
            continue;
        }
        // not close enough t values
        if(*closestValue < t1 && *closestValue < t2){
            continue;
        }
        // t1 is the good value
        if(tmin < t1 && t1 < *closestValue && t1 < t2){
            *closestValue = t1;
            closestSphere_ptr = &g_context.spheres[i];
            continue;
        }
        // t2 is the good value
        if(tmin < t2 && t2 < *closestValue && t2 < t1){
            *closestValue = t2;
            closestSphere_ptr = &g_context.spheres[i];
            continue;
        }
    }
    return closestSphere_ptr;
}

int computeLight(point_t* pointOnSphere_ptr, vector_t* normal_ptr, vector_t* leavingLightVector_ptr, int specular, float* intensity){
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
            commingLightVector_ptr = COO_vectorizePoints(pointOnSphere_ptr, &g_context.lights[i].carac);
        }else{
            continue;
        }
        // get for point if shadow or not
        float tmin = 0.00001;
        float tmax = (g_context.lights[i].type == LT_directional) ? 1000000 : 1;
        float closestValue = tmax + 1;
        sphere_t* closestSphere_ptr = intersectLineSphere(pointOnSphere_ptr, commingLightVector_ptr, tmin, &closestValue);
        if(closestSphere_ptr != NULL && closestValue < tmax && closestValue > tmin){
            continue;
        }
        // Transform L into a unitary vector.
        COO_applyFactor(commingLightVector_ptr, sqrt(COO_scalarProduct(commingLightVector_ptr, commingLightVector_ptr)), FT_DIV);
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
            reflection /= sqrt(COO_scalarProduct(reflectionVector_ptr, reflectionVector_ptr) * COO_scalarProduct(leavingLightVector_ptr, leavingLightVector_ptr));
            coeff += pow(reflection, specular);
        }
        // add to intensity
        *intensity += g_context.lights[i].intensity * coeff;
        // free unneeded vectors
        free(commingLightVector_ptr);
        free(reflectionVector_ptr);
    }
}

rgba_t* getPixelColor(int x, int y, point_t* origin_ptr, double tmin, double tmax){
    float closestValue = tmax + 1;
    rgba_t* ret = NULL;
    // Vector that goes from one pixel on the canvas to one point of the view port. Named D.
    vector_t* rayVector_ptr = canvasToViewport(x, y);
    // get closest sphere
    sphere_t* closestSphere_ptr = intersectLineSphere(origin_ptr, rayVector_ptr, tmin, &closestValue);
    if(closestSphere_ptr == NULL || closestValue >= tmax){
        free(rayVector_ptr);
        return ret;
    }
    // point on the sphere that intersected the ray <=> point on the ray that intersected the sphere. Named P.
    point_t* pointOnSphere_ptr = COO_linearTransformation(origin_ptr, 1, rayVector_ptr, closestValue);
    // normal vector for the point P. Named N. Be aware that N is non unitary.
    vector_t* normal_ptr = COO_vectorizePoints(pointOnSphere_ptr, &closestSphere_ptr->center);
    // Transform N into a unitary vector.
    COO_applyFactor(normal_ptr, sqrt(COO_scalarProduct(normal_ptr, normal_ptr)), FT_DIV);
    // vector coming from P and going on the point of the viewport. Mainly -D. 
    vector_t* lightVector_ptr = COO_linearTransformation(rayVector_ptr, -1, NULL, 0);
    float intensity = 0;
    computeLight(pointOnSphere_ptr, normal_ptr, lightVector_ptr, closestSphere_ptr->specular, &intensity);
    ret = DRAW_addIntensity(&closestSphere_ptr->color, intensity);
    free(pointOnSphere_ptr);
    free(normal_ptr);
    free(lightVector_ptr);
    free(rayVector_ptr);
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
    for(int x = -windowWidth / 2; x < windowWidth / 2; x++){
        for(int y = -windowWidth / 2; y < windowWidth / 2; y++){
            rgba_t* color_ptr = getPixelColor(x, y, &g_context.origin, 1, 50);
            if(color_ptr == NULL){
                continue;
            }
            DRAW_pixel(x, y, color_ptr);
            free(color_ptr);
        }
    }
}