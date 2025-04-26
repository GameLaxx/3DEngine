//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "software.h"
#include "RenderTools/rasterization.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
sceneContext_t g_sceneContext;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int SW_initScene(point_t* origin_ptr, int viewportWidth, int viewportHeight, int viewportDistance, int renderDistance){
    g_sceneContext.context_ptr = calloc(1, sizeof(renderContext_t));
    g_sceneContext.cameraPos_ptr = calloc(1, sizeof(point_t));
    g_sceneContext.indexBuffer_ptr = calloc(g_pixelHeight * g_pixelWidth, sizeof(int));
    for(int i = 0; i < g_pixelHeight * g_pixelWidth; i++){
        g_sceneContext.indexBuffer_ptr[i] = -1;
    }
    RR_initScene(origin_ptr, viewportWidth, viewportHeight, viewportDistance, renderDistance, g_sceneContext.context_ptr);
    return EXIT_SUCCESS;
}

int SW_cleanScene(){
    RR_clearScene(g_sceneContext.context_ptr);
    free(g_sceneContext.context_ptr);
    free(g_sceneContext.cameraPos_ptr);
    return EXIT_SUCCESS;
}