//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "rasterization.h"
#include "draw.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
sceneContext_t g_context;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
int scalePoint(point_t* point_ptr, float scale_ptr[3]){
    point_ptr->x *= scale_ptr[0];
    point_ptr->y *= scale_ptr[1];
    point_ptr->z *= scale_ptr[2];
    return EXIT_SUCCESS;
}

int translatePoint(point_t* point_ptr, vector_t* vector_ptr){
    point_ptr->x += vector_ptr->x;
    point_ptr->y += vector_ptr->y;
    point_ptr->z += vector_ptr->z;
    return EXIT_SUCCESS;
}

int backFaceCulling(vector_t* normal_ptr, vector_t* ray_ptr){
    if(COO_scalarProduct(normal_ptr, ray_ptr) <= 0){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int point3DtoPixel(point_t* point_ptr, point_t* ret_ptr){
    if(!ret_ptr || !point_ptr){
        return EXIT_FAILURE;
    }
    ret_ptr->x = (point_ptr->x * g_context.viewportDistance) / point_ptr->z * (g_windowWidth / g_context.viewportWidth);
    ret_ptr->y = (point_ptr->y * g_context.viewportDistance) / point_ptr->z * (g_windowHeight / g_context.viewportHeight);
    ret_ptr->z = g_context.viewportDistance;
    return EXIT_SUCCESS;
}

float triangleArea(point_t* p1_ptr, point_t* p2_ptr, point_t* p3_ptr){
    return (p2_ptr->x - p1_ptr->x) * (p3_ptr->y - p1_ptr->y) - (p2_ptr->y - p1_ptr->y) * (p3_ptr->x - p1_ptr->x);
}

int fillTriangle(triangle_t* triangle_ptr){
    point_t p1, p2, p3 = {};
    if(point3DtoPixel(&triangle_ptr->p1, &p1) == EXIT_FAILURE || 
       point3DtoPixel(&triangle_ptr->p2, &p2) == EXIT_FAILURE ||
       point3DtoPixel(&triangle_ptr->p3, &p3) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    float totalArea = triangleArea(&p1, &p2, &p3);
    if(totalArea == 0){
        return EXIT_SUCCESS;
    }
    int minXs = fminf(fminf(p1.x, p2.x), p3.x);
    int maxXs = fmaxf(fmaxf(p1.x, p2.x), p3.x);
    int minYs = fminf(fminf(p1.y, p2.y), p3.y);
    int maxYs = fmaxf(fmaxf(p1.y, p2.y), p3.y);
    float areaWithout1 = 0;
    float areaWithout2 = 0;
    float areaWithout3 = 0;
    float interpolatedZ = 0;
    for (int y = minYs; y <= maxYs; y++) {
    for (int x = minXs; x <= maxXs; x++) {
        if (x < -g_xShift || y < -g_yShift || x >= g_windowWidth - g_xShift || y >= g_windowHeight - g_yShift) {
            continue;
        }
        point_t currentPoint = {.x = x + 0.5, .y = y + 0.5, .z = 0};
        areaWithout1 = triangleArea(&p2, &currentPoint, &p3);
        areaWithout2 = triangleArea(&p3, &currentPoint, &p1);
        areaWithout3 = triangleArea(&p1, &currentPoint, &p2);
        if (((areaWithout1 < 0 || areaWithout2 < 0 || areaWithout3 < 0) && (areaWithout1 > 0 || areaWithout2 > 0 || areaWithout3 > 0))){
            // pixel outside of the triangle
            continue;
        }
        interpolatedZ = (areaWithout1/totalArea) * triangle_ptr->p1.z
                        + (areaWithout2/totalArea) * triangle_ptr->p2.z
                        + (areaWithout3/totalArea) * triangle_ptr->p3.z + 0.1; // zBuffer is calloc so if == 0 then never assigned hence + 0.1
        // add shift to go in zbuffer because x and y can be < 0
        if(g_context.zBuffer[(int) ((y + g_yShift) + (x + g_xShift) * g_windowHeight)] != 0 
        && g_context.zBuffer[(int) ((y + g_yShift) + (x + g_xShift) * g_windowHeight)] < 1 / interpolatedZ){
            continue;
        }
        g_context.zBuffer[(int) ((y + g_yShift) + (x + g_xShift) * g_windowHeight)] = 1 / interpolatedZ;
        DRAW_pixel(x, y, &triangle_ptr->color);
    }
    }
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int RR_addMesh(mesh_t* mesh_ptr){
    if(g_context.meshesCount == MAX_MESHES){
        return EXIT_FAILURE;
    }
    if(mesh_ptr->id >= MAX_MESH_IDS){
        return EXIT_FAILURE;
    }
    g_context.meshesId[mesh_ptr->id] = g_context.meshesCount + 1; // calloc set at 0, so if 0 then nothing has been added ==> easier to compare hence + 1
    g_context.meshes[g_context.meshesCount] = *mesh_ptr;
    g_context.meshesCount++;
    return EXIT_SUCCESS;
}

int RR_addObject(object_t* object_ptr){
    if(g_context.objectsCount == MAX_OBJECTS){
        return EXIT_FAILURE;
    }
    if(object_ptr->meshId < 0 || object_ptr->meshId >= MAX_MESH_IDS){
        return EXIT_FAILURE;
    }
    if(g_context.meshesId[object_ptr->meshId] == 0){
        return EXIT_FAILURE;
    }
    g_context.objects[g_context.objectsCount] = *object_ptr;
    g_context.objects[g_context.objectsCount].mesh = &g_context.meshes[g_context.meshesId[object_ptr->meshId] - 1]; // because added with + 1
    g_context.objectsCount++;
    return EXIT_SUCCESS;
}

int RR_initScene(point_t* origin, int vW, int vH, int vD){
    g_context.origin = *origin;
    g_context.viewportWidth = vW;
    g_context.viewportHeight = vH;
    g_context.viewportDistance = vD;
    g_context.meshesCount = 0;
    g_context.objectsCount = 0;
    g_context.zBuffer = NULL;
    return EXIT_SUCCESS;
}

int RR_clearScene(){
    for(int i = 0; i < g_context.meshesCount; i++){
        if(g_context.meshes[i].vertices){
            free(g_context.meshes[i].vertices);
        }
        if(g_context.meshes[i].indices){
            free(g_context.meshes[i].indices);
        }
        if(g_context.meshes[i].normalTriangles_ptr){
            free(g_context.meshes[i].normalTriangles_ptr);
        }
        if(g_context.meshes[i].normalVertices_ptr){
            free(g_context.meshes[i].normalVertices_ptr);
        }
    }
    return EXIT_SUCCESS;
}

int RR_drawScene(){
    g_context.zBuffer = calloc(g_windowHeight * g_windowWidth, sizeof(float));
    for(int obj = 0; obj < g_context.objectsCount; obj++){
        rgba_t* color_ptr = (rgba_t*)g_context.objects[obj].material_ptr;
        for(int t = 0; t < g_context.objects[obj].mesh->trianglesCount; t++){
            point_t p1 = g_context.objects[obj].mesh->vertices[g_context.objects[obj].mesh->indices[3 * t]];
            vector_t ray = {};
            point_t p2 = g_context.objects[obj].mesh->vertices[g_context.objects[obj].mesh->indices[3 * t + 1]];
            point_t p3 = g_context.objects[obj].mesh->vertices[g_context.objects[obj].mesh->indices[3 * t + 2]];
            scalePoint(&p1, g_context.objects[obj].scale);
            scalePoint(&p2, g_context.objects[obj].scale);
            scalePoint(&p3, g_context.objects[obj].scale);
            translatePoint(&p1, &g_context.objects[obj].origin);
            translatePoint(&p2, &g_context.objects[obj].origin);
            translatePoint(&p3, &g_context.objects[obj].origin);
            COO_vectorizePoints(&p1, &g_context.origin, &ray);
            if(backFaceCulling(&g_context.objects[obj].mesh->normalTriangles_ptr[t], &ray) == EXIT_FAILURE){
                continue; // is facing backward
            }
            triangle_t triangle = {
                .p1 = p1,
                .p2 = p2,
                .p3 = p3,
                .color = color_ptr[t]
            };
            fillTriangle(&triangle);
        }
    }
    free(g_context.zBuffer);
    g_context.zBuffer = NULL;
    return EXIT_SUCCESS;
}