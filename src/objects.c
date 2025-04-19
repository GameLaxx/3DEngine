//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h> // for memcpy
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
int OBJ_createCubeMesh(mesh_t* ret_ptr){
    ret_ptr->id = 0;
    ret_ptr->verticesCount = 8;
    ret_ptr->trianglesCount = 12;
    ret_ptr->vertices = calloc(ret_ptr->verticesCount, sizeof(point_t));
    ret_ptr->normalVertices_ptr = NULL;
    ret_ptr->indices = calloc(ret_ptr->trianglesCount * 3, sizeof(int)); // 3 indices per triangle
    ret_ptr->normalTriangles_ptr = calloc(ret_ptr->trianglesCount, sizeof(vector_t));
    point_t vertices[] = {
        {0,0,0},
        {1,0,0},
        {0,1,0},
        {0,0,1},
        {1,1,0},
        {1,0,1},
        {0,1,1},
        {1,1,1},
    };
    int indices[] = {
        1, 7, 5,
        1, 4, 7,
        1, 0, 2,
        1, 2, 4,
        1, 0, 3,
        1, 3, 5,
        6, 2, 4,
        6, 4, 7,
        6, 0, 2,
        6, 3, 0,
        6, 3, 5,
        6, 7, 5
    };
    vector_t normalTriangles[] = {
        {1,0,0},
        {1,0,0},
        {0,0,-1},
        {0,0,-1},
        {0,-1,0},
        {0,-1,0},

        {0,1,0},
        {0,1,0},
        {-1,0,0},
        {-1,0,0},
        {0,0,1},
        {0,0,1}
    };
    memcpy(ret_ptr->vertices, vertices, ret_ptr->verticesCount * sizeof(point_t));
    memcpy(ret_ptr->indices, indices, ret_ptr->trianglesCount * 3 * sizeof(int));
    memcpy(ret_ptr->normalTriangles_ptr, normalTriangles, ret_ptr->trianglesCount * sizeof(vector_t));
    return EXIT_SUCCESS;
}

int OBJ_readObjFile(char* filePath_ptr, mesh_t* ret_ptr){
    FILE *file_ptr;
    char buffer[256];
    file_ptr = fopen(filePath_ptr, "r");
    ret_ptr->verticesCount = 0;
    ret_ptr->trianglesCount = 0;
    ret_ptr->id = 1;
    //-------------------------- get different counts
    while (fgets(buffer, sizeof(buffer), file_ptr)) {
        if(buffer[0] == 'v' && buffer[1] == ' '){
            ret_ptr->verticesCount++;
        }
        if(buffer[0] == 'f' && buffer[1] == ' '){
            ret_ptr->trianglesCount++;
        }
    }
    //-------------------------- use counts to allocate
    ret_ptr->vertices = calloc(ret_ptr->verticesCount, sizeof(point_t));
    ret_ptr->normalVertices_ptr = NULL;
    ret_ptr->indices = calloc(ret_ptr->trianglesCount * 3, sizeof(int)); // 3 indices per triangle
    ret_ptr->normalTriangles_ptr = NULL;
    int currentVerticesCount = 0;
    int currentTriangleCount = 0;
    //-------------------------- get data
    rewind(file_ptr);
    while (fgets(buffer, sizeof(buffer), file_ptr)) {
        if(buffer[0] == 'v' && buffer[1] == ' '){
            float x,y,z;
            if (sscanf(buffer, "v %f %f %f", &x, &y, &z) == 3) {
                ret_ptr->vertices[currentVerticesCount].x = x;
                ret_ptr->vertices[currentVerticesCount].y = y;
                ret_ptr->vertices[currentVerticesCount].z = z;
                currentVerticesCount++;
            }
        }
        if(buffer[0] == 'f' && buffer[1] == ' '){
            int v1, v2, v3;
            if (sscanf(buffer, "f %d%*[^ ] %d%*[^ ] %d%*[^ ]", &v1, &v2, &v3) == 3) {
                ret_ptr->indices[currentTriangleCount++] = v1 - 1;
                ret_ptr->indices[currentTriangleCount++] = v2 - 1;
                ret_ptr->indices[currentTriangleCount++] = v3 - 1;
            }            
        }
    }
    //-------------------------- close file
    fclose(file_ptr);
    return EXIT_SUCCESS;
}