/* objects.h
 * date : $$DAY$$/$$MONTH$$/$$YEAR$$ (dd/mm/yy)
 * author : tboisse
*/
#ifndef OBJECTS_H
#define OBJECTS_H
//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "coordinates.h"
#include "draw.h"
//-----------------------------------------------------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------------------------------------------------
#define EPSILON 0.000001
//-----------------------------------------------------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------------------------------------------------
typedef struct sphere_s sphere_t;
typedef struct cube_s cube_t;
typedef struct object_s object_t;
//-----------------------------------------------------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------------------------------------------------
struct sphere_s{
    point_t center;
    float radius;
};

struct cube_s{
    point_t center;
    vector_t extendVector;
    float rotateX;
    float rotateY;
    float rotateZ;
    float rotationMatrice[9];
    float invertRotationMatrice[9];
};

struct object_s{
    void* content_ptr;
    int type;
    rgba_t color;
    int specular;
    float reflective;
};
//-----------------------------------------------------------------------------------------------------------------------
// Enums
//-----------------------------------------------------------------------------------------------------------------------
enum OBJ_objectType_e{
    OT_NAO = 0, // not an object
    OT_sphere = 1,
    OT_cube
};
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------------------------------------

float OBJ_intersectObject(point_t* origin_ptr, point_t* lightVector_ptr, object_t* object_ptr, float tmin, float tmax);

vector_t* OBJ_normalObject(object_t* object_ptr, vector_t* pointOnObject_ptr);

int OBJ_checkObject(object_t* object_ptr);

void OBJ_initObject(object_t* object_ptr);

#endif /* OBJECTS_H */