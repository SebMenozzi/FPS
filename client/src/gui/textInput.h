#ifndef INPUT_TEXT_H_INCLUDED
#define INPUT_TEXT_H_INCLUDED

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../texturesContainer.h"
#include "widget.h"

class TextInput : public Widget
{
  public:
    TextInput(SDL_Window* window, sint32 x, sint32 y);
    ~TextInput();
    void keyPressed(char key);
    void changeText(std::string text, bool8 cursor = FALSE);
    std::string getText();
    void focus();
    void blur();

  private:
    std::string text;
    TexturesContainer texturesContainer;
    GLuint textTexture;
    static TTF_Font* font;
    static uint32 compteurInstance;
    uint32 textWidth;
    uint32 textHeight;
    uint32 textureWidth;
    uint32 textureHeight;
    float16 largeurPortionUtile;
    float16 hauteurPortionUtile;
    void drawWidget();

    int windowWidth;
    int windowHeight;
};


#endif // INPUT_TEXT_H_INCLUDED
