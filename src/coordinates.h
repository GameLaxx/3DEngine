/* coordinates.h
 * date : 06/09/2024 (dd/mm/yy)
 * author : tboisse
*/
#ifndef COORDINATES_H
#define COORDINATES_H
//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------------------------------------------------
typedef struct coordinate_s coordinate_t;
typedef struct coordinate_s vector_t;
typedef struct coordinate_s point_t;
//-----------------------------------------------------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------------------------------------------------
/**
 * @brief Structure used to represent coordinates, points or even vectors in 3D (x,y,z).
 */
struct coordinate_s{
    float x; // x coordinate
    float y; // y coordinate
    float z; // z coordinate
};
//-----------------------------------------------------------------------------------------------------------------------
// Enums
//-----------------------------------------------------------------------------------------------------------------------
enum COO_factorType_e{
    FT_MULT = 0,
    FT_DIV,
    FT_PLUS,
    FT_MIN
};
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------------------------------------
/**
 * @brief Return a calloc copy of the given coordinate.
 * 
 * @param coo1_ptr The coordinate to copy.
 * @return The copied coordinate. Has to be free when not used anymore.
 */
coordinate_t* COO_copyCoordinates(coordinate_t* coo1_ptr);

int COO_applyFactor(coordinate_t* coo_ptr, float factor, int type);

/**
 * @brief Return a calloc coordinate made by calculating `a * v1 + b * v2` where a,b are numbers and v1,v2 vectors.
 * 
 * @param coo1_ptr The first coordinate (v1)
 * @param firstCoeff The first coefficient (a)
 * @param coo2_ptr The second coordinate (v2)
 * @param secondCoeff The second coefficient (b)
 * @return The newly made vector. Has to be free when not used anymore.
 */
coordinate_t* COO_linearTransformation(coordinate_t* coo1_ptr, float firstCoeff, coordinate_t* coo2_ptr, float secondCoeff);

/**
 * @brief Side use case of @ref COO_linearTransformation with `a = -1` and `b = 1`.
 * 
 * @param p1_ptr The first point (v1)
 * @param p2_ptr The second point (v2)
 * @return A vector made by `v2 - v1`. Has to be free when not used anymore.
 */
vector_t* COO_vectorizePoints(point_t* p1_ptr, point_t* p2_ptr);

/**
 * @brief The scalar product between v1 and v2 where v1,v2 are vectors.
 * 
 * @param coo1_ptr The first vector.
 * @param coo2_ptr The second vector.
 * @return The value of `< v1 . v2 >`.
 */
float COO_scalarProduct(coordinate_t* coo1_ptr, coordinate_t* coo2_ptr);

vector_t* COO_matrixVectorProduct(float matrix[3][3], vector_t* vector_ptr);

void COO_applyRotationMatrice(vector_t* vector_ptr, float theta, float phi, float psi);

#endif /* COORDINATES_H */