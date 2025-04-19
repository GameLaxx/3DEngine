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
#define MAX_MESHES 5
#define MAX_MESH_IDS 50
#define MAX_OBJECTS 10
//-----------------------------------------------------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------------------------------------------------
typedef struct sceneContext_s sceneContext_t;
typedef struct triangle_s triangle_t;
//-----------------------------------------------------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------------------------------------------------
struct sceneContext_s{
    point_t origin;
    int viewportWidth;
    int viewportHeight;
    int viewportDistance;
    int meshesCount;
    int objectsCount;
    int meshesId[MAX_MESH_IDS];
    mesh_t meshes[MAX_MESHES];
    object_t objects[MAX_OBJECTS];
    float* zBuffer;
};

struct triangle_s{
    point_t p1;
    point_t p2;
    point_t p3;
    rgba_t color;
};
//-----------------------------------------------------------------------------------------------------------------------
// Enums
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
extern sceneContext_t g_context;
//-----------------------------------------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------------------------------------

int RR_addMesh(mesh_t* mesh_ptr);

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