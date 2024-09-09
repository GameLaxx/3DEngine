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
coordinate_t* COO_copyCoordinates(coordinate_t* coo1_ptr){
    coordinate_t *ret_ptr = calloc(1, sizeof(coordinate_t));
    ret_ptr->x = coo1_ptr->x;
    ret_ptr->y = coo1_ptr->y;
    ret_ptr->z = coo1_ptr->z;
    return ret_ptr;
}

int COO_applyFactor(coordinate_t* coo_ptr, float factor, int type){
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

coordinate_t* COO_linearTransformation(coordinate_t* coo1_ptr, float firstCoeff, coordinate_t* coo2_ptr, float secondCoeff){
    if(coo1_ptr == NULL && coo2_ptr == NULL) return NULL;
    if(coo1_ptr == NULL) return COO_linearTransformation(coo2_ptr, secondCoeff, coo1_ptr, firstCoeff);
    coordinate_t *ret_ptr = calloc(1, sizeof(coordinate_t));
    if(coo2_ptr == NULL){
        ret_ptr->x = coo1_ptr->x * firstCoeff;
        ret_ptr->y = coo1_ptr->y * firstCoeff;
        ret_ptr->z = coo1_ptr->z * firstCoeff;
    }else{
        ret_ptr->x = coo1_ptr->x * firstCoeff + coo2_ptr->x * secondCoeff;
        ret_ptr->y = coo1_ptr->y * firstCoeff + coo2_ptr->y * secondCoeff;
        ret_ptr->z = coo1_ptr->z * firstCoeff + coo2_ptr->z * secondCoeff;
    }
    return ret_ptr;
}

vector_t* COO_vectorizePoints(point_t* p1_ptr, point_t* p2_ptr){
    return COO_linearTransformation(p1_ptr, -1, p2_ptr, 1);
}

float COO_scalarProduct(coordinate_t* coo1_ptr, coordinate_t* coo2_ptr){
    return coo1_ptr->x * coo2_ptr->x + coo1_ptr->y * coo2_ptr->y + coo1_ptr->z * coo2_ptr->z;
}

vector_t* COO_matrixVectorProduct(float matrix[3][3], vector_t* vector_ptr){
    vector_t* ret = calloc(1, sizeof(vector_t));
    ret->x = matrix[0][0] * vector_ptr->x +  matrix[0][1] * vector_ptr->y +  matrix[0][2] * vector_ptr->z; 
    ret->y = matrix[1][0] * vector_ptr->x +  matrix[1][1] * vector_ptr->y +  matrix[1][2] * vector_ptr->z; 
    ret->z = matrix[2][0] * vector_ptr->x +  matrix[2][1] * vector_ptr->y +  matrix[2][2] * vector_ptr->z; 
    return ret;
}

void COO_applyRotationMatrice(vector_t* vector_ptr, float theta, float phi, float psi){
    float theta_rad = theta * M_PI / 180;
    float phi_rad = phi * M_PI / 180;
    float psi_rad = psi * M_PI / 180;
    float nx = vector_ptr->x * cos(phi_rad) * cos(psi_rad) \
              + vector_ptr->y * (cos(psi_rad) * sin(phi_rad) * sin(theta_rad) - sin(psi_rad) * cos(theta_rad)) \
              + vector_ptr->z * (sin(psi_rad * sin(theta_rad) + cos(psi_rad) * sin(phi_rad) * cos(theta_rad)));
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