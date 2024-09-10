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
static float determinant(float matrix[9]) {
    return matrix[0] * (matrix[4] * matrix[8] - matrix[5] * matrix[7]) -
           matrix[1] * (matrix[3] * matrix[8] - matrix[5] * matrix[6]) +
           matrix[2] * (matrix[3] * matrix[7] - matrix[4] * matrix[6]);
}

static void cofactor(float matrix[9], float cofactors[9]) {
    cofactors[0] =  (matrix[4] * matrix[8] - matrix[5] * matrix[7]);
    cofactors[1] = -(matrix[3] * matrix[8] - matrix[5] * matrix[6]);
    cofactors[2] =  (matrix[3] * matrix[7] - matrix[4] * matrix[6]);

    cofactors[3] = -(matrix[1] * matrix[8] - matrix[2] * matrix[7]);
    cofactors[4] =  (matrix[0] * matrix[8] - matrix[2] * matrix[6]);
    cofactors[5] = -(matrix[0] * matrix[7] - matrix[1] * matrix[6]);

    cofactors[6] =  (matrix[1] * matrix[5] - matrix[2] * matrix[4]);
    cofactors[7] = -(matrix[0] * matrix[5] - matrix[2] * matrix[3]);
    cofactors[8] =  (matrix[0] * matrix[4] - matrix[1] * matrix[3]);
}

static void transpose(float matrix[9], float result[9]) {
    result[0] = matrix[0];
    result[1] = matrix[3];
    result[2] = matrix[6];
    result[3] = matrix[1];
    result[4] = matrix[4];
    result[5] = matrix[7];
    result[6] = matrix[2];
    result[7] = matrix[5];
    result[8] = matrix[8];
}
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

vector_t* COO_matrixVectorProduct(float matrix[9], vector_t* vector_ptr){
    vector_t* ret = calloc(1, sizeof(vector_t));
    ret->x = matrix[0] * vector_ptr->x +  matrix[1] * vector_ptr->y +  matrix[2] * vector_ptr->z; 
    ret->y = matrix[3] * vector_ptr->x +  matrix[4] * vector_ptr->y +  matrix[5] * vector_ptr->z; 
    ret->z = matrix[6] * vector_ptr->x +  matrix[7] * vector_ptr->y +  matrix[8] * vector_ptr->z; 
    return ret;
}

int COO_calculateInverse(float matrix[9], float result[9]) {
    float det = determinant(matrix);
    if (det == 0) {
        return 0; // La matrice n'a pas d'inverse
    }

    float cofactors[9];
    cofactor(matrix, cofactors);

    float adjugate[9];
    transpose(cofactors, adjugate);

    // Multiplier la matrice adjointe par 1/déterminant
    for (int i = 0; i < 9; i++) {
        result[i] = adjugate[i] / det;
    }

    return 1; // Inverse calculé avec succès
}

void COO_applyRotationMatrice(vector_t* vector_ptr, float theta, float phi, float psi){
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