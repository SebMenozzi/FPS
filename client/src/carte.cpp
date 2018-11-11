#include <SDL2/SDL.h>
#include "carte.h"

Carte::Carte(const char* nomFichier)
{
  // Initialisation des attributs
  this->carte = NULL;
  this->largeurCarte = 0u;
  this->hauteurCarte = 0u;
  this->listeAffichage = 0u;

  // Tentative de chargement
  SDL_Surface* imageCarte = SDL_LoadBMP(nomFichier);

  // Si l'image est chargee en 24 bits/px
  if ((NULL != imageCarte) && (24 == imageCarte->format->BitsPerPixel))
  {
    // Dimension de la carte
    this->largeurCarte = imageCarte->w;
    this->hauteurCarte = imageCarte->h;

    // Calcule le nombre de cases
    uint32 nbCases = this->largeurCarte * this->hauteurCarte;

    // Instantiation des cases de la carte
    this->carte = new bool8[nbCases];

    // Parcours de chaques lignes
    for(uint32 ligne = 0; ligne < this->hauteurCarte; ligne++)
    {
      // Structuration des pixels BGR pour une lecture simplifiee
      typedef struct {uint8 bleu; uint8 vert; uint8 rouge;} bgr;
      bgr* pixelsCarte = (bgr*) ((uint8*)imageCarte->pixels + (ligne * imageCarte->pitch));

      // Parcours des pixels de la ligne
      for(uint32 pixel = 0; pixel < this->largeurCarte; pixel++)
      {
        // Si la moyenne des 3 couleurs est superieur � 50% de la luminosit� maximale
        if((pixelsCarte[pixel].rouge + pixelsCarte[pixel].vert + pixelsCarte[pixel].bleu) > 382)
        {
          // Pas de mur
          this->carte[(this->largeurCarte * ligne) + pixel] = 0;
        }
        else
        {
          this->carte[(this->largeurCarte * ligne) + pixel] = 1;
        }
      }
    }

    // Creation de la liste d'affichage de la carte
    this->creerListeAffichage();

    SDL_FreeSurface(imageCarte);
  }
  else  // Erreur de chargement
  {
    printf("Erreur de chargement de la carte\n");
  }
}

Carte::~Carte()
{
  delete this->carte;
  this->conteneurTextures.supprimer("mur.bmp");
  this->conteneurTextures.supprimer("herbe.bmp");
}

void Carte::dessiner()
{
  glCallList(this->listeAffichage);
}

