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
typedef struct renderContext_s renderContext_t;
//-----------------------------------------------------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------------------------------------------------
struct renderContext_s{
    point_t origin;
    int viewportWidth;
    int viewportHeight;
    int viewportDistance;
    float renderDistance;
    float angleRotation[3];
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

//-----------------------------------------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------------------------------------
int RR_addMesh(mesh_t* mesh_ptr, renderContext_t* context_ptr);

/**
 * @brief Add a light to the context.
 * 
 * @param light The light to add.
 * @return int 
 */
int RR_addLight(lightSource_t* light, renderContext_t* context_ptr);

/**
 * @brief Add an object to the context.
 * 
 * @param object_ptr The object to add.
 * @return int 
 */
int RR_addObject(object_t* object_ptr, renderContext_t* context_ptr);

/**
 * @brief Set up the context variables.
 * 
 * @param origin Origin point of the camera.
 * @param vW Viewport width.
 * @param vH Viewport height.
 * @param vD Viewport distance from the camera.
 * @return int
 */
int RR_initScene(point_t* origin_ptr, int viewportWidth, int viewportHeight, int viewportDistance, int renderDistance, renderContext_t* context_ptr);

int RR_clearScene();

int RR_renderObjects(renderContext_t* context_ptr, int* indexBuffer_ptr);

int RR_renderGrids(renderContext_t* context_ptr);

#endif /* RASTERIZATION_H */