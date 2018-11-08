#ifndef ETIQUETTE_H_INCLUDED
#define ETIQUETTE_H_INCLUDED

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

class Etiquette : public Widget
{
  public:
    Etiquette(SDL_Window* fenetre, sint32 x, sint32 y, std::string texte, uint32 taille = 35);
    ~Etiquette();
    void modifierTexte(std::string texte);

  private:
    std::string texte;
    GLuint textureTexte;
    uint32 largeurTexte;
    uint32 hauteurTexte;
    uint32 largeurTexture;
    uint32 hauteurTexture;
    float16 largeurPortionUtile;
    float16 hauteurPortionUtile;
    TTF_Font* police;
    void dessinDuWidget();
    int largeurFenetre;
    int hauteurFenetre;
};

#endif // ETIQUETTE_H_INCLUDED
