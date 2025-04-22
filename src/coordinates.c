//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include "coordinates.h"
#include <math.h>
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int COO_copyCoordinates(coordinate_t* coo1_ptr, coordinate_t* ret_ptr){
    ret_ptr->x = coo1_ptr->x;
    ret_ptr->y = coo1_ptr->y;
    ret_ptr->z = coo1_ptr->z;
    return EXIT_SUCCESS;
}

int COO_lambdaProduct(coordinate_t* coo_ptr, float factor, int type){
    if(factor == 0){
        return EXIT_FAILURE;
    }
    switch (type)
    {
        case FT_MULT:
            coo_ptr->x *= factor;
            coo_ptr->y *= factor;
            coo_ptr->z *= factor;
            break;
        case FT_DIV:
            coo_ptr->x /= factor;
            coo_ptr->y /= factor;
            coo_ptr->z /= factor;
            break; 
        case FT_PLUS:
            coo_ptr->x += factor;
            coo_ptr->y += factor;
            coo_ptr->z += factor;
            break; 
        case FT_MIN:
            coo_ptr->x -= factor;
            coo_ptr->y -= factor;
            coo_ptr->z -= factor;
            break; 
        default:
            break;
    }
    return EXIT_SUCCESS;
}

int COO_linearTransformation(coordinate_t* coo1_ptr, float factor1, coordinate_t* coo2_ptr, float factor2, coordinate_t* ret_ptr){
    if(coo1_ptr == NULL && coo2_ptr == NULL) return EXIT_FAILURE;
    if(coo1_ptr == NULL) return COO_linearTransformation(coo2_ptr, factor2, coo1_ptr, factor1, ret_ptr);
    if(coo2_ptr == NULL){
        ret_ptr->x = coo1_ptr->x * factor1;
        ret_ptr->y = coo1_ptr->y * factor1;
        ret_ptr->z = coo1_ptr->z * factor1;
    }else{
        ret_ptr->x = coo1_ptr->x * factor1 + coo2_ptr->x * factor2;
        ret_ptr->y = coo1_ptr->y * factor1 + coo2_ptr->y * factor2;
        ret_ptr->z = coo1_ptr->z * factor1 + coo2_ptr->z * factor2;
    }
    return EXIT_SUCCESS;
}

int COO_vectorizePoints(point_t* p1_ptr, point_t* p2_ptr, point_t* ret_ptr){
    return COO_linearTransformation(p1_ptr, -1, p2_ptr, 1, ret_ptr);
}

float COO_scalarProduct(coordinate_t* coo1_ptr, coordinate_t* coo2_ptr){
    return coo1_ptr->x * coo2_ptr->x + coo1_ptr->y * coo2_ptr->y + coo1_ptr->z * coo2_ptr->z;
}

int COO_crossProduct(vector_t* vector1_ptr, vector_t* vector2_ptr, vector_t* ret_ptr){
    ret_ptr->x = vector1_ptr->y * vector2_ptr->z - vector1_ptr->z * vector2_ptr->y;
    ret_ptr->y = vector1_ptr->z * vector2_ptr->x - vector1_ptr->x * vector2_ptr->z;
    ret_ptr->z = vector1_ptr->x * vector2_ptr->y - vector1_ptr->y * vector2_ptr->x;
    return EXIT_SUCCESS;
}

int COO_matrixVectorProduct(float matrix[9], vector_t* vector_ptr, vector_t* ret_ptr){
    ret_ptr->x = matrix[0] * vector_ptr->x +  matrix[1] * vector_ptr->y +  matrix[2] * vector_ptr->z; 
    ret_ptr->y = matrix[3] * vector_ptr->x +  matrix[4] * vector_ptr->y +  matrix[5] * vector_ptr->z; 
    ret_ptr->z = matrix[6] * vector_ptr->x +  matrix[7] * vector_ptr->y +  matrix[8] * vector_ptr->z; 
    return EXIT_SUCCESS;
}

void COO_rotationVectorProduct(vector_t* vector_ptr, float theta, float phi, float psi){
    float theta_rad = theta * M_PI / 180;
    float phi_rad = phi * M_PI / 180;
    float psi_rad = psi * M_PI / 180;
    float nx = vector_ptr->x * cos(phi_rad) * cos(psi_rad) \
              + vector_ptr->y * (cos(psi_rad) * sin(phi_rad) * sin(theta_rad) - sin(psi_rad) * cos(theta_rad)) \
              + vector_ptr->z * (sin(psi_rad) * sin(theta_rad) + cos(psi_rad) * sin(phi_rad) * cos(theta_rad));
    float ny = vector_ptr->x * cos(phi_rad) * sin(psi_rad) \
              + vector_ptr->y * (sin(psi_rad) * sin(phi_rad) * sin(theta_rad) + cos(psi_rad) * cos(theta_rad)) \
              + vector_ptr->z * (- cos(psi_rad) * sin(theta_rad) + sin(psi_rad) * sin(phi_rad) * cos(theta_rad));
    float nz = - vector_ptr->x * sin(phi_rad) \
              + vector_ptr->y * cos(phi_rad) * sin(theta_rad) \
              + vector_ptr->z * cos(phi_rad) * cos(theta_rad);
    vector_ptr->x = nx; 
    vector_ptr->y = ny;
    vector_ptr->z = nz;
}