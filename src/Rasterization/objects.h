/* objects.h
 * date : 18/04/2025 (dd/mm/yy)
 * author : tboisse
*/
#ifndef OBJECTS_H
#define OBJECTS_H
//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "coordinates.h"
//-----------------------------------------------------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------------------------------------------------
typedef struct mesh_s mesh_t;
typedef struct object_s object_t;
typedef struct lightSource_s lightSource_t;
//-----------------------------------------------------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------------------------------------------------
struct lightSource_s{
    int type;
    float intensity;
    point_t carac; //<< could be position or direction
};

struct mesh_s {
    int verticesCount;
    int trianglesCount;
    int normalsCount;
    point_t* vertices_ptr;
    int* indicesVertices_ptr;
    vector_t* normals_ptr;
    int* indicesNormals_ptr; // no need for a count for this one because already stored in vertices or triangles
    int id; // TODO : can store up to 64 to not waste space
};

struct object_s {
    int materialType;
    int meshId;
    mesh_t* mesh;
    point_t origin;
    float scale[3];
    float angleRotation[3];
    float rotationMatrix[9];
    void* material_ptr;
};
//-----------------------------------------------------------------------------------------------------------------------
// Enums
//-----------------------------------------------------------------------------------------------------------------------
enum materialType_e{
    MT_COLOR_UNIFORM = 0,
    MT_COLOR_EACH,
    MT_BITMAP
};
enum lightType_e{
    LT_ambiant = 0,
    LT_point,
    LT_directional
};
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------------------------------------
int OBJ_readObjFile(char* filePath_ptr, int mesh_id, mesh_t* ret_ptr);

#endif /* OBJECTS_H */