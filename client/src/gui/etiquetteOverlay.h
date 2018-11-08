#ifndef ETIQUETTEOVERLAY_H_INCLUDED
#define ETIQUETTEOVERLAY_H_INCLUDED

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#include "widgetOverlay.h"
#include "../types.h"

class EtiquetteOverlay : public WidgetOverlay
{
  public:
    EtiquetteOverlay(SDL_Window* fenetre, sint32 x, sint32 y, std::string texte, uint32 taille = 45, uint8 r = 255, uint8 v = 255, uint8 b = 255, std::string police = "Cartonsix NC.ttf");
    virtual ~EtiquetteOverlay();

    void modifierTexte(std::string texte);
    void modifierCouleur(uint8 r, uint8 v, uint8 b);
    bool modifierPoliceTaille(std::string police, uint32 taille);

    void dessiner();

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
    SDL_Color couleur;

    void regenererTexture(void);
};

#endif // ETIQUETTEOVERLAY_H_INCLUDED
