#include <SDL2/SDL.h>
#include "heightmap.h"

Heightmap::Heightmap(const char* nomFichier)
{
  this->heightmap = NULL;
  this->hauteur = 0u;
  this->largeur = 0u;
  this->atenuation = 10.0;

  // Ajouter la texture
  this->conteneurTextures.ajouter("herbe.bmp");

  this->heightmap = SDL_LoadBMP(nomFichier);

  // Si l'image est chargee
  if ((NULL != this->heightmap))
  {
    // Dimension de la carte
    this->largeur = this->heightmap->w;
    this->hauteur = this->heightmap->h;

    std::vector<float> tmp;
  	for(int y = 0; y < this->hauteur; y++)
  	{
  		tmp.clear();
  		for(int x = 0; x < this->largeur; x++)
  		{
        Uint32 pixel = getpixel(this->heightmap, x, y);
  			Uint8 r, g, b;
  			SDL_GetRGB(pixel, this->heightmap->format, &r, &g, &b);
  			tmp.push_back((float16) r);
  		}
  		this->hauteurs.push_back(tmp);
  	}
  }
  else  // Erreur de chargement
  {
    std::cout << "Impossible de charger le heightmap" << std::endl;
  }
}

Heightmap::~Heightmap()
{
  SDL_FreeSurface(this->heightmap);

  delete this->heightmap;

  this->conteneurTextures.supprimer("herbe.bmp");
}

void Heightmap::dessiner()
{
  // Activation des textures
  glEnable(GL_TEXTURE_2D);

  // Selection de la texture
  glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture("herbe.bmp").texture);
  if (NULL != this->heightmap)
  {
    for(int y = 0; y < this->hauteur - 1; y++) {
      glBegin(GL_TRIANGLE_STRIP);
      for(int x = 0; x < this->largeur - 1; x++) {
        glTexCoord2d(x, y);
        glVertex3f(x, y, this->hauteurs[x][y] / this->atenuation);
        glTexCoord2d(x, y + 1);
        glVertex3f(x, y + 1, this->hauteurs[x][y + 1] / this->atenuation);
      }
      glEnd();
    }
  }
}

float16 Heightmap::lireHauteur(float16 x, float16 y)
{
  float gridSquareSize = this->largeur / ((float) this->hauteurs.size());

  int gridX = (int) floor(x / gridSquareSize);
  int gridY = (int) floor(y / gridSquareSize);

  if (gridX >= this->largeur - 1 || gridY >= this->hauteur || gridX < 0 || gridY < 0)
  {
    return 0;
  }

  float16 xCoord = fmod(x, gridSquareSize) / gridSquareSize;
  float16 yCoord = fmod(y, gridSquareSize) / gridSquareSize;

  if (xCoord <= (1 - yCoord))
  {
    Point3Float p1 = { 0, hauteurs[gridX][gridY] / this->atenuation, 0 };
    Point3Float p2 = { 1, hauteurs[gridX + 1][gridY] / this->atenuation, 0 };
    Point3Float p3 = { 0, hauteurs[gridX][gridY + 1] / this->atenuation, 1 };
    Point2Float pos = { xCoord, yCoord };

    return barycentre(p1, p2, p3, pos);
  }
  else
  {
    Point3Float p1 = { 1, hauteurs[gridX + 1][gridY] / this->atenuation, 0 };
    Point3Float p2 = { 1, hauteurs[gridX + 1][gridY + 1] / this->atenuation, 1 };
    Point3Float p3 = { 0, hauteurs[gridX][gridY + 1] / this->atenuation, 1 };
    Point2Float pos = { xCoord, yCoord };

    return barycentre(p1, p2, p3, pos);
  }
}
