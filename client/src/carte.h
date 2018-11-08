#ifndef CARTE_H_INCLUDED
#define CARTE_H_INCLUDED

#include <set>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#include <string>
#include "conteneurTextures.h"
#include "types.h"

class Carte
{
  private:
    ConteneurTextures conteneurTextures;

    bool8* carte;
    uint32 largeurCarte;
    uint32 hauteurCarte;
    GLuint listeAffichage;

    void creerListeAffichage();

  public:
    Carte(const char* nomFichier);
    ~Carte();
    void dessiner();

    void entourage(sint32 x, sint32 y, bool8 entourage[8]);
};

#endif // CARTE_H_INCLUDED
