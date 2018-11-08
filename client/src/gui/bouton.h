#ifndef BOUTON_H_INCLUDED
#define BOUTON_H_INCLUDED

#include <SDL2/SDL.h>
#include "widget.h"
#include "../types.h"
#include <string>
#include "../conteneurTextures.h"
#include <SDL2/SDL_ttf.h>

class Bouton : public Widget
{
  public:
    Bouton(SDL_Window* fenetre, sint32 x, sint32 y, std::string texte);
    ~Bouton();
    //void dessiner();
    void toucheEnfoncee(char c);
    std::string texte();
    void texte(std::string texte);

  private:
    ConteneurTextures conteneurTextures;
    GLuint textureTexte;
    uint32 largeurTexte;
    uint32 hauteurTexte;
    uint32 largeurTexture;
    uint32 hauteurTexture;
    float16 largeurPortionUtile;
    float16 hauteurPortionUtile;
    static TTF_Font* police;
    static uint32 compteurInstances;
    void dessinDuWidget();
    std::string _texte;

    int largeurFenetre;
    int hauteurFenetre;
};

#endif // BOUTON_H_INCLUDED
