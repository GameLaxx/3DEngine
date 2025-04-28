//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
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
rgba_t textBoxColor = {65, 65, 65, 255};
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
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 && g_interface.numberMeshBoxes < MAX_BOXES) {
            g_interface.meshBoxes_ptr[g_interface.numberMeshBoxes].x = -g_xShift;
            g_interface.meshBoxes_ptr[g_interface.numberMeshBoxes].y = g_windowHeight - g_yShift - (1 + g_interface.numberMeshBoxes) * g_windowHeight / 5;
            g_interface.meshBoxes_ptr[g_interface.numberMeshBoxes].width = (g_windowWidth - g_pixelWidth) / 2;
            g_interface.meshBoxes_ptr[g_interface.numberMeshBoxes].height = g_windowHeight / 5;
            g_interface.meshBoxes_ptr[g_interface.numberMeshBoxes].padding = 10;
            g_interface.meshBoxes_ptr[g_interface.numberMeshBoxes].margin = 10;
            g_interface.meshBoxes_ptr[g_interface.numberMeshBoxes].name = calloc(strlen(ent->d_name), sizeof(char));
            strcpy(g_interface.meshBoxes_ptr[g_interface.numberMeshBoxes].name, ent->d_name);
            g_interface.meshBoxes_ptr[g_interface.numberMeshBoxes].name[0] = toupper(g_interface.meshBoxes_ptr[g_interface.numberMeshBoxes].name[0]);
            g_interface.meshBoxes_ptr[g_interface.numberMeshBoxes].name[strlen(ent->d_name) - 4] = '\0';
            g_interface.numberMeshBoxes += 1;
        }
    }

    closedir(dir);
    return EXIT_SUCCESS;
}

