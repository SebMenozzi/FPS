#include "materiau.h"
#include "traitementChemin.h"

#include <iostream>

Materiau::Materiau()
{
  // Initialisation du materiau avec les parametres par defaut
  this->_Ka.r = 0.2;
  this->_Ka.v = 0.2;
  this->_Ka.b = 0.2;
  this->_Kd.r = 0.8;
  this->_Kd.v = 0.8;
  this->_Kd.b = 0.8;
  this->_Ks.r = 0.0;
  this->_Ks.v = 0.0;
  this->_Ks.b = 0.0;
  this->_d = 1.0;
  this->_Ns = 65.0;
  this->_illum = 2;
  this->_map_Kd = "";
}

Couleur3Float Materiau::Ka()
{
  return this->_Ka;
}

void Materiau::Ka(Couleur3Float Ka)
{
  this->_Ka = Ka;
}

Couleur3Float Materiau::Kd()
{
  return this->_Kd;
}

void Materiau::Kd(Couleur3Float Kd)
{
  this->_Kd = Kd;
}

Couleur3Float Materiau::Ks()
{
  return this->_Ks;
}

void Materiau::Ks(Couleur3Float Ks)
{
  this->_Ks = Ks;
}

float16 Materiau::d()
{
  return this->_d;
}

void Materiau::d(float16 d)
{
  this->_d = d;
}

float16 Materiau::Ns()
{
  return this->_Ns;
}

void Materiau::Ns(float16 Ns)
{
  this->_Ns = Ns;
}

uint32 Materiau::illum()
{
  return this->_illum;
}

void Materiau::illum(uint32 illum)
{
  this->_illum = illum;
}

std::string Materiau::map_Kd()
{
  return this->_map_Kd;
}

void Materiau::map_Kd(std::string cheminFichier)
{
  std::string cheminImage = cheminFinal(this->_cheminDeLaBibliotheque, cheminFichier);

  this->_conteneurTexture.ajouter(cheminImage);
  this->_map_Kd = cheminFichier;

  std::cout << "renseigne " << cheminImage << std::endl;
}

void Materiau::cheminDeLaBibliotheque(std::string chemin)
{
  this->_cheminDeLaBibliotheque = chemin;
}

void Materiau::selectionner()
{
  // Le tag "illum" n'est pas pris en charge

  // Activation de la gestion des texture 2D
  glEnable(GL_TEXTURE_2D);

  // Active le coloriage
  glEnable(GL_COLOR_MATERIAL);

  // Degrade lisse
  glShadeModel (GL_SMOOTH);

  // Ne devrait pas etre utile, pourtant sans
  // elle le resultat est different
  glEnable (GL_NORMALIZE);

  // Selection de la couleur GL_EMISSION en couleur par defaut
  glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
  // Modification de la couleur selectionnee par defaut
  // MTL ne suporte pas de composante GL_EMISSION (Certain editeurs utilisent Ke)
  glColor3f(0.0, 0.0, 0.0);

  // Selection de la couleur GL_AMBIENT en couleur par defaut
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
  // Modification de la couleur selectionnee par defaut
  glColor4f(this->_Ka.r, this->_Ka.v, this->_Ka.b, this->_d);

  // Selection de la couleur GL_DIFFUSE en couleur par defaut
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glColor4f(this->_Kd.r, this->_Kd.v, this->_Kd.b, this->_d);

  // Selection de la couleur GL_SPECULAR en couleur par defaut
  glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
  glColor4f(this->_Ks.r, this->_Ks.v, this->_Ks.b, this->_d);

  // Definition de la brillance
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, this->_Ns);

  // Chargement de la texture
  //this->_cheminDeLaBibliotheque

  std::string cheminImage = cheminFinal(this->_cheminDeLaBibliotheque, this->_map_Kd);
  std::cout << "reclamee " << cheminImage << std::endl;
  glBindTexture(GL_TEXTURE_2D, this->_conteneurTexture.texture(cheminImage).texture);
  //glBindTexture(GL_TEXTURE_2D, 11);

  // TRANSPARENCE
  // Il n'est pas simple de gerer efficacement la semi-transparence
  // Pour des raisons de simplicite, chaques pixels d'une texture
  // sera remplace par un pixel soit totalement opaque, soit
  // totalement transparent. Il n'y aurra pas d'intermediaire

  // Pour les polygones semi-transparent (tentative)
  // glEnable(GL_DEPTH_TEST); // Activation du test de profondeur
  // glDepthMask(GL_FALSE); // Pas d'ecriture dans le z-buffer
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Activation du test de transparence
  glEnable(GL_ALPHA_TEST);

  // Definition du seuil de transparence
  glAlphaFunc(GL_GREATER, 0.5);
}

uint32 Materiau::tex()
{
  std::string cheminImage = cheminFinal(this->_cheminDeLaBibliotheque, this->_map_Kd);
  return this->_conteneurTexture.texture(cheminImage).texture;
}
