#ifndef WIDGETOVERLAY_H_INCLUDED
#define WIDGETOVERLAY_H_INCLUDED

#include <SDL2/SDL.h>
#include "../types.h"

class WidgetOverlay
{
  public:
    WidgetOverlay(SDL_Window* fenetre, sint32 x, sint32 y);
    virtual ~WidgetOverlay();
    virtual void dessiner() = 0;

    void rendreVisible(void);
    void rendreInvisible(void);

  protected:
    uint32 puissanceDeDeuxSuperieure(uint32 i);

    sint32 _positionX;
    sint32 _positionY;
    uint32 _largeur;
    uint32 _hauteur;

    bool8 visible;

    SDL_Window* fenetre;
    int largeurFenetre;
    int hauteurFenetre;
};

#endif // WIDGETOVERLAY_H_INCLUDED
