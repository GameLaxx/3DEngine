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
 * @brief Return a copy of the given coordinate without calloc.
 * 
 * @param coo1_ptr The coordinate to copy.
 * @param ret_ptr The coordinate copied.
 * @return SUCCESS or FAILURE.
 */
int COO_copyCoordinates(coordinate_t* coo1_ptr, coordinate_t* ret_ptr);

int COO_translatePoint(point_t* point_ptr, vector_t* vector_ptr);

/**
 * @brief Apply a factor to a vector using +,-,/ or * computations. This computation is in-place.
 * 
 * @param coo_ptr Vector that will be changed.
 * @param factor Factor that will be used. Should be != 0.
 * @param type @ref COO_factorType_e
 * @return SUCCESS or FAILURE. 
 */
int COO_lambdaProduct(coordinate_t* coo_ptr, float factor, int type);

/**
 * @brief Return a coordinate made by calculating `a * v1 + b * v2` where a,b are numbers and v1,v2 vectors without calloc.
 * 
 * @param coo1_ptr The first coordinate (v1)
 * @param factor1 The first coefficient (a)
 * @param coo2_ptr The second coordinate (v2)
 * @param factor2 The second coefficient (b)
 * @param ret_ptr The variable in which the result is stored.
 * @return SUCCESS or FAILURE.
 */
int COO_linearTransformation(coordinate_t* coo1_ptr, float factor1, coordinate_t* coo2_ptr, float factor2, coordinate_t* ret_ptr);

/**
 * @brief Side use case of COO_linearTransformation with `a = -1` and `b = 1`.
 * 
 * @param p1_ptr The first point (v1)
 * @param p2_ptr The second point (v2)
 * @param ret_ptr The variable in which the result is stored.
 * @return SUCCESS or FAILURE.
 */
int COO_vectorizePoints(point_t* p1_ptr, point_t* p2_ptr, point_t* ret_ptr);

/**
 * @brief The scalar product between v1 and v2 where v1,v2 are vectors.
 * 
 * @param coo1_ptr The first vector.
 * @param coo2_ptr The second vector.
 * @return The value of `< v1 . v2 >`.
 */
float COO_scalarProduct(coordinate_t* coo1_ptr, coordinate_t* coo2_ptr);

int COO_crossProduct(vector_t* vector1_ptr, vector_t* vector2_ptr, vector_t* ret_ptr);

/**
 * @brief Given a 3x3 matrix M represented by a 1x9 vector, return `M*V` where V is a vector without calloc. 
 * 
 * @param matrix The matrix
 * @param vector_ptr The vector
 * @param ret_ptr The variable in which the result is stored.
 * @return The newly allocated vector resulting of the product.
 */
int COO_matrixVectorProduct(float matrix[9], vector_t* vector_ptr, vector_t* ret_ptr);

/**
 * @brief Given a vector, apply the rotation matrix using 3 axes of rotation.
 * 
 * @param vector_ptr The vector that will be modified used.
 * @param theta Angle around Ox. In °.
 * @param phi Angle around Oy. In °.
 * @param psi Angle around Oz. In °.
 */
void COO_rotationVectorProduct(vector_t* vector_ptr, float theta, float phi, float psi);

#endif /* COORDINATES_H */