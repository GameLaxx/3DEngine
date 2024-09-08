//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "objects.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Intersect Functions
//-----------------------------------------------------------------------------------------------------------------------
static float OBJ_intersectSphere(point_t* origin_ptr, point_t* lightVector_ptr, void* content_ptr, float tmin, float tmax){
    sphere_t* sphere_ptr = (sphere_t*) content_ptr;
    if(sphere_ptr->radius == 0){
        return tmax + 1;
    }
    // value that determine where is the intersection between the ray and a sphere
    vector_t *vector = COO_vectorizePoints(&sphere_ptr->center, origin_ptr);
    float a = COO_scalarProduct(lightVector_ptr, lightVector_ptr);
    float b = 2 * COO_scalarProduct(vector, lightVector_ptr);
    float c = COO_scalarProduct(vector, vector) - sphere_ptr->radius * sphere_ptr->radius;
    free(vector);
    float delta = b * b - 4 * a * c;
    if(delta < 0){
        return tmax + 1;
    }
    double t1 = (- (float)b - sqrt(delta)) / (2.f * (float)a);
    double t2 = (-(float)b + sqrt(delta)) / (2.f * (float)a);
    // unvalid t values for the current sphere
    if(t1 < tmin && t2 < tmin){
        return tmax + 1;
    }
    if(t1 >= tmax && t2 >= tmax){
        return tmax + 1;
    }
    // valid t values
    if(t2 < t1){
        return t2;
    }
    return t1;
}

//------ Only shared function
float OBJ_intersectObject(point_t* origin_ptr, point_t* lightVector_ptr, object_t* object_ptr, float tmin, float tmax){
    switch (object_ptr->type){
    case OT_sphere:
        return OBJ_intersectSphere(origin_ptr, lightVector_ptr, object_ptr->content_ptr, tmin, tmax);
    default:
        return tmax + 1;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
// Normal Functions
//-----------------------------------------------------------------------------------------------------------------------
static vector_t* OBJ_normalSphere(sphere_t* sphere_ptr, point_t* pointOnSphere_ptr){
    return  COO_vectorizePoints(pointOnSphere_ptr, &sphere_ptr->center);
}

//------ Only shared function
vector_t* OBJ_normalObject(object_t* object_ptr, point_t* pointOnObject_ptr){
    switch (object_ptr->type){
    case OT_sphere:
        return OBJ_normalSphere(object_ptr->content_ptr, pointOnObject_ptr);
    default:
        return NULL;
    }
}