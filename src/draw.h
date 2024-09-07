/* draw.h
 * date : 05/09/2024 (dd/mm/yy)
 * author : tboisse
*/
#ifndef DRAW_H
#define DRAW_H
//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <SDL2/SDL.h>
#include <stdint.h>
//-----------------------------------------------------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------------------------------------------------
typedef struct rgba_s rgba_t;
typedef int (*rectangleFunction)(SDL_Renderer*, const SDL_Rect*);
//-----------------------------------------------------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------------------------------------------------
struct rgba_s{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};
//-----------------------------------------------------------------------------------------------------------------------
// Enums
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
extern SDL_Renderer* g_renderer;
extern int windowWidth;
extern int windowHeight;
extern rgba_t g_whiteColor;
//-----------------------------------------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------------------------------------
int DRAW_initSDL(int windowWidth, int windowHeight);
int DRAW_showRenderer();
int DRAW_clearRenderer();
int DRAW_cleanRenderer();

int DRAW_moveOrigin(int x, int y);
int DRAW_invertYAxis();

rgba_t* DRAW_initBackgroundColor();
rgba_t* DRAW_addIntensity(rgba_t* color_ptr, float intensity);
void DRAW_computeReflection(rgba_t* localColor_ptr, rgba_t* recursiveColor_ptr, float reflection);

int DRAW_line(int x1, int y1, int x2, int y2, rgba_t* color_ptr);
int DRAW_rectangleOutline(int x, int y, int width, int height, rgba_t* color_ptr);
int DRAW_rectangleFill(int x, int y, int width, int height, rgba_t* color_ptr);
int DRAW_pixel(int x, int y, rgba_t* color_ptr);
#endif