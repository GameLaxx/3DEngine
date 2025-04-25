/* software.h
 * date : 23/04/2025 (dd/mm/yy)
 * author : tboisse
*/
#ifndef SOFTWARE_H
#define SOFTWARE_H
//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "coordinates.h"
#include "draw.h"
#include "Rasterization/objects.h"
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
typedef struct sceneContext_s sceneContext_t;
//-----------------------------------------------------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------------------------------------------------
struct sceneContext_s{
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
    lightSource_t lights[2];
    float* zBuffer;
};
//-----------------------------------------------------------------------------------------------------------------------
// Enums
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
extern sceneContext_t g_sceneContext;
//-----------------------------------------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------------------------------------

/**
 * @brief Set up the context variables.
 * 
 * @param origin Origin point of the camera.
 * @param vW Viewport width.
 * @param vH Viewport height.
 * @param vD Viewport distance from the camera.
 * @return int
 */
int SW_initScene(point_t* origin, int vW, int vH, int vD);

int SW_clearScene();

int SW_drawScene();

#endif /* SOFTWARE_H */