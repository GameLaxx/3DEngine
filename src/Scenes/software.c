//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <dirent.h>
#include "software.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
sceneContext_t g_sceneContext;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
int getAllMeshVariables(){
    const char *dossier = "./meshes/";
    struct dirent *ent;
    DIR *dir = opendir(dossier);

    if (dir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 && g_sceneContext.meshesCount < MAX_MESHES) {
            char* filePath_ptr = calloc(strlen(dossier) + strlen(ent->d_name), sizeof(char));
            strcpy(filePath_ptr, dossier);
            strcat(filePath_ptr, ent->d_name);
            mesh_t currentMesh = {};
            OBJ_readObjFile(filePath_ptr, g_sceneContext.meshesCount, &currentMesh);
            SW_addMesh(&currentMesh);
        }
    }

    closedir(dir);
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int SW_initScene(point_t* origin, int vW, int vH, int vD){
    g_sceneContext.origin = *origin;
    g_sceneContext.viewportWidth = vW;
    g_sceneContext.viewportHeight = vH;
    g_sceneContext.viewportDistance = vD;
    g_sceneContext.renderDistance = 20;
    g_sceneContext.meshesCount = 0;
    g_sceneContext.objectsCount = 0;
    g_sceneContext.zBuffer = NULL;
    getAllMeshVariables();
    return EXIT_SUCCESS;
}