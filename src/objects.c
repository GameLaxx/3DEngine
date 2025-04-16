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
static float OBJ_intersectSphere(point_t* origin_ptr, point_t* lightVector_ptr, sphere_t* sphere_ptr, float tmin, float tmax){
    if(sphere_ptr->radius <= 0.0001){
        return tmax + 1;
    }
    // value that determine where is the intersection between the ray and a sphere
    vector_t vector = {};
    if(COO_vectorizePoints(&sphere_ptr->center, origin_ptr, &vector) == EXIT_FAILURE){
        return tmax + 1;
    }
    float a = COO_scalarProduct(lightVector_ptr, lightVector_ptr);
    float b = 2 * COO_scalarProduct(&vector, lightVector_ptr);
    float c = COO_scalarProduct(&vector, &vector) - sphere_ptr->radius * sphere_ptr->radius;
    float delta = b * b - 4 * a * c;
    if(delta < 0){
        return tmax + 1;
    }
    double t1 = (- b - sqrt(delta)) / (2.f * a);
    double t2 = (- b + sqrt(delta)) / (2.f * a);
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
    point_t translatedOrigin = {};
    if(COO_vectorizePoints(&cube_ptr->center, origin_ptr, &translatedOrigin) == EXIT_FAILURE){
        return tmax + 1;
    }
    point_t newOrigin = {};
    if(COO_matrixVectorProduct(cube_ptr->invertRotationMatrice, &translatedOrigin, &newOrigin) == EXIT_FAILURE){
        return tmax + 1;
    }
    point_t newLightVector = {};
    if(COO_matrixVectorProduct(cube_ptr->invertRotationMatrice, lightVector_ptr, &newLightVector) == EXIT_FAILURE){
        return tmax + 1;
    }
    // value that determine where is the intersection between the ray and a sphere
    float xmin = - cube_ptr->extendVector.x;
    float xmax = cube_ptr->extendVector.x;
    float txmin;
    float txmax;
    isInInterval(xmin, xmax, newOrigin.x, newLightVector.x, tmin, tmax, &txmin, &txmax);
    if(txmin > txmax){
        return tmax + 1;
    }
    float ymin = - cube_ptr->extendVector.y;
    float ymax = cube_ptr->extendVector.y;
    float tymin;
    float tymax;
    isInInterval(ymin, ymax, newOrigin.y, newLightVector.y, tmin, tmax, &tymin, &tymax);
    if(tymin > tymax){
        return tmax + 1;
    }
    float zmin = - cube_ptr->extendVector.z;
    float zmax = cube_ptr->extendVector.z;
    float tzmin;
    float tzmax;
    isInInterval(zmin, zmax, newOrigin.z, newLightVector.z, tmin, tmax, &tzmin, &tzmax);
    if(tzmin > tzmax){
        return tmax + 1;
    }
    float te = max(txmin, tymin, tzmin);
    float ts = min(txmax, tymax, tzmax);
    if(ts < te || ts < 0) return tmax + 1;
    if(te < tmin || te > tmax) return tmax + 1;
    return te;
}

