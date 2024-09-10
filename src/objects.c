//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "objects.h"
#include <math.h>
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

static float OBJ_intersectCube(point_t* origin_ptr, point_t* lightVector_ptr, cube_t* cube_ptr, float tmin, float tmax){
    if(cube_ptr == NULL || origin_ptr == NULL || lightVector_ptr == NULL){
        return tmax + 1;
    }
    // Calculate origin and lightVector in the new reference
    point_t* centerWithOrigin_ptr = COO_vectorizePoints(&cube_ptr->center, origin_ptr);
    point_t* newOrigin_ptr = COO_matrixVectorProduct(cube_ptr->invertRotationMatrice, centerWithOrigin_ptr);
    point_t* newLightVector_ptr = COO_matrixVectorProduct(cube_ptr->invertRotationMatrice, lightVector_ptr);
    // value that determine where is the intersection between the ray and a sphere
    float xmin = cube_ptr->center.x - cube_ptr->extendVector.x;
    float xmax = cube_ptr->center.x + cube_ptr->extendVector.x;
    float txmin;
    float txmax;
    isInInterval(xmin, xmax, newOrigin_ptr->x, newLightVector_ptr->x, tmin, tmax, &txmin, &txmax);
    if(txmin > txmax){
        free(centerWithOrigin_ptr);
        free(newOrigin_ptr);
        free(newLightVector_ptr);
        return tmax + 1;
    }
    float ymin = cube_ptr->center.y - cube_ptr->extendVector.y;
    float ymax = cube_ptr->center.y + cube_ptr->extendVector.y;
    float tymin;
    float tymax;
    isInInterval(ymin, ymax, newOrigin_ptr->y, newLightVector_ptr->y, tmin, tmax, &tymin, &tymax);
    if(tymin > tymax){
        free(centerWithOrigin_ptr);
        free(newOrigin_ptr);
        free(newLightVector_ptr);
        return tmax + 1;
    }
    float zmin = cube_ptr->center.z - cube_ptr->extendVector.z;
    float zmax = cube_ptr->center.z + cube_ptr->extendVector.z;
    float tzmin;
    float tzmax;
    isInInterval(zmin, zmax, newOrigin_ptr->z, newLightVector_ptr->z, tmin, tmax, &tzmin, &tzmax);
    if(tzmin > tzmax){
        free(centerWithOrigin_ptr);
        free(newOrigin_ptr);
        free(newLightVector_ptr);
        return tmax + 1;
    }
    free(centerWithOrigin_ptr);
    free(newOrigin_ptr);
    free(newLightVector_ptr);
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
    vector_t* pointNewOrigin_ptr = COO_vectorizePoints(&cube_ptr->center, pointOnCube_ptr);
    vector_t* pointNewReference_ptr = COO_matrixVectorProduct(cube_ptr->invertRotationMatrice, pointOnCube_ptr);
    if(fabs(pointNewReference_ptr->x + cube_ptr->extendVector.x) < EPSILON){
        ret->x = -1;
        ret->y = 0;
        ret->z = 0;
    }else if(fabs(pointNewReference_ptr->x - cube_ptr->extendVector.x) < EPSILON){
        ret->x = 1;
        ret->y = 0;
        ret->z = 0;
    }else if(fabs(pointNewReference_ptr->y + cube_ptr->extendVector.y) < EPSILON){
        ret->x = 0;
        ret->y = -1;
        ret->z = 0;
    }else if(fabs(pointNewReference_ptr->y - cube_ptr->extendVector.y) < EPSILON){
        ret->x = 0;
        ret->y = 1;
        ret->z = 0;
    }else if(fabs(pointNewReference_ptr->z + cube_ptr->extendVector.z) < EPSILON){
        ret->x = 0;
        ret->y = 0;
        ret->z = -1;
    }else if(fabs(pointNewReference_ptr->z - cube_ptr->extendVector.z) < EPSILON){
        ret->x = 0;
        ret->y = 0;
        ret->z = 1;
    }
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
//-----------------------------------------------------------------------------------------------------------------------
// Check Functions
//-----------------------------------------------------------------------------------------------------------------------
int OBJ_checkSphere(sphere_t* sphere_ptr){
    return sphere_ptr->radius > 0;
}

int OBJ_checkCube(cube_t* cube_ptr){
    return cube_ptr->extendVector.x >= 0 && cube_ptr->extendVector.y >= 0 && cube_ptr->extendVector.z >= 0;
}
//------ Only shared function
int OBJ_checkObject(object_t* object_ptr){
    switch (object_ptr->type){
        case OT_sphere:
            return OBJ_checkSphere(object_ptr->content_ptr); 
        case OT_cube:
            return OBJ_checkCube(object_ptr->content_ptr); 
        default:
            return 0;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
// Init Functions
//-----------------------------------------------------------------------------------------------------------------------
void OBJ_initCube(cube_t* cube_ptr){
    float rotateX_rad = cube_ptr->rotateX * M_PI / 180;
    float rotateY_rad = cube_ptr->rotateY * M_PI / 180;
    float rotateZ_rad = cube_ptr->rotateZ * M_PI / 180;
    cube_ptr->rotationMatrice[0] = cos(rotateY_rad) * cos(rotateZ_rad);
    cube_ptr->rotationMatrice[1] = cos(rotateZ_rad) * sin(rotateY_rad) * sin(rotateX_rad) - sin(rotateZ_rad) * cos(rotateX_rad);
    cube_ptr->rotationMatrice[2] = sin(rotateZ_rad) * sin(rotateX_rad) + cos(rotateZ_rad) * sin(rotateY_rad) * cos(rotateX_rad);
    cube_ptr->rotationMatrice[3] = cos(rotateY_rad) * sin(rotateZ_rad);
    cube_ptr->rotationMatrice[4] = sin(rotateZ_rad) * sin(rotateY_rad) * sin(rotateX_rad) + cos(rotateZ_rad) * cos(rotateX_rad);
    cube_ptr->rotationMatrice[5] = - cos(rotateZ_rad) * sin(rotateX_rad) + sin(rotateZ_rad) * sin(rotateY_rad) * cos(rotateX_rad);
    cube_ptr->rotationMatrice[6] = -sin(rotateY_rad);
    cube_ptr->rotationMatrice[7] = cos(rotateY_rad) * sin(rotateX_rad);
    cube_ptr->rotationMatrice[8] = cos(rotateY_rad) * cos(rotateX_rad);
    COO_calculateInverse(cube_ptr->rotationMatrice, cube_ptr->invertRotationMatrice);
}
//------ Only shared function
void OBJ_initObject(object_t* object_ptr){
    switch (object_ptr->type){
        case OT_cube:
            OBJ_initCube(object_ptr->content_ptr); 
            break;
        default:
            break;
    }
}