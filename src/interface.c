//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "interface.h"
#include "draw.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
rgba_t interfaceColor = {36,36,36,255};
rgba_t meshBoxColor = {88, 88, 88, 255};
rgba_t addBoxColor = {.green = 51};
interface_t g_interface;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
int getAllMeshes(){
    const char *dossier = "./ressources/";
    struct dirent *ent;
    DIR *dir = opendir(dossier);

    if (dir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 && g_interface.numberBoxes < MAX_BOXES) {
            g_interface.boxes_ptr[g_interface.numberBoxes].x = -g_xShift;
            g_interface.boxes_ptr[g_interface.numberBoxes].y = g_windowHeight - g_yShift - (1 + g_interface.numberBoxes) * g_windowHeight / 5;
            g_interface.boxes_ptr[g_interface.numberBoxes].width = (g_windowWidth - g_pixelWidth) / 2;
            g_interface.boxes_ptr[g_interface.numberBoxes].height = g_windowHeight / 5;
            g_interface.boxes_ptr[g_interface.numberBoxes].padding = 10;
            g_interface.boxes_ptr[g_interface.numberBoxes].margin = 10;
            g_interface.boxes_ptr[g_interface.numberBoxes].name = calloc(strlen(ent->d_name), sizeof(char));
            strcpy(g_interface.boxes_ptr[g_interface.numberBoxes].name, ent->d_name);
            g_interface.boxes_ptr[g_interface.numberBoxes].name[0] = toupper(g_interface.boxes_ptr[g_interface.numberBoxes].name[0]);
            g_interface.boxes_ptr[g_interface.numberBoxes].name[strlen(ent->d_name) - 4] = '\0';
            g_interface.numberBoxes += 1;
        }
    }

    closedir(dir);
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int IF_initInterface(){
    g_interface.boxes_ptr = calloc(MAX_BOXES, sizeof(meshBox_t));
    g_interface.numberBoxes = 0;
    getAllMeshes();
    return EXIT_SUCCESS;
}

int IF_cleanInterface(){
    free(g_interface.boxes_ptr);
    g_interface.numberBoxes = 0;
    return EXIT_SUCCESS;
}

int IF_updateInterface(){
    for(int i = 0; i < g_interface.numberBoxes; i++){
        g_interface.boxes_ptr[i].x = -g_xShift;
        g_interface.boxes_ptr[i].y = g_windowHeight - g_yShift - (1 + i) * g_windowHeight / 5;
        g_interface.boxes_ptr[i].width = (g_windowWidth - g_pixelWidth) / 2;
        g_interface.boxes_ptr[i].height = g_windowHeight / 5;
    }
    return EXIT_SUCCESS;
}

int IF_drawInterface(){
    // draw sides
    DRAW_rectangleFill(-g_xShift, -g_yShift, (g_windowWidth - g_pixelWidth) / 2, g_windowHeight, &interfaceColor);
    DRAW_rectangleFill((g_windowWidth + g_pixelWidth) / 2 - g_xShift, -g_yShift, (g_windowWidth - g_pixelWidth) / 2,g_windowHeight, &interfaceColor);
    IF_drawMeshBoxes();
    return EXIT_SUCCESS;
}

int IF_drawMeshBoxes(){
    for(int i = 0; i < g_interface.numberBoxes; i++){
        DRAW_rectangleFill(g_interface.boxes_ptr[i].x + g_interface.boxes_ptr[i].margin, g_interface.boxes_ptr[i].y + g_interface.boxes_ptr[i].margin, 
            g_interface.boxes_ptr[i].width - 2 * g_interface.boxes_ptr[i].margin, g_interface.boxes_ptr[i].height - 2 * g_interface.boxes_ptr[i].margin, &meshBoxColor);
        DRAW_rectangleFill(
            g_interface.boxes_ptr[i].x + g_interface.boxes_ptr[i].width - g_interface.boxes_ptr[i].margin - g_interface.boxes_ptr[i].padding - g_interface.boxes_ptr[i].height / 10,
            g_interface.boxes_ptr[i].y + g_interface.boxes_ptr[i].margin + g_interface.boxes_ptr[i].padding, 
            g_interface.boxes_ptr[i].height / 10, g_interface.boxes_ptr[i].height / 10, &addBoxColor);
    }
    return EXIT_SUCCESS;
}

int IF_clickMeshBox(int xMouse, int yMouse){
    xMouse = xMouse - g_xShift;
    yMouse = g_windowHeight - yMouse - g_yShift;
    for(int i = 0; i < g_interface.numberBoxes; i++){
        int checkBoxXmin = g_interface.boxes_ptr[i].x + g_interface.boxes_ptr[i].width - g_interface.boxes_ptr[i].margin - g_interface.boxes_ptr[i].padding - g_interface.boxes_ptr[i].height / 10;
        int checkBoxXmax = g_interface.boxes_ptr[i].x + g_interface.boxes_ptr[i].width - g_interface.boxes_ptr[i].margin - g_interface.boxes_ptr[i].padding;
        int checkBoxYmin = g_interface.boxes_ptr[i].y + g_interface.boxes_ptr[i].margin + g_interface.boxes_ptr[i].padding;
        int checkBoxYmax = g_interface.boxes_ptr[i].y + g_interface.boxes_ptr[i].margin + g_interface.boxes_ptr[i].padding + g_interface.boxes_ptr[i].height / 10;
        if(checkBoxXmin < xMouse && xMouse < checkBoxXmax 
            && checkBoxYmin < yMouse && yMouse < checkBoxYmax){
            printf("Clicked on %s\n", g_interface.boxes_ptr[i].name);
            return EXIT_SUCCESS;
        }
    }
    return EXIT_SUCCESS;
}

int IF_hoverMeshBox(int xMouse, int yMouse){
    xMouse = xMouse - g_xShift;
    yMouse = g_windowHeight - yMouse - g_yShift;
    for(int i = 0; i < g_interface.numberBoxes; i++){
        int checkBoxXmin = g_interface.boxes_ptr[i].x + g_interface.boxes_ptr[i].width - g_interface.boxes_ptr[i].margin - g_interface.boxes_ptr[i].padding - g_interface.boxes_ptr[i].height / 10;
        int checkBoxXmax = g_interface.boxes_ptr[i].x + g_interface.boxes_ptr[i].width - g_interface.boxes_ptr[i].margin - g_interface.boxes_ptr[i].padding;
        int checkBoxYmin = g_interface.boxes_ptr[i].y + g_interface.boxes_ptr[i].margin + g_interface.boxes_ptr[i].padding;
        int checkBoxYmax = g_interface.boxes_ptr[i].y + g_interface.boxes_ptr[i].margin + g_interface.boxes_ptr[i].padding + g_interface.boxes_ptr[i].height / 10;
        if(checkBoxXmin < xMouse && xMouse < checkBoxXmax 
            && checkBoxYmin < yMouse && yMouse < checkBoxYmax){
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}