static float OBJ_intersectCylinder(point_t* origin_ptr, point_t* lightVector_ptr, cylinder_t* cylinder_ptr, float tmin, float tmax){
    // Calculate origin and lightVector in the new reference
    point_t translatedOrigin = {};
    if(COO_vectorizePoints(&cylinder_ptr->center, origin_ptr, &translatedOrigin) == EXIT_FAILURE){
        return tmax + 1;
    }
    point_t newOrigin = {}; 
    if(COO_matrixVectorProduct(cylinder_ptr->invertRotationMatrice, &translatedOrigin, &newOrigin) == EXIT_FAILURE){
        return tmax + 1;
    }
    point_t newLightVector = {};
    if(COO_matrixVectorProduct(cylinder_ptr->invertRotationMatrice, lightVector_ptr, &newLightVector) == EXIT_FAILURE){
        return tmax + 1;
    }
    
    float ox = newOrigin.x;
    float oz = newOrigin.z;
    float dx = newLightVector.x;
    float dz = newLightVector.z;
    
    float a = dx * dx + dz * dz;
    float b = 2 * (ox * dx + oz * dz);
    float c = ox * ox + oz * oz - cylinder_ptr->radius * cylinder_ptr->radius;
    
    float delta = b * b - 4 * a * c;
    if(delta < 0){
        return tmax + 1;
    }
    double t1 = (- b - sqrt(delta)) / (2.f * a);
    double t2 = (- b + sqrt(delta)) / (2.f * a);
    
    // unvalid t values for the current sphere
    if(t1 < tmin && t2 < tmin){
        return tmax + 1;
    }
    if(t1 > tmax && t2 > tmax){
        return tmax + 1;
    }
    float t = t1;
    if(t2 < t1){
        t = t2;
    }
    
    // Check if the ray is in range for y positions
    float y = newOrigin.y + t * newLightVector.y;
    if (y >= - cylinder_ptr->height && y <= cylinder_ptr->height) {
        return t;
    }

    // Light ray is `//` to Oy
    if(newLightVector.y == 0){
        return tmax + 1;
    }

    float tBottom = (- cylinder_ptr->height - newOrigin.y) / newLightVector.y;
    if (tBottom > tmin && tBottom < tmax) {
        // Vérifie si l'intersection est dans le rayon du cylindre
        float xBottom = newOrigin.x + tBottom * newLightVector.x;
        float zBottom = newOrigin.z + tBottom * newLightVector.z;
        if (xBottom * xBottom + zBottom * zBottom <= cylinder_ptr->radius * cylinder_ptr->radius) {
            return tBottom;
        }
    }
    
    float tTop = (cylinder_ptr->height - newOrigin.y) / newLightVector.y;
    if (tTop > tmin && tTop < tmax) {
        // Vérifie si l'intersection est dans le rayon du cylindre
        float xTop = newOrigin.x + tTop * newLightVector.x;
        float zTop = newOrigin.z + tTop * newLightVector.z;
        if (xTop * xTop + zTop * zTop <= cylinder_ptr->radius * cylinder_ptr->radius) {
            return tTop;
        }
    }
    return tmax + 1;
}

