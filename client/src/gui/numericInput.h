#ifndef NUMERIC_INPUT_H_INCLUDED
#define NUMERIC_INPUT_H_INCLUDED

#include <SDL2/SDL_ttf.h>
#include "widget.h"
#include "../types.h"
#include "../conteneurTextures.h"

class NumericInput : public Widget
{
  public:
    NumericInput(sint32 x, sint32 y, sint32 value);
    ~NumericInput();
    void draw();
    void keyPressed(char key);
    void changeValue(sint32 value, bool8 cursor = FALSE);
    sint32 getValue();
    void focus();
    void blur();
    void changeMin(sint32 min);
    void changeMax(sint32 max);

  private:
    ConteneurTextures texturesContainer;
    GLuint textTexture;
    uint32 textWidth;
    uint32 textHeight;
    uint32 textureWidth;
    uint32 textureHeight;
    float16 largeurPortionUtile;
    float16 hauteurPortionUtile;
    static TTF_Font* font;
    static uint32 compteurInstances;
    void drawWidget();
    sint32 value;
    bool8 topArrowHovered();
    bool8 bottomArrowHovered();
    void leftClickReleased();
    void cursorMoved(uint32 x, uint32 y);
    sint32 min;
    sint32 max;
};

#endif // NUMERIC_INPUT_H_INCLUDED