int renderTextBox(textBox_t* box_ptr){
    DRAW_textBox(box_ptr->x, box_ptr->y, box_ptr->width, box_ptr->height, box_ptr->content_ptr, &textBoxColor, &textColor);
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int IF_initInterface(){
    g_interface.meshBoxes_ptr = calloc(MAX_BOXES, sizeof(meshBox_t));
    g_interface.numberMeshBoxes = 0;
    g_interface.textBoxes_ptr = NULL;
    getAllMeshBoxes();
    return EXIT_SUCCESS;
}

int IF_cleanInterface(){
    free(g_interface.meshBoxes_ptr);
    g_interface.numberMeshBoxes = 0;
    if(g_interface.textBoxes_ptr){
        free(g_interface.textBoxes_ptr);
        g_interface.textBoxes_ptr = NULL;
        g_interface.numberTextBoxes = 0;
    }
    return EXIT_SUCCESS;
}

int IF_updateInterface(){
    for(int i = 0; i < g_interface.numberMeshBoxes; i++){
        g_interface.meshBoxes_ptr[i].x = -g_xShift;
        g_interface.meshBoxes_ptr[i].y = g_windowHeight - g_yShift - (1 + i) * g_windowHeight / 5;
        g_interface.meshBoxes_ptr[i].width = (g_windowWidth - g_pixelWidth) / 2;
        g_interface.meshBoxes_ptr[i].height = g_windowHeight / 5;
    }
    return EXIT_SUCCESS;
}

int IF_drawMeshBoxes(){
    for(int i = 0; i < g_interface.numberMeshBoxes; i++){
        DRAW_rectangleFill(g_interface.meshBoxes_ptr[i].x + g_interface.meshBoxes_ptr[i].margin, g_interface.meshBoxes_ptr[i].y + g_interface.meshBoxes_ptr[i].margin, 
            g_interface.meshBoxes_ptr[i].width - 2 * g_interface.meshBoxes_ptr[i].margin, g_interface.meshBoxes_ptr[i].height - 2 * g_interface.meshBoxes_ptr[i].margin, &meshBoxColor);
        DRAW_rectangleFill(
            g_interface.meshBoxes_ptr[i].x + g_interface.meshBoxes_ptr[i].width - g_interface.meshBoxes_ptr[i].margin - g_interface.meshBoxes_ptr[i].padding - g_interface.meshBoxes_ptr[i].height / 10,
            g_interface.meshBoxes_ptr[i].y + g_interface.meshBoxes_ptr[i].margin + g_interface.meshBoxes_ptr[i].padding, 
            g_interface.meshBoxes_ptr[i].height / 10, g_interface.meshBoxes_ptr[i].height / 10, &addBoxColor);
        DRAW_text(
            g_interface.meshBoxes_ptr[i].x + g_interface.meshBoxes_ptr[i].margin + g_interface.meshBoxes_ptr->padding,
            g_interface.meshBoxes_ptr[i].y + g_interface.meshBoxes_ptr[i].height - g_interface.meshBoxes_ptr[i].margin - g_interface.meshBoxes_ptr[i].padding,
            g_interface.meshBoxes_ptr[i].name, &textColor
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
    if(g_interface.textBoxes_ptr){
        free(g_interface.textBoxes_ptr);
        g_interface.numberTextBoxes = 0;
        g_interface.textBoxes_ptr = NULL;
    }
    DRAW_rectangleFill((g_windowWidth + g_pixelWidth) / 2 - g_xShift, -g_yShift, (g_windowWidth - g_pixelWidth) / 2,g_windowHeight, &interfaceColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 10, g_windowHeight * 19 / 20 - g_yShift, "Camera", &textColor);
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
    for(int i = 0; i < g_interface.numberMeshBoxes; i++){
        int checkBoxXmin = g_interface.meshBoxes_ptr[i].x + g_interface.meshBoxes_ptr[i].width - g_interface.meshBoxes_ptr[i].margin - g_interface.meshBoxes_ptr[i].padding - g_interface.meshBoxes_ptr[i].height / 10;
        int checkBoxXmax = g_interface.meshBoxes_ptr[i].x + g_interface.meshBoxes_ptr[i].width - g_interface.meshBoxes_ptr[i].margin - g_interface.meshBoxes_ptr[i].padding;
        int checkBoxYmin = g_interface.meshBoxes_ptr[i].y + g_interface.meshBoxes_ptr[i].margin + g_interface.meshBoxes_ptr[i].padding;
        int checkBoxYmax = g_interface.meshBoxes_ptr[i].y + g_interface.meshBoxes_ptr[i].margin + g_interface.meshBoxes_ptr[i].padding + g_interface.meshBoxes_ptr[i].height / 10;
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
    for(int i = 0; i < g_interface.numberMeshBoxes; i++){
        int checkBoxXmin = g_interface.meshBoxes_ptr[i].x + g_interface.meshBoxes_ptr[i].width - g_interface.meshBoxes_ptr[i].margin - g_interface.meshBoxes_ptr[i].padding - g_interface.meshBoxes_ptr[i].height / 10;
        int checkBoxXmax = g_interface.meshBoxes_ptr[i].x + g_interface.meshBoxes_ptr[i].width - g_interface.meshBoxes_ptr[i].margin - g_interface.meshBoxes_ptr[i].padding;
        int checkBoxYmin = g_interface.meshBoxes_ptr[i].y + g_interface.meshBoxes_ptr[i].margin + g_interface.meshBoxes_ptr[i].padding;
        int checkBoxYmax = g_interface.meshBoxes_ptr[i].y + g_interface.meshBoxes_ptr[i].margin + g_interface.meshBoxes_ptr[i].padding + g_interface.meshBoxes_ptr[i].height / 10;
        if(checkBoxXmin < xMouse && xMouse < checkBoxXmax 
            && checkBoxYmin < yMouse && yMouse < checkBoxYmax){
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

int IF_renderInterfaceObject(object_t* object_ptr){
    if(object_ptr == NULL){ // allow to escape current object without clicking on another object
        IF_renderInterfaceRight();
        return EXIT_SUCCESS;
    }
    g_interface.textBoxes_ptr = calloc(9, sizeof(textBox_t));
    g_interface.numberTextBoxes = 1;
    g_interface.textBoxes_ptr[0].x = (g_windowWidth + g_pixelWidth) / 2 - g_xShift + 120;
    g_interface.textBoxes_ptr[0].y = g_windowHeight * 15 / 20 - g_yShift + 1;
    g_interface.textBoxes_ptr[0].width = (g_windowWidth - g_pixelWidth) / 2 - 130;
    g_interface.textBoxes_ptr[0].height = g_windowHeight / 20 - 2;
    g_interface.textBoxes_ptr[0].active = 0;
    sprintf(g_interface.textBoxes_ptr[0].content_ptr, "%.2f", object_ptr->origin.x);
    g_interface.textBoxes_ptr[0].length = strlen(g_interface.textBoxes_ptr[0].content_ptr);
    sprintf(g_interface.textBoxes_ptr[1].content_ptr, "%.2f", object_ptr->origin.y);
    sprintf(g_interface.textBoxes_ptr[2].content_ptr, "%.2f", object_ptr->origin.z);
    char xRotBuffer[40];
    char yRotBuffer[40];
    char zRotBuffer[40];
    sprintf(xRotBuffer, "%.2f", object_ptr->angleRotation[0]);
    sprintf(yRotBuffer, "%.2f", object_ptr->angleRotation[1]);
    sprintf(zRotBuffer, "%.2f", object_ptr->angleRotation[2]);
    char xScaleBuffer[40];
    char yScaleBuffer[40];
    char zScaleBuffer[40];
    sprintf(xScaleBuffer, "%.2f", object_ptr->scale[0]);
    sprintf(yScaleBuffer, "%.2f", object_ptr->scale[1]);
    sprintf(zScaleBuffer, "%.2f", object_ptr->scale[2]);
    DRAW_rectangleFill((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 5, 5 - g_yShift, (g_windowWidth - g_pixelWidth) / 2 - 10, g_windowHeight * 17 / 20 + 5, &meshBoxColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 10, g_windowHeight * 17 / 20 - g_yShift, g_interface.meshBoxes_ptr[object_ptr->meshId].name, &textColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 10, g_windowHeight * 16 / 20 - g_yShift - g_windowHeight / 80, "Position : ", &textColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 100, g_windowHeight * 16 / 20 - g_yShift - g_windowHeight / 80, "X", &textColor);
    renderTextBox(&g_interface.textBoxes_ptr[0]);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 100, g_windowHeight * 15 / 20 - g_yShift - g_windowHeight / 80, "Y", &textColor);
    DRAW_textBox((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 120, g_windowHeight * 14 / 20 - g_yShift + 1,
                 (g_windowWidth - g_pixelWidth) / 2 - 130, g_windowHeight / 20 - 2, g_interface.textBoxes_ptr[1].content_ptr, 
                 &textBoxColor, &textColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 100, g_windowHeight * 14 / 20 - g_yShift - g_windowHeight / 80, "Z", &textColor);
    DRAW_textBox((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 120, g_windowHeight * 13 / 20 - g_yShift + 1,
                 (g_windowWidth - g_pixelWidth) / 2 - 130, g_windowHeight / 20 - 2, g_interface.textBoxes_ptr[2].content_ptr, 
                 &textBoxColor, &textColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 10, g_windowHeight * 13 / 20 - g_yShift - g_windowHeight / 80, "Rotation : ", &textColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 100, g_windowHeight * 13 / 20 - g_yShift - g_windowHeight / 80, "X", &textColor);
    DRAW_text((3 * g_windowWidth + g_pixelWidth) / 4 - g_xShift + 50, g_windowHeight * 13 / 20 - g_yShift, xRotBuffer, &textColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 100, g_windowHeight * 12 / 20 - g_yShift - g_windowHeight / 80, "Y", &textColor);
    DRAW_text((3 * g_windowWidth + g_pixelWidth) / 4 - g_xShift + 50, g_windowHeight * 12 / 20 - g_yShift, yRotBuffer, &textColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 100, g_windowHeight * 11 / 20 - g_yShift - g_windowHeight / 80, "Z", &textColor);
    DRAW_text((3 * g_windowWidth + g_pixelWidth) / 4 - g_xShift + 50, g_windowHeight * 11 / 20 - g_yShift, zRotBuffer, &textColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 10, g_windowHeight * 10 / 20 - g_yShift - g_windowHeight / 80, "Scale : ", &textColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 100, g_windowHeight * 10 / 20 - g_yShift - g_windowHeight / 80, "X", &textColor);
    DRAW_text((3 * g_windowWidth + g_pixelWidth) / 4 - g_xShift + 50, g_windowHeight * 10 / 20 - g_yShift, xScaleBuffer, &textColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 100, g_windowHeight * 9 / 20 - g_yShift - g_windowHeight / 80, "Y", &textColor);
    DRAW_text((3 * g_windowWidth + g_pixelWidth) / 4 - g_xShift + 50, g_windowHeight * 9 / 20 - g_yShift, yScaleBuffer, &textColor);
    DRAW_text((g_windowWidth + g_pixelWidth) / 2 - g_xShift + 100, g_windowHeight * 8 / 20 - g_yShift - g_windowHeight / 80, "Z", &textColor);
    DRAW_text((3 * g_windowWidth + g_pixelWidth) / 4 - g_xShift + 50, g_windowHeight * 8 / 20 - g_yShift, zScaleBuffer, &textColor);
    return EXIT_SUCCESS;
}

int IF_clickTextBox(int xMouse, int yMouse){
    int found = 0;
    xMouse = xMouse - g_xShift;
    yMouse = g_windowHeight - yMouse - g_yShift;
    for(int i = 0; i < g_interface.numberTextBoxes; i++){
        int checkBoxXmin = g_interface.textBoxes_ptr[i].x;
        int checkBoxXmax = g_interface.textBoxes_ptr[i].x + g_interface.textBoxes_ptr[i].width;
        int checkBoxYmin = g_interface.textBoxes_ptr[i].y;
        int checkBoxYmax = g_interface.textBoxes_ptr[i].y + g_interface.textBoxes_ptr[i].height;
        if(checkBoxXmin < xMouse && xMouse < checkBoxXmax 
            && checkBoxYmin < yMouse && yMouse < checkBoxYmax){
                g_interface.textBoxes_ptr[i].active = 1;
                found = 1;
                continue;
        }
        g_interface.textBoxes_ptr[i].active = 0;
    }
    return (found) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int IF_hoverTextBox(int xMouse, int yMouse){
    xMouse = xMouse - g_xShift;
    yMouse = g_windowHeight - yMouse - g_yShift;
    for(int i = 0; i < g_interface.numberTextBoxes; i++){
        int checkBoxXmin = g_interface.textBoxes_ptr[i].x;
        int checkBoxXmax = g_interface.textBoxes_ptr[i].x + g_interface.textBoxes_ptr[i].width;
        int checkBoxYmin = g_interface.textBoxes_ptr[i].y;
        int checkBoxYmax = g_interface.textBoxes_ptr[i].y + g_interface.textBoxes_ptr[i].height;
        if(checkBoxXmin < xMouse && xMouse < checkBoxXmax 
            && checkBoxYmin < yMouse && yMouse < checkBoxYmax){
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

int IF_updateTextBox(){
    for(int i = 0; i < g_interface.numberTextBoxes; i++){
        if(!g_interface.textBoxes_ptr[i].active){
            continue;
        }
        renderTextBox(&g_interface.textBoxes_ptr[i]);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int IF_writeTextBox(char c){
    for(int i = 0; i < g_interface.numberTextBoxes; i++){
        if(!g_interface.textBoxes_ptr[i].active){
            continue;
        }
        if(g_interface.textBoxes_ptr[i].length >= MAX_CHARS - 1){
            return EXIT_FAILURE;
        }
        g_interface.textBoxes_ptr[i].content_ptr[g_interface.textBoxes_ptr[i].length] = c;
        g_interface.textBoxes_ptr[i].content_ptr[g_interface.textBoxes_ptr[i].length + 1] = '\0';
        g_interface.textBoxes_ptr[i].length++;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int IF_deleteTextBox(){
    for(int i = 0; i < g_interface.numberTextBoxes; i++){
        if(!g_interface.textBoxes_ptr[i].active){
            continue;
        }
        if(g_interface.textBoxes_ptr[i].length >= MAX_CHARS - 1){
            return EXIT_FAILURE;
        }
        g_interface.textBoxes_ptr[i].content_ptr[g_interface.textBoxes_ptr[i].length - 1] = '\0';
        g_interface.textBoxes_ptr[i].length--;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}