//------ Only shared function
float OBJ_intersectObject(point_t* origin_ptr, point_t* lightVector_ptr, object_t* object_ptr, float tmin, float tmax){
    switch (object_ptr->type){
        case OT_sphere:
            return OBJ_intersectSphere(origin_ptr, lightVector_ptr, object_ptr->content_ptr, tmin, tmax);
        case OT_cube:
            return OBJ_intersectCube(origin_ptr, lightVector_ptr, object_ptr->content_ptr, tmin, tmax);
        case OT_cylinder:
            return OBJ_intersectCylinder(origin_ptr, lightVector_ptr, object_ptr->content_ptr, tmin, tmax);
        default:
            return tmax + 1;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
// Normal Functions
//-----------------------------------------------------------------------------------------------------------------------
static int OBJ_normalSphere(sphere_t* sphere_ptr, point_t* pointOnSphere_ptr, vector_t* ret_ptr){
    return COO_vectorizePoints(pointOnSphere_ptr, &sphere_ptr->center, ret_ptr);
}

static int OBJ_normalCube(cube_t* cube_ptr, point_t* pointOnCube_ptr, vector_t* ret_ptr){
    vector_t tmp;
    vector_t pointNewOrigin = {};
    if(COO_vectorizePoints(&cube_ptr->center, pointOnCube_ptr, &pointNewOrigin) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    vector_t pointNewReference = {};
    if(COO_matrixVectorProduct(cube_ptr->invertRotationMatrice, &pointNewOrigin, &pointNewReference) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    if(fabs(pointNewReference.x + cube_ptr->extendVector.x) < EPSILON){
        tmp.x = 1;
        tmp.y = 0;
        tmp.z = 0;
    }else if(fabs(pointNewReference.x - cube_ptr->extendVector.x) < EPSILON){
        tmp.x = -1;
        tmp.y = 0;
        tmp.z = 0;
    }else if(fabs(pointNewReference.y + cube_ptr->extendVector.y) < EPSILON){
        tmp.x = 0;
        tmp.y = 1;
        tmp.z = 0;
    }else if(fabs(pointNewReference.y - cube_ptr->extendVector.y) < EPSILON){
        tmp.x = 0;
        tmp.y = -1;
        tmp.z = 0;
    }else if(fabs(pointNewReference.z + cube_ptr->extendVector.z) < EPSILON){
        tmp.x = 0;
        tmp.y = 0;
        tmp.z = 1;
    }else if(fabs(pointNewReference.z - cube_ptr->extendVector.z) < EPSILON){
        tmp.x = 0;
        tmp.y = 0;
        tmp.z = -1;
    }
    return COO_matrixVectorProduct(cube_ptr->rotationMatrice, &tmp, ret_ptr);
}

static int OBJ_normalCylinder(cylinder_t* cylinder_ptr, point_t* pointOnCylinder_ptr, vector_t* ret_ptr) {
    vector_t pointNewOrigin = {};
    if(COO_vectorizePoints(&cylinder_ptr->center, pointOnCylinder_ptr, &pointNewOrigin) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    vector_t pointNewReference = {};
    if(COO_matrixVectorProduct(cylinder_ptr->invertRotationMatrice, &pointNewOrigin, &pointNewReference) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    vector_t tmp;
    if(pointNewReference.y == cylinder_ptr->height){
        tmp.y = -1;
    }else if(pointNewReference.y == -cylinder_ptr->height){
        tmp.y = 1;
    }else{
        tmp.x = -pointNewReference.x;
        tmp.z = -pointNewReference.z;
    }
    return COO_matrixVectorProduct(cylinder_ptr->rotationMatrice, &tmp, ret_ptr);
}

//------ Only shared function
int OBJ_normalObject(object_t* object_ptr, point_t* pointOnObject_ptr, vector_t* ret_ptr){
    switch (object_ptr->type){
        case OT_sphere:
            return OBJ_normalSphere(object_ptr->content_ptr, pointOnObject_ptr, ret_ptr);
        case OT_cube:
            return OBJ_normalCube(object_ptr->content_ptr, pointOnObject_ptr, ret_ptr);
        case OT_cylinder:
            return OBJ_normalCylinder(object_ptr->content_ptr, pointOnObject_ptr, ret_ptr);
        default:
            return EXIT_FAILURE;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
// Check Functions
//-----------------------------------------------------------------------------------------------------------------------
static int OBJ_checkObjType(object_t* object_ptr){
    return 1;
    switch (object_ptr->type){
        case OT_sphere:
            return sizeof(*object_ptr->content_ptr) == sizeof(sphere_t);
        case OT_cube:
            return sizeof(*object_ptr->content_ptr) == sizeof(cube_t);
        case OT_cylinder:
            return sizeof(*object_ptr->content_ptr) == sizeof(cylinder_t);
        default:
            return 0;
    }
}

static int OBJ_checkSphere(sphere_t* sphere_ptr){
    return sphere_ptr->radius > 0;
}

static int OBJ_checkCube(cube_t* cube_ptr){
    return cube_ptr->extendVector.x >= 0 && cube_ptr->extendVector.y >= 0 && cube_ptr->extendVector.z >= 0;
}

static int OBJ_checkCylinder(cylinder_t* cylinder_ptr){
    return cylinder_ptr->height > 0 && cylinder_ptr->radius > 0;
}
//------ Only shared function
int OBJ_checkObject(object_t* object_ptr){
    // Object type and content pointer are not the same
    if(OBJ_checkObjType(object_ptr) == 0) return 0;
    // Check each type of object indivually
    switch (object_ptr->type){
        case OT_sphere:
            return OBJ_checkSphere(object_ptr->content_ptr); 
        case OT_cube:
            return OBJ_checkCube(object_ptr->content_ptr); 
        case OT_cylinder:
            return OBJ_checkCylinder(object_ptr->content_ptr); 
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
    // rotation
    cube_ptr->rotationMatrice[0] = cos(rotateY_rad) * cos(rotateZ_rad);
    cube_ptr->rotationMatrice[1] = cos(rotateZ_rad) * sin(rotateY_rad) * sin(rotateX_rad) - sin(rotateZ_rad) * cos(rotateX_rad);
    cube_ptr->rotationMatrice[2] = sin(rotateZ_rad) * sin(rotateX_rad) + cos(rotateZ_rad) * sin(rotateY_rad) * cos(rotateX_rad);
    cube_ptr->rotationMatrice[3] = cos(rotateY_rad) * sin(rotateZ_rad);
    cube_ptr->rotationMatrice[4] = sin(rotateZ_rad) * sin(rotateY_rad) * sin(rotateX_rad) + cos(rotateZ_rad) * cos(rotateX_rad);
    cube_ptr->rotationMatrice[5] = - cos(rotateZ_rad) * sin(rotateX_rad) + sin(rotateZ_rad) * sin(rotateY_rad) * cos(rotateX_rad);
    cube_ptr->rotationMatrice[6] = -sin(rotateY_rad);
    cube_ptr->rotationMatrice[7] = cos(rotateY_rad) * sin(rotateX_rad);
    cube_ptr->rotationMatrice[8] = cos(rotateY_rad) * cos(rotateX_rad);
    // inverse
    cube_ptr->invertRotationMatrice[0] = cos(rotateY_rad) * cos(rotateZ_rad);
    cube_ptr->invertRotationMatrice[1] = cos(rotateY_rad) * sin(rotateZ_rad);
    cube_ptr->invertRotationMatrice[2] = -sin(rotateY_rad); 
    cube_ptr->invertRotationMatrice[3] = cos(rotateZ_rad) * sin(rotateY_rad) * sin(rotateX_rad) - sin(rotateZ_rad) * cos(rotateX_rad);
    cube_ptr->invertRotationMatrice[4] = sin(rotateZ_rad) * sin(rotateY_rad) * sin(rotateX_rad) + cos(rotateZ_rad) * cos(rotateX_rad);
    cube_ptr->invertRotationMatrice[5] = sin(rotateX_rad) * cos(rotateY_rad);
    cube_ptr->invertRotationMatrice[6] = sin(rotateZ_rad) * sin(rotateX_rad) + cos(rotateZ_rad) * sin(rotateY_rad) * cos(rotateX_rad);
    cube_ptr->invertRotationMatrice[7] = cos(rotateX_rad) * sin(rotateY_rad) * sin(rotateZ_rad) - sin(rotateX_rad) * cos(rotateZ_rad);
    cube_ptr->invertRotationMatrice[8] = cos(rotateX_rad) * cos(rotateY_rad);
}
void OBJ_initCylinder(cylinder_t* cylinder_ptr){
    float rotateX_rad = cylinder_ptr->rotateX * M_PI / 180;
    float rotateZ_rad = cylinder_ptr->rotateZ * M_PI / 180;
    // rotation
    cylinder_ptr->rotationMatrice[0] = cos(rotateZ_rad);
    cylinder_ptr->rotationMatrice[1] = - sin(rotateZ_rad) * cos(rotateX_rad);
    cylinder_ptr->rotationMatrice[2] = sin(rotateZ_rad) * sin(rotateX_rad);
    cylinder_ptr->rotationMatrice[3] = sin(rotateZ_rad);
    cylinder_ptr->rotationMatrice[4] = cos(rotateZ_rad) * cos(rotateX_rad);
    cylinder_ptr->rotationMatrice[5] = - cos(rotateZ_rad) * sin(rotateX_rad);
    cylinder_ptr->rotationMatrice[6] = 0;
    cylinder_ptr->rotationMatrice[7] = sin(rotateX_rad);
    cylinder_ptr->rotationMatrice[8] = cos(rotateX_rad);
    // inverse
    cylinder_ptr->invertRotationMatrice[0] = cos(rotateZ_rad);
    cylinder_ptr->invertRotationMatrice[1] = sin(rotateZ_rad);
    cylinder_ptr->invertRotationMatrice[2] = 0; 
    cylinder_ptr->invertRotationMatrice[3] = - sin(rotateZ_rad) * cos(rotateX_rad);
    cylinder_ptr->invertRotationMatrice[4] = cos(rotateZ_rad) * cos(rotateX_rad);
    cylinder_ptr->invertRotationMatrice[5] = sin(rotateX_rad);
    cylinder_ptr->invertRotationMatrice[6] = sin(rotateZ_rad) * sin(rotateX_rad);
    cylinder_ptr->invertRotationMatrice[7] = - sin(rotateX_rad) * cos(rotateZ_rad);
    cylinder_ptr->invertRotationMatrice[8] = cos(rotateX_rad);
}
//------ Only shared function
void OBJ_initObject(object_t* object_ptr){
    switch (object_ptr->type){
        case OT_cube:
            OBJ_initCube(object_ptr->content_ptr); 
            break;
        case OT_cylinder:
            OBJ_initCylinder(object_ptr->content_ptr); 
            break;
        default:
            break;
    }
}