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
int projectPoint(point_t* point_ptr, point_t* ret_ptr){
    if(!ret_ptr || !point_ptr){
        return EXIT_FAILURE;
    }
    ret_ptr->x = (point_ptr->x * g_context.viewportDistance) / (point_ptr->z * g_context.viewportWidth);
    ret_ptr->y = (point_ptr->y * g_context.viewportDistance) / (point_ptr->z * g_context.viewportWidth);
    ret_ptr->z = g_context.viewportDistance;
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int RR_fillTriangle(triangle_t* triangle_ptr){
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
            // if (w0 == 0 && w1 == 0 && w2 == 0) {
                if (x >= -g_xShift && y >= -g_yShift && x < g_windowWidth - g_xShift && y < g_windowHeight - g_yShift) {
                    DRAW_pixel(x, y, &triangle_ptr->color);
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

int RR_initScene(point_t* origin, int vW, int vH, int vD){
    g_context.origin = *origin;
    g_context.viewportWidth = vW;
    g_context.viewportHeight = vH;
    g_context.viewportDistance = vD;
    return 0;
}