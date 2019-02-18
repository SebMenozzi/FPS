#ifndef LABEL_H_INCLUDED
#define LABEL_H_INCLUDED

#include <string>
#include <SDL2/SDL_ttf.h>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#include "widget.h"
#include "../types.h"

class Label : public Widget
{
  public:
    Label(SDL_Window* window, sint32 x, sint32 y, std::string text, uint32 size = 35);
    ~Label();
    void changeText(std::string text);

  private:
    std::string text;
    GLuint textTexture;
    uint32 textWidth;
    uint32 textHeight;
    uint32 textureWidth;
    uint32 textureHeight;
    float16 largeurPortionUtile;
    float16 hauteurPortionUtile;
    TTF_Font* font;
    void drawWidget();
    int windowWidth;
    int windowHeight;
};

#endif // LABEL_H_INCLUDED
