/* rasterization.h
 * date : 18/04/2025 (dd/mm/yy)
 * author : tboisse
*/
#ifndef RASTERIZATION_H
#define RASTERIZATION_H
//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "coordinates.h"
#include "draw.h"
#include "objects.h"
//-----------------------------------------------------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------------------------------------------------
#define MAX_MESHES 10
#define MAX_MESH_IDS 50
#define MAX_OBJECTS 10
#define MAX_LIGHTS 5
//-----------------------------------------------------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------------------------------------------------
typedef struct rasterizationContext_s rasterizationContext_t;
//-----------------------------------------------------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------------------------------------------------
struct rasterizationContext_s{
    point_t origin;
    int viewportWidth;
    int viewportHeight;
    int viewportDistance;
    int meshesCount;
    int objectsCount;
    int lightsCount;
    int meshesId[MAX_MESH_IDS];
    mesh_t meshes[MAX_MESHES];
    object_t objects[MAX_OBJECTS];
    lightSource_t lights[MAX_LIGHTS];
    float* zBuffer;
};
//-----------------------------------------------------------------------------------------------------------------------
// Enums
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
extern rasterizationContext_t g_rastContext;
//-----------------------------------------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------------------------------------

int RR_addMesh(mesh_t* mesh_ptr);

/**
 * @brief Add a light to the context.
 * 
 * @param light The light to add.
 * @return int 
 */
int RR_addLight(lightSource_t* light);

/**
 * @brief Add an object to the context.
 * 
 * @param object_ptr The object to add.
 * @return int 
 */
int RR_addObject(object_t* object_ptr);

/**
 * @brief Set up the context variables.
 * 
 * @param origin Origin point of the camera.
 * @param vW Viewport width.
 * @param vH Viewport height.
 * @param vD Viewport distance from the camera.
 * @return int
 */
int RR_initScene(point_t* origin, int vW, int vH, int vD);

int RR_clearScene();

int RR_drawScene();

#endif /* RASTERIZATION_H */