void Carte::creerListeAffichage()
{
  // Chargement de la texture du mur
  this->conteneurTextures.ajouter("mur.bmp");
  this->conteneurTextures.ajouter("herbe.bmp");

  // Creation de la liste d'affichage
  this->listeAffichage = glGenLists(1);
  glNewList(this->listeAffichage, GL_COMPILE);

  // Activation des textures
  glEnable(GL_TEXTURE_2D);

  // Selection de la texture
  glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture("mur.bmp").texture);

  bool8 caseGauche;
  bool8 caseDroite;

  #define HAUTEUR_MURS 1

  // Parcours des lignes
  for(uint32 j = 0; j < this->hauteurCarte; j++)
  {
    // Parcours d'une ligne
    for (uint32 i = 0u; i < this->largeurCarte - 1; i++)
    {
      caseGauche = this->carte[(j*this->largeurCarte) + i];
      caseDroite = this->carte[(j*this->largeurCarte) + i+1];

      // Si le mur est visible de l'ouest vers l'est
      if ((0 == caseGauche) && (1 == caseDroite))
      {
        // Dessin du mur
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0); glVertex3i(j+0, i+1, HAUTEUR_MURS);
        glTexCoord2i(0, 1); glVertex3i(j+0, i+1, 0);
        glTexCoord2i(1, 1); glVertex3i(j+1, i+1, 0);
        glTexCoord2i(1, 0); glVertex3i(j+1, i+1, HAUTEUR_MURS);
        glEnd();
      }

      // Si le mur est visible de l'est vers l'ouest
      if ((1 == caseGauche) && (0 == caseDroite))
      {
        // Dessin du mur
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0); glVertex3i(j+1, i+1, HAUTEUR_MURS);
        glTexCoord2i(0, 1); glVertex3i(j+1, i+1, 0);
        glTexCoord2i(1, 1); glVertex3i(j+0, i+1, 0);
        glTexCoord2i(1, 0); glVertex3i(j+0, i+1, HAUTEUR_MURS);
        glEnd();
      }
    }
  }

  bool8 caseHaut;
  bool8 caseBas;

  // Parcours des colonnes
  for(uint32 j = 0; j < this->largeurCarte - 1; j++)
  {
    // Parcours d'une colonne
    for (uint32 i = 0u; i < this->largeurCarte; i++)
    {
      caseHaut = this->carte[(j*this->hauteurCarte) + i];
      caseBas = this->carte[((j+1)*this->hauteurCarte) + i];

      // Si le mur est visible du nord vers le sud
      if ((0 == caseHaut) && (1 == caseBas))
      {
        // Dessin du mur
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0); glVertex3i(j+1, i+1, HAUTEUR_MURS);
        glTexCoord2i(0, 1); glVertex3i(j+1, i+1, 0);
        glTexCoord2i(1, 1); glVertex3i(j+1, i+0, 0);
        glTexCoord2i(1, 0); glVertex3i(j+1, i+0, HAUTEUR_MURS);
        glEnd();
      }

      // Si le mur est visible du sud vers le nord
      if ((1 == caseHaut) && (0 == caseBas))
      {
        // Dessin du mur
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0); glVertex3i(j+1, i+0, HAUTEUR_MURS);
        glTexCoord2i(0, 1); glVertex3i(j+1, i+0, 0);
        glTexCoord2i(1, 1); glVertex3i(j+1, i+1, 0);
        glTexCoord2i(1, 0); glVertex3i(j+1, i+1, HAUTEUR_MURS);
        glEnd();
      }
    }
  }

  // L'herbe
  glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture("herbe.bmp").texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Repete la texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex3i(0, 0, 0);
    glTexCoord2i(0, 10); glVertex3i(64, 0, 0);
    glTexCoord2i(10, 10); glVertex3i(64, 64, 0);
    glTexCoord2i(10, 0); glVertex3i(0, 64, 0);
  glEnd();

  // Fin de la liste d'affichage
  glEndList();
}

void Carte::entourage(sint32 x, sint32 y, bool8 entourage[8])
{
  sint32 largeurCarte = this->largeurCarte;

  // Par defaut, pas de murs
  entourage[0] = 0;
  entourage[1] = 0;
  entourage[2] = 0;
  entourage[3] = 0;
  entourage[4] = 0;
  entourage[5] = 0;
  entourage[6] = 0;
  entourage[7] = 0;

  // Si le personnage est a gauche
  if (0 == x)
  {
    entourage[0] = 1;
    entourage[3] = 1;
    entourage[5] = 1;
  }

  // Si le personnage est a droite
  if ((sint32)(largeurCarte-1) == x)
  {
    entourage[2] = 1;
    entourage[4] = 1;
    entourage[7] = 1;
  }

  // Si le personnage est en haut
  if (0 == y)
  {
    entourage[0] = 1;
    entourage[1] = 1;
    entourage[2] = 1;
  }

  // Si le personnage est en haut
  if ((sint32) (hauteurCarte-1) == y)
  {
    entourage[5] = 1;
    entourage[6] = 1;
    entourage[7] = 1;
  }

  // Construction de la vue de l'entourage dans le milieu de la carte
  if (y >= 0 && y <= (sint32)hauteurCarte - 1 && x >= 0 && x <= largeurCarte - 1)
  {
    if (0 == entourage[0]) entourage[0] = this->carte[(x-1) + ((y-1) * largeurCarte)];
    if (0 == entourage[1]) entourage[1] = this->carte[(x)   + ((y-1) * largeurCarte)];
    if (0 == entourage[2]) entourage[2] = this->carte[(x+1) + ((y-1) * largeurCarte)];
    if (0 == entourage[3]) entourage[3] = this->carte[(x-1) + ((y)   * largeurCarte)];
    if (0 == entourage[4]) entourage[4] = this->carte[(x+1) + ((y)   * largeurCarte)];
    if (0 == entourage[5]) entourage[5] = this->carte[(x-1) + ((y+1) * largeurCarte)];
    if (0 == entourage[6]) entourage[6] = this->carte[(x)   + ((y+1) * largeurCarte)];
    if (0 == entourage[7]) entourage[7] = this->carte[(x+1) + ((y+1) * largeurCarte)];
  }

  // ATTENTION : Pour une gestion correcte des collisions,
  // la carte doit etre entouree de murs.
}
