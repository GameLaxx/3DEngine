/* interface.h
 * date : 25/04/2025 (dd/mm/yy)
 * author : tboisse
*/
#ifndef INTERFACE_H
#define INTERFACE_H
//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include "draw.h"
#include "RenderTools/objects.h"
//-----------------------------------------------------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------------------------------------------------
#define MAX_BOXES 10
#define MAX_CHARS 256
//-----------------------------------------------------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------------------------------------------------
typedef struct meshBox_s meshBox_t;
typedef struct textBox_s textBox_t;
typedef struct interface_s interface_t;
//-----------------------------------------------------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------------------------------------------------
struct meshBox_s {
    int x;
    int y;
    int width;
    int height;
    int margin;
    int padding;
    char* name;
};

struct textBox_s {
    int x;
    int y;
    int width;
    int height;
    char content_ptr[MAX_CHARS];
    int length;
    int active;
    int textType;
};

struct interface_s{
    meshBox_t* meshBoxes_ptr;
    textBox_t* textBoxes_ptr;
    int numberMeshBoxes;
    int numberTextBoxes;
};
//-----------------------------------------------------------------------------------------------------------------------
// Enums
//-----------------------------------------------------------------------------------------------------------------------
enum textType_e{
    TT_NOTEXT = 0,
    TT_INT,
    TT_FLOAT
};
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
extern interface_t g_interface;
//-----------------------------------------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------------------------------------
int IF_initInterface();

int IF_cleanInterface();

int IF_updateInterface();

int IF_renderInterfaceLeft();
int IF_renderInterfaceRight();
int IF_renderInterface();

int IF_drawMeshBoxes();

int IF_clickMeshBox(int xMouse, int yMouse);
int IF_hoverMeshBox(int xMouse, int yMouse);

int IF_renderInterfaceObject(object_t* object_ptr);
int IF_clickTextBox(int xMouse, int yMouse);
int IF_hoverTextBox(int xMouse, int yMouse);
int IF_updateTextBox();
int IF_writeTextBox(char c, object_t* object_ptr);
int IF_deleteTextBox(object_t* object_ptr);

#endif /* INTERFACE_H */