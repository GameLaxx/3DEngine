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
extern int g_windowWidth;
extern int g_windowHeight;
extern int g_pixelWidth;
extern int g_pixelHeight;
extern int g_xShift;
extern int g_yShift;
extern rgba_t g_whiteColor;
//-----------------------------------------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------------------------------------
/* Maintenance Functions */
/**
 * @brief Init every usefull variables for the canvas to run.
 * 
 * @param windowWidth Width of the window.
 * @param windowHeight Height of the window.
 * @return int 
 */
int DRAW_initSDL(int windowWidth, int windowHeight);

/**
 * @brief Show modifications applied to the canvas.
 * 
 * @return int 
 */
int DRAW_showRenderer();

/**
 * @brief Draw the background in the default color.
 * 
 * @return int 
 */
int DRAW_clearRenderer();

/**
 * @brief Clear every used variables for the canvas.
 * 
 * @return int 
 */
int DRAW_cleanRenderer();

/* Canvas Functions */
/**
 * @brief Move the origin of the canvas to the coordinates (x,y) (default is (0,0))
 * 
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return int 
 */
int DRAW_moveOrigin(int x, int y);

/**
 * @brief Invert Oy axis of the canvas. Default is Oy+ going down.
 * 
 * @return int 
 */
int DRAW_invertYAxis();

/* Color functions */
/**
 * @brief Return the default color of the background.
 * 
 * @param ret_ptr The variable in which the result is stored.
 * 
 * @return The newly allocated color.
 */
int DRAW_initBackgroundColor(rgba_t* ret_ptr);

/**
 * @brief Compute the new color given an intensity.
 * 
 * @param color_ptr The color that will be used as a base.
 * @param intensity The intensity (<0 completely black, 1 the initial color, >1 can lead to full white).
 * @param ret_ptr The variable in which the result is stored.
 * @return The newly allocated color.
 */
int DRAW_addIntensity(rgba_t* color_ptr, float intensity, rgba_t* ret_ptr);

void DRAW_computeReflection(rgba_t* localColor_ptr, rgba_t* recursiveColor_ptr, float reflection);
/* Drawing Functions */
/**
 * @brief Draw a line between (x1,y1) and (x2,y2) in a given color.
 * 
 * @param x1 First x coordinate.
 * @param y1 First y coordinate.
 * @param x2 Second x coordinate.
 * @param y2 Second y coordinate.
 * @param color_ptr The color of the line.
 * @return SUCCESS or FAILURE.
 */
int DRAW_line(int x1, int y1, int x2, int y2, rgba_t* color_ptr);

/**
 * @brief Draw the outling of a rectangle.
 * 
 * @param x X coordinate of the origin.
 * @param y Y coordinate of the origin.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 * @param color_ptr Color of the rectangle.
 * @return SUCCESS or FAILURE.
 */
int DRAW_rectangleOutline(int x, int y, int width, int height, rgba_t* color_ptr);

/**
 * @brief Fill a rectangle.
 * 
 * @param x X coordinate of the origin.
 * @param y Y coordinate of the origin.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 * @param color_ptr Color of the rectangle.
 * @return SUCCESS or FAILURE.
 */
int DRAW_rectangleFill(int x, int y, int width, int height, rgba_t* color_ptr);

/**
 * @brief Draw one pixel in a given color.
 * 
 * @param x X coordinate of the origin.
 * @param y Y coordinate of the origin.
 * @param color_ptr Color of the pixel.
 * @return SUCCESS or FAILURE.
 */
int DRAW_pixel(int x, int y, rgba_t* color_ptr);

int DRAW_text(int x, int y, char* content_ptr, rgba_t* color_ptr);

int DRAW_textBox(int x, int y, int width, int height, char* content_ptr, rgba_t* boxColor_ptr, rgba_t* textColor_ptr);

#endif /* DRAW_H */