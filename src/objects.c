//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "objects.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
float min(float t1, float t2, float t3){
    if(t1 <= t2 && t1 <= t3) return t1;
    if(t2 <= t1 && t2 <= t3) return t2;
    return t3;
}
float max(float t1, float t2, float t3){
    if(t1 >= t2 && t1 >= t3) return t1;
    if(t2 >= t1 && t2 >= t3) return t2;
    return t3;
}

void isInInterval(float vmin, float vmax, float origin, float direction, float tmin, float tmax, float* retmin, float* retmax){
    if(direction == 0){
        if (vmin > origin || vmax < origin){
            *retmin = tmax + 1;
            *retmax = tmin - 1;
            return;
        }
        *retmin = tmin;
        *retmax = tmax;
        return;
    }
    if(direction < 0){
        *retmin = (vmax - origin) / direction;
        *retmax = (vmin - origin) / direction;
        return;
    }
    *retmin = (vmin - origin) / direction;
    *retmax = (vmax - origin) / direction;
}
//-----------------------------------------------------------------------------------------------------------------------
// Intersect Functions
//-----------------------------------------------------------------------------------------------------------------------
static float OBJ_intersectSphere(point_t* origin_ptr, point_t* lightVector_ptr, void* content_ptr, float tmin, float tmax){
    sphere_t* sphere_ptr = (sphere_t*) content_ptr;
    if(sphere_ptr->radius <= 0.0001){
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
    if(t1 > tmax && t2 > tmax){
        return tmax + 1;
    }
    // valid t values
    if(t2 < t1){
        return t2;
    }
    return t1;
}

static float OBJ_intersectCube(point_t* origin_ptr, point_t* lightVector_ptr, void* content_ptr, float tmin, float tmax){
    cube_t* cube_ptr = (cube_t*) content_ptr;
    if(cube_ptr == NULL){
        return tmax + 1;
    }
    // value that determine where is the intersection between the ray and a sphere
    float txmin;
    float txmax;
    isInInterval(cube_ptr->vectorMin.x, cube_ptr->vectorMax.x, origin_ptr->x, lightVector_ptr->x, tmin, tmax, &txmin, &txmax);
    if(txmin > txmax){
        return tmax + 1;
    }
    float tymin;
    float tymax;
    isInInterval(cube_ptr->vectorMin.y, cube_ptr->vectorMax.y, origin_ptr->y, lightVector_ptr->y, tmin, tmax, &tymin, &tymax);
    if(tymin > tymax){
        return tmax + 1;
    }
    float tzmin;
    float tzmax;
    isInInterval(cube_ptr->vectorMin.z, cube_ptr->vectorMax.z, origin_ptr->z, lightVector_ptr->z, tmin, tmax, &tzmin, &tzmax);
    if(tzmin > tzmax){
        return tmax + 1;
    }
    float te = max(txmin, tymin, tzmin);
    float ts = min(txmax, tymax, tzmax);
    if(ts < te || ts < 0) return tmax + 1;
    if(te < tmin || te > tmax) return tmax + 1;
    return te;
}

//------ Only shared function
float OBJ_intersectObject(point_t* origin_ptr, point_t* lightVector_ptr, object_t* object_ptr, float tmin, float tmax){
    switch (object_ptr->type){
        case OT_sphere:
            return OBJ_intersectSphere(origin_ptr, lightVector_ptr, object_ptr->content_ptr, tmin, tmax);
        case OT_cube:
            return OBJ_intersectCube(origin_ptr, lightVector_ptr, object_ptr->content_ptr, tmin, tmax);
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

static vector_t* OBJ_normalCube(cube_t* cube_ptr, point_t* pointOnCube_ptr){
    vector_t* ret = calloc(1, sizeof(vector_t));
    if(fabs(pointOnCube_ptr->x - cube_ptr->vectorMin.x) < EPSILON){
        ret->x = -1;
        ret->y = 0;
        ret->z = 0;
        return ret;
    }
    if(fabs(pointOnCube_ptr->x - cube_ptr->vectorMax.x) < EPSILON){
        ret->x = 1;
        ret->y = 0;
        ret->z = 0;
        return ret;
    }
    if(fabs(pointOnCube_ptr->y - cube_ptr->vectorMin.y) < EPSILON){
        ret->x = 0;
        ret->y = -1;
        ret->z = 0;
        return ret;
    }
    if(fabs(pointOnCube_ptr->y - cube_ptr->vectorMax.y) < EPSILON){
        ret->x = 0;
        ret->y = 1;
        ret->z = 0;
        return ret;
    }
    if(fabs(pointOnCube_ptr->z - cube_ptr->vectorMin.z) < EPSILON){
        ret->x = 0;
        ret->y = 0;
        ret->z = -1;
        return ret;
    }
    ret->x = 0;
    ret->y = 0;
    ret->z = 1;
    return ret;
}

//------ Only shared function
vector_t* OBJ_normalObject(object_t* object_ptr, point_t* pointOnObject_ptr){
    switch (object_ptr->type){
        case OT_sphere:
            return OBJ_normalSphere(object_ptr->content_ptr, pointOnObject_ptr);
        case OT_cube:
            return OBJ_normalCube(object_ptr->content_ptr, pointOnObject_ptr);
        default:
            return NULL;
    }
}