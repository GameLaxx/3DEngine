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

int projectPoint(point_t* point_ptr, point_t* ret_ptr){
    if(!ret_ptr || !point_ptr){
        return EXIT_FAILURE;
    }
    ret_ptr->x = (point_ptr->x * g_context.viewportDistance) / point_ptr->z * (g_windowWidth / g_context.viewportWidth);
    ret_ptr->y = (point_ptr->y * g_context.viewportDistance) / point_ptr->z * (g_windowHeight / g_context.viewportHeight);
    ret_ptr->z = g_context.viewportDistance;
    return EXIT_SUCCESS;
}

int fillTriangle(triangle_t* triangle_ptr){
    point_t p1, p2, p3 = {};
    if(projectPoint(&triangle_ptr->p1, &p1) == EXIT_FAILURE || 
       projectPoint(&triangle_ptr->p2, &p2) == EXIT_FAILURE ||
       projectPoint(&triangle_ptr->p3, &p3) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    int min_x = fminf(fminf(p1.x, p2.x), p3.x);
    int max_x = fmaxf(fmaxf(p1.x, p2.x), p3.x);
    int min_y = fminf(fminf(p1.y, p2.y), p3.y);
    int max_y = fmaxf(fmaxf(p1.y, p2.y), p3.y);
    // TODO : manque le test pour savoir si l'air est nulle
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            float w0 = (x - p2.x) * (p3.y - p2.y) - (p3.x - p2.x) * (y - p2.y);
            float w1 = (x - p3.x) * (p1.y - p3.y) - (p1.x - p3.x) * (y - p3.y);
            float w2 = (x - p1.x) * (p2.y - p1.y) - (p2.x - p1.x) * (y - p1.y);

            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                if (x >= -g_xShift && y >= -g_yShift && x < g_windowWidth - g_xShift && y < g_windowHeight - g_yShift) {
                    DRAW_pixel(x, y, &triangle_ptr->color);
                }
            }
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
    return EXIT_SUCCESS;
}

int RR_clearScene(){
    for(int i = 0; i < g_context.meshesCount; i++){
        free(g_context.meshes[i].vertices);
        free(g_context.meshes[i].indices);
    }
    return EXIT_SUCCESS;
}

int RR_drawScene(){
    for(int o = 0; o < g_context.objectsCount; o++){
        rgba_t* color_ptr = (rgba_t*)g_context.objects[o].material_ptr;
        for(int t = 0; t < g_context.objects[o].mesh->trianglesCount * 3; t += 3){
            point_t p1 = g_context.objects[o].mesh->vertices[g_context.objects[o].mesh->indices[t]];
            point_t p2 = g_context.objects[o].mesh->vertices[g_context.objects[o].mesh->indices[t + 1]];
            point_t p3 = g_context.objects[o].mesh->vertices[g_context.objects[o].mesh->indices[t + 2]];
            scalePoint(&p1, g_context.objects[o].scale);
            scalePoint(&p2, g_context.objects[o].scale);
            scalePoint(&p3, g_context.objects[o].scale);
            translatePoint(&p1, &g_context.objects[o].origin);
            translatePoint(&p2, &g_context.objects[o].origin);
            translatePoint(&p3, &g_context.objects[o].origin);
            triangle_t triangle = {
                .p1 = p1,
                .p2 = p2,
                .p3 = p3,
                .color = *color_ptr
            };
            fillTriangle(&triangle);
        }
    }
    return EXIT_SUCCESS;
}