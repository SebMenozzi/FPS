#ifndef ZONETEXTE_H_INCLUDED
#define ZONETEXTE_H_INCLUDED

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../conteneurTextures.h"
#include "widget.h"

class ZoneTexte : public Widget
{
  public:
    ZoneTexte(SDL_Window* fenetre, sint32 x, sint32 y);
    ~ZoneTexte();
    void toucheEnfoncee(char c);
    void modifierTexte(std::string texte, bool8 curseur = FALSE);
    std::string texte();
    void prendLeFocus();
    void perdLeFocus();

  private:
    std::string _texte;
    ConteneurTextures conteneurTextures;
    GLuint textureTexte;
    static TTF_Font* police;
    static uint32 compteurInstance;
    uint32 largeurTexte;
    uint32 hauteurTexte;
    uint32 largeurTexture;
    uint32 hauteurTexture;
    float16 largeurPortionUtile;
    float16 hauteurPortionUtile;
    void dessinDuWidget();

    int largeurFenetre;
    int hauteurFenetre;
};


#endif // ZONETEXTE_H_INCLUDED
