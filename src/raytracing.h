/* raytracing.h
 * date : 05/09/2024 (dd/mm/yy)
 * author : tboisse
*/
#ifndef RAYTRACING_H
#define RAYTRACING_H
//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "draw.h"
#include "coordinates.h"
//-----------------------------------------------------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------------------------------------------------
#define MAX_ELEMENTS 5
//-----------------------------------------------------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------------------------------------------------
typedef struct sphere_s sphere_t;
typedef struct lightSource_s lightSource_t;
typedef struct sceneContext_s sceneContext_t;
typedef enum lightType_e lightType;
//-----------------------------------------------------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------------------------------------------------
struct sphere_s{
    point_t center;
    float radius;
    rgba_t color;
    int specular;
    float reflective;
};

struct lightSource_s{
    int type;
    float intensity;
    point_t carac; //<< could be position or direction
};

struct sceneContext_s{
    point_t origin;
    int viewportWidth;
    int viewportHeight;
    int viewportDistance;
    sphere_t spheres[MAX_ELEMENTS];
    lightSource_t lights[MAX_ELEMENTS];
    int numSpheres; 
    int numLights; 
};
//-----------------------------------------------------------------------------------------------------------------------
// Enums
//-----------------------------------------------------------------------------------------------------------------------
enum lightType_e{
    LT_ambiant = 0,
    LT_point,
    LT_directional
};
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
extern sceneContext_t g_context;
//-----------------------------------------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------------------------------------
int RT_initScene(point_t* origin, int vW, int vH, int vD);
int RT_addSphere(sphere_t* sphere);
int RT_addLight(lightSource_t* light);
int RT_drawScene();

#endif /* RAYTRACING_H */