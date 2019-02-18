#include <SDL2/SDL.h>
#include "map.h"

Map::Map(const char* nomFichier)
{
  // Initialisation des attributs
  this->map = NULL;
  this->width = 0u;
  this->height = 0u;
  this->draw_list = 0u;

  // Tentative de chargement
  SDL_Surface* imagemap = SDL_LoadBMP(nomFichier);

  // Si l'image est chargee en 24 bits/px
  if ((NULL != imagemap) && (24 == imagemap->format->BitsPerPixel))
  {
    // Dimension de la map
    this->width = imagemap->w;
    this->height = imagemap->h;

    // Calcule le nombre de cases
    uint32 nbCases = this->width * this->height;

    // Instantiation des cases de la map
    this->map = new bool8[nbCases];

    // Parcours de chaques lignes
    for(uint32 ligne = 0; ligne < this->height; ligne++)
    {
      // Structuration des pixels BGR pour une lecture simplifiee
      typedef struct {uint8 bleu; uint8 vert; uint8 rouge;} bgr;
      bgr* pixelsmap = (bgr*) ((uint8*)imagemap->pixels + (ligne * imagemap->pitch));

      // Parcours des pixels de la ligne
      for(uint32 pixel = 0; pixel < this->width; pixel++)
      {
        // Si la moyenne des 3 couleurs est superieur � 50% de la luminosit� maximale
        if((pixelsmap[pixel].rouge + pixelsmap[pixel].vert + pixelsmap[pixel].bleu) > 382)
        {
          // Pas de mur
          this->map[(this->width * ligne) + pixel] = 0;
        }
        else
        {
          this->map[(this->width * ligne) + pixel] = 1;
        }
      }
    }

    // Creation de la liste d'affichage de la map
    this->create_draw_list();

    SDL_FreeSurface(imagemap);
  }
  else  // Erreur de chargement
  {
    printf("Erreur de chargement de la map\n");
  }
}

Map::~Map()
{
  delete this->map;
  this->texturesContainer.deleteTexture("mur.bmp");
  this->texturesContainer.deleteTexture("herbe.bmp");
}

void Map::draw()
{
  glCallList(this->draw_list);
}

void Map::create_draw_list()
{
  // Chargement de la texture du mur
  this->texturesContainer.addTexture("mur.bmp");
  this->texturesContainer.addTexture("herbe.bmp");

  // Creation de la liste d'affichage
  this->draw_list = glGenLists(1);
  glNewList(this->draw_list, GL_COMPILE);

  // Activation des textures
  glEnable(GL_TEXTURE_2D);

  // Selection de la texture
  glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture("mur.bmp").texture);

  bool8 caseGauche;
  bool8 caseDroite;

  #define HAUTEUR_MURS 1

  // Parcours des lignes
  for(uint32 j = 0; j < this->height; j++)
  {
    // Parcours d'une ligne
    for (uint32 i = 0u; i < this->width - 1; i++)
    {
      caseGauche = this->map[(j*this->width) + i];
      caseDroite = this->map[(j*this->width) + i+1];

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
  for(uint32 j = 0; j < this->width - 1; j++)
  {
    // Parcours d'une colonne
    for (uint32 i = 0u; i < this->width; i++)
    {
      caseHaut = this->map[(j*this->height) + i];
      caseBas = this->map[((j+1)*this->height) + i];

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
  glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture("herbe.bmp").texture);
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

void Map::contourn(sint32 x, sint32 y, bool8 entourage[8])
{
  sint32 width = this->width;

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
  if ((sint32)(width-1) == x)
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
  if ((sint32) (height-1) == y)
  {
    entourage[5] = 1;
    entourage[6] = 1;
    entourage[7] = 1;
  }

  // Construction de la vue de l'entourage dans le milieu de la map
  if (y >= 0 && y <= (sint32)height - 1 && x >= 0 && x <= width - 1)
  {
    if (0 == entourage[0]) entourage[0] = this->map[(x-1) + ((y-1) * width)];
    if (0 == entourage[1]) entourage[1] = this->map[(x)   + ((y-1) * width)];
    if (0 == entourage[2]) entourage[2] = this->map[(x+1) + ((y-1) * width)];
    if (0 == entourage[3]) entourage[3] = this->map[(x-1) + ((y)   * width)];
    if (0 == entourage[4]) entourage[4] = this->map[(x+1) + ((y)   * width)];
    if (0 == entourage[5]) entourage[5] = this->map[(x-1) + ((y+1) * width)];
    if (0 == entourage[6]) entourage[6] = this->map[(x)   + ((y+1) * width)];
    if (0 == entourage[7]) entourage[7] = this->map[(x+1) + ((y+1) * width)];
  }

  // ATTENTION : Pour une gestion correcte des collisions,
  // la map doit etre entouree de murs.
}
