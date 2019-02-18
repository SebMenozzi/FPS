#ifndef OVERLAY_H_INCLUDED
#define OVERLAY_H_INCLUDED

#include <vector>
#include "widgetOverlay.h"

class Overlay
{
  public:
    Overlay(SDL_Window* fenetre);
    ~Overlay();
    void ajouter(WidgetOverlay* widget);
    void dessiner();

  private:
    std::vector<WidgetOverlay*> listeWidgets;

    // Fenetre
    SDL_Window* fenetre;
    int largeurFenetre;
    int hauteurFenetre;
};

#endif // OVERLAY_H_INCLUDED
