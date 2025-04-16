/* objects.h
 * date : 08/09/2024 (dd/mm/yy)
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
typedef struct cylinder_s cylinder_t;
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

struct cylinder_s{
    point_t center;
    float radius;
    float height;
    float rotateX;
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
    OT_cube,
    OT_cylinder
};
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------------------------------------
/**
 * @brief Given an origin O and a direction D, return the lowest t such that O + t * D intersect an object.
 * 
 * @param origin_ptr Origin point.
 * @param lightVector_ptr Direction vector.
 * @param object_ptr Object to intersect.
 * @param tmin Minimum value that t should take.
 * @param tmax Maximum value that t should take.
 * @return The value t if it is in range. tmax + 1 else.
 */
float OBJ_intersectObject(point_t* origin_ptr, point_t* lightVector_ptr, object_t* object_ptr, float tmin, float tmax);

/**
 * @brief Given an object and a point on the object, return the normal vector for the given point. Convention : the normal goes inside the object.
 * 
 * @param object_ptr The object.
 * @param pointOnObject_ptr The point on the object.
 * @return The normal vector (it goes inside the object and not outside).
 */
vector_t* OBJ_normalObject(object_t* object_ptr, vector_t* pointOnObject_ptr);

/**
 * @brief Check if an object can be considered as well defined.
 * 
 * @param object_ptr The object to check.
 * @return 1 it is safe, 0 it isnt. 
 */
int OBJ_checkObject(object_t* object_ptr);

/**
 * @brief Apply several functions if needed.
 * 
 * @param object_ptr The object to initialize.
 */
void OBJ_initObject(object_t* object_ptr);

#endif /* OBJECTS_H */