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
#include "texturesContainer.h"

class Objet3DStatique
{
  private:
    typedef std::set<std::string> Textures;

    Textures textures; // Les textures de l'objet
    TexturesContainer texturesContainer;
    GLuint listeAffichage;
    GLuint listeRST;

  protected:
    float16 x;
    float16 y;
    float16 z;

    // Angle horizontal avec la verticale en z
    float16 horizontalAngle;

    std::string nom;

  public:
    Objet3DStatique(std::string nomFichier);
    ~Objet3DStatique();
    void dessiner();
};

#endif // OBJET3DSTATIQUE_H_INCLUDED
