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
rgba_t interfaceColor = {22,22,22,255};
rgba_t meshBoxColor = {48, 48, 48, 255};
rgba_t addBoxColor = {94, 176, 162, 255};
rgba_t textColor = {212,212,212,255};
interface_t g_interface;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
int getAllMeshBoxes(){
    const char *dossier = "./meshes/";
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
    getAllMeshBoxes();
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

int IF_drawMeshBoxes(){
    for(int i = 0; i < g_interface.numberBoxes; i++){
        DRAW_rectangleFill(g_interface.boxes_ptr[i].x + g_interface.boxes_ptr[i].margin, g_interface.boxes_ptr[i].y + g_interface.boxes_ptr[i].margin, 
            g_interface.boxes_ptr[i].width - 2 * g_interface.boxes_ptr[i].margin, g_interface.boxes_ptr[i].height - 2 * g_interface.boxes_ptr[i].margin, &meshBoxColor);
        DRAW_rectangleFill(
            g_interface.boxes_ptr[i].x + g_interface.boxes_ptr[i].width - g_interface.boxes_ptr[i].margin - g_interface.boxes_ptr[i].padding - g_interface.boxes_ptr[i].height / 10,
            g_interface.boxes_ptr[i].y + g_interface.boxes_ptr[i].margin + g_interface.boxes_ptr[i].padding, 
            g_interface.boxes_ptr[i].height / 10, g_interface.boxes_ptr[i].height / 10, &addBoxColor);
        DRAW_texte(
            g_interface.boxes_ptr[i].x + g_interface.boxes_ptr[i].margin + g_interface.boxes_ptr->padding,
            g_interface.boxes_ptr[i].y + g_interface.boxes_ptr[i].height - g_interface.boxes_ptr[i].margin - g_interface.boxes_ptr[i].padding,
            g_interface.boxes_ptr[i].name, &textColor
        );
    }
    return EXIT_SUCCESS;
}

int IF_renderInterfaceLeft(){
    DRAW_rectangleFill(-g_xShift, -g_yShift, (g_windowWidth - g_pixelWidth) / 2, g_windowHeight, &interfaceColor);
    IF_drawMeshBoxes();
    return EXIT_SUCCESS;
}

int IF_renderInterfaceRight(){
    DRAW_rectangleFill((g_windowWidth + g_pixelWidth) / 2 - g_xShift, -g_yShift, (g_windowWidth - g_pixelWidth) / 2,g_windowHeight, &interfaceColor);
    DRAW_texte((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 10, g_windowHeight * 19 / 20 - g_yShift, "Camera", &textColor);
    DRAW_line((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 10, g_windowHeight * 9 / 10 - g_yShift, g_windowWidth - g_xShift - 10, g_windowHeight * 9 / 10 - g_yShift, &textColor);
    return EXIT_SUCCESS;
}

int IF_renderInterface(){
    IF_renderInterfaceLeft();
    IF_renderInterfaceRight();
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
            return i;
        }
    }
    return -1;
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

int IF_renderInterfaceObject(object_t* object_ptr){
    DRAW_texte((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 10, g_windowHeight * 17 / 20 - g_yShift, g_interface.boxes_ptr[object_ptr->meshId].name, &textColor);
    DRAW_texte((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 10, g_windowHeight * 16 / 20 - g_yShift, "Position", &textColor);
    return EXIT_SUCCESS;
}