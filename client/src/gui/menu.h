#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include "../types.h"
#include "widget.h"
#include "../conteneurTextures.h"

class Menu
{
  public:
    Menu(SDL_Window* fenetre, std::string imageFond);
    ~Menu();
    void dessiner(bool8 force = FALSE);
    void ajouter(Widget* widget);

  private:
    bool8 modifie();
    void animer();
    void distribuerEvenementAuxWidgets(SDL_Event evenement);
    void passerLeFocusAuWidgetSuivant();
    void dessinerFond();
    ConteneurTextures conteneurTextures;
    std::vector<Widget*> listeWidgets;
    std::string imageFond;
    bool8 apparenceModifiee;
    SDL_Window* fenetre;
    int largeurFenetre;
    int hauteurFenetre;
};

#endif // MENU_H_INCLUDED
