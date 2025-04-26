//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h> // for memcpy
#include <math.h> // for sqrt
#include "objects.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Meshes Functions
//-----------------------------------------------------------------------------------------------------------------------
int OBJ_readObjFile(char* filePath_ptr, int mesh_id, mesh_t* ret_ptr){
    FILE *file_ptr;
    char buffer[256];
    file_ptr = fopen(filePath_ptr, "r");
    if(!file_ptr){
        return EXIT_FAILURE;
    }
    ret_ptr->verticesCount = 0;
    ret_ptr->trianglesCount = 0;
    ret_ptr->normalsCount = 0;
    ret_ptr->id = mesh_id;
    //-------------------------- get different counts
    while (fgets(buffer, sizeof(buffer), file_ptr)) {
        if(buffer[0] == 'v' && buffer[1] == ' '){
            ret_ptr->verticesCount++;
        }
        if(buffer[0] == 'v' && buffer[1] == 'n'){
            ret_ptr->normalsCount++;
        }
        if(buffer[0] == 'f' && buffer[1] == ' '){
            ret_ptr->trianglesCount++;
        }
    }
    if(ret_ptr->normalsCount == 0){ // no vn in the file then normal will be build
        ret_ptr->normalsCount = ret_ptr->trianglesCount;
    }
    //-------------------------- use counts to allocate
    ret_ptr->vertices_ptr = calloc(ret_ptr->verticesCount, sizeof(point_t));
    if(!ret_ptr->vertices_ptr){
        return EXIT_FAILURE;
    }
    ret_ptr->indicesVertices_ptr = calloc(ret_ptr->trianglesCount * 3, sizeof(int)); // 3 indices per triangle
    if(!ret_ptr->indicesVertices_ptr){
        return EXIT_FAILURE;
    }
    ret_ptr->normals_ptr = calloc(ret_ptr->normalsCount, sizeof(vector_t));
    if(!ret_ptr->normals_ptr){
        return EXIT_FAILURE;
    }
    ret_ptr->indicesNormals_ptr = calloc(ret_ptr->trianglesCount * 3, sizeof(int));
    if(!ret_ptr->indicesNormals_ptr){
        return EXIT_FAILURE;
    }
    int currentVerticesCount = 0;
    int currentNormalsCount = 0;
    int currentTrianglesCount = 0;
    //-------------------------- get data
    rewind(file_ptr);
    while (fgets(buffer, sizeof(buffer), file_ptr)) {
        if(buffer[0] == 'v' && buffer[1] == ' '){
            float x,y,z;
            if (sscanf(buffer, "v %f %f %f", &x, &y, &z) == 3) {
                ret_ptr->vertices_ptr[currentVerticesCount].x = x;
                ret_ptr->vertices_ptr[currentVerticesCount].y = y;
                ret_ptr->vertices_ptr[currentVerticesCount].z = z;
                currentVerticesCount++;
            }
        }
        if(buffer[0] == 'v' && buffer[1] == 'n'){
            float x,y,z;
            if (sscanf(buffer, "vn %f %f %f", &x, &y, &z) == 3) {
                ret_ptr->normals_ptr[currentNormalsCount].x = x;
                ret_ptr->normals_ptr[currentNormalsCount].y = y;
                ret_ptr->normals_ptr[currentNormalsCount].z = z;
                currentNormalsCount++;
            }            
        }
        if(buffer[0] == 'f' && buffer[1] == ' '){
            int v1, v2, v3;
            int n1, n2, n3;
            if (sscanf(buffer, "f %d/%*d/%d %d/%*d/%d %d/%*d/%d",
                       &v1, &n1, &v2, &n2, &v3, &n3) == 6) {
                ret_ptr->indicesVertices_ptr[3 * currentTrianglesCount] = v1 - 1;
                ret_ptr->indicesVertices_ptr[3 * currentTrianglesCount + 1] = v2 - 1;
                ret_ptr->indicesVertices_ptr[3 * currentTrianglesCount + 2] = v3 - 1;
                ret_ptr->indicesNormals_ptr[3 * currentTrianglesCount] = n1 - 1;
                ret_ptr->indicesNormals_ptr[3 * currentTrianglesCount + 1] = n1 - 1;
                ret_ptr->indicesNormals_ptr[3 * currentTrianglesCount + 2] = n1 - 1;
                currentTrianglesCount++;
            }
            if (sscanf(buffer, "f %d//%d %d//%d %d//%d",
                       &v1, &n1, &v2, &n2, &v3, &n3) == 6) {
                ret_ptr->indicesVertices_ptr[3 * currentTrianglesCount] = v1 - 1;
                ret_ptr->indicesVertices_ptr[3 * currentTrianglesCount + 1] = v2 - 1;
                ret_ptr->indicesVertices_ptr[3 * currentTrianglesCount + 2] = v3 - 1;
                ret_ptr->indicesNormals_ptr[3 * currentTrianglesCount] = n1 - 1;
                ret_ptr->indicesNormals_ptr[3 * currentTrianglesCount + 1] = n1 - 1;
                ret_ptr->indicesNormals_ptr[3 * currentTrianglesCount + 2] = n1 - 1;
                currentTrianglesCount++;
            }
            if(sscanf(buffer, "f %d %d %d",
                &v1, &v2, &v3) == 3) {
                ret_ptr->indicesVertices_ptr[3 * currentTrianglesCount] = v1 - 1;
                ret_ptr->indicesVertices_ptr[3 * currentTrianglesCount + 1] = v2 - 1;
                ret_ptr->indicesVertices_ptr[3 * currentTrianglesCount + 2] = v3 - 1;
                currentTrianglesCount++;
            }   
        }
    }
    //-------------------------- close file
    fclose(file_ptr);
    if(currentNormalsCount != 0){ // vn in file means we can return mesh 
        return EXIT_SUCCESS;
    }
    printf("Building..\n");
    //-------------------------- build normals
    for(int i = 0; i < ret_ptr->trianglesCount; i++){
        vector_t u = {};
        vector_t v = {};
        // COO_vectorizePoints(&ret_ptr->vertices_ptr[ret_ptr->indicesVertices_ptr[3 * i]], 
        //     &ret_ptr->vertices_ptr[ret_ptr->indicesVertices_ptr[3 * i + 1]], &u);
        // COO_vectorizePoints(&ret_ptr->vertices_ptr[ret_ptr->indicesVertices_ptr[3 * i]], 
        //     &ret_ptr->vertices_ptr[ret_ptr->indicesVertices_ptr[3 * i + 2]], &v);
            COO_vectorizePoints(&ret_ptr->vertices_ptr[ret_ptr->indicesVertices_ptr[3 * i + 1]], 
                &ret_ptr->vertices_ptr[ret_ptr->indicesVertices_ptr[3 * i]], &u);
            COO_vectorizePoints(&ret_ptr->vertices_ptr[ret_ptr->indicesVertices_ptr[3 * i + 2]], 
                &ret_ptr->vertices_ptr[ret_ptr->indicesVertices_ptr[3 * i]], &v);
        COO_crossProduct(&u, &v, &ret_ptr->normals_ptr[i]);
        float normalLength = sqrt(COO_scalarProduct(&ret_ptr->normals_ptr[i],&ret_ptr->normals_ptr[i]));
        COO_lambdaProduct(&ret_ptr->normals_ptr[i], normalLength, FT_DIV);
        ret_ptr->indicesNormals_ptr[i] = i;
    }
    return EXIT_SUCCESS;
}