#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include "../types.h"
#include "widget.h"
#include "../texturesContainer.h"

class Menu
{
  public:
    Menu(SDL_Window* window, std::string backgroundImage);
    ~Menu();
    void draw(bool8 force = FALSE);
    void add(Widget* widget);

  private:
    bool8 change();
    void animate();
    void distributeEventToWidgets(SDL_Event evenement);
    void focusToNext();
    void drawBackground();
    TexturesContainer texturesContainer;
    std::vector<Widget*> widgetsList;
    std::string backgroundImage;
    bool8 stateChanged;
    SDL_Window* window;
    int windowWidth;
    int windowHeight;
};

#endif // MENU_H_INCLUDED
