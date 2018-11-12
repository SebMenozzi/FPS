#ifndef HEIGHTMAP_H_INCLUDED
#define HEIGHTMAP_H_INCLUDED

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#include <iostream>
#include "conteneurTextures.h"
#include "fonctionsUtiles.h"
#include "point3Float.h"
#include "point2Float.h"
#include <vector>

class Heightmap
{
  private:
    ConteneurTextures conteneurTextures;
    std::vector<std::vector<float16> > hauteurs;
    SDL_Surface* heightmap;
    uint32 hauteur;
    uint32 largeur;
    float16 atenuation;

  public:
    Heightmap(const char* nomFichier);
    ~Heightmap();
    void dessiner();
    float16 lireHauteur(float16 x, float16 y);
};

#endif // HEIGHTMAP_H_INCLUDED
