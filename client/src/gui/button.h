#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

#include <SDL2/SDL.h>
#include "widget.h"
#include "../types.h"
#include <string>
#include "../conteneurTextures.h"
#include <SDL2/SDL_ttf.h>

class Button : public Widget
{
  public:
    Button(SDL_Window* window, sint32 x, sint32 y, std::string text);
    ~Button();
    void focus(char c);
    std::string getText();
    void setText(std::string text);

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
    std::string text;

    int windowWidth;
    int windowHeight;
};

#endif // BUTTON_H_INCLUDED
