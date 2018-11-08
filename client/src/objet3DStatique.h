#ifndef OBJET3DSTATIQUE_H_INCLUDED
#define OBJET3DSTATIQUE_H_INCLUDED

#include <string>
#include <set>
#include "types.h"
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#include "conteneurTextures.h"

class Objet3DStatique
{
  private:
    typedef std::set<std::string> Textures;

    Textures textures; // Les textures de l'objet
    ConteneurTextures conteneurTextures;
    GLuint listeAffichage;
    GLuint listeRST;

  protected:
    // Position de l'objet
    float16 positionX;
    float16 positionY;
    float16 positionZ;

    // Angle horizontal avec la verticale en z
    float16 angleHorizontal;

    std::string nom;

  public:
    Objet3DStatique(std::string nomFichier);
    ~Objet3DStatique();
    void dessiner();
};

#endif // OBJET3DSTATIQUE_H_INCLUDED