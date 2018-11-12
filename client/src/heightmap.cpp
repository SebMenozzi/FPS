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

    std::vector<float16> tmp;
  	for(int y = 0; y < this->hauteur; y++)
  	{
  		tmp.clear();
  		for(int x = 0; x < this->largeur; x++)
  		{
  			tmp.push_back(((float16) (getpixel(this->heightmap, x, y) & 0xff)));
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
        //glVertex3f(x, y, ((float16) (getpixel(this->heightmap, x, y) & 0xff)) / this->atenuation); // ça ne fonctionne pas avec

        glTexCoord2d(x, y + 1);
        glVertex3f(x, y + 1, this->hauteurs[x][y + 1] / this->atenuation);
        //glVertex3f(x, y + 1, ((float16) (getpixel(this->heightmap, x, y + 1) & 0xff)) / this->atenuation); // ça ne fonctionne pas avec
      }
      glEnd();
    }
  }
}

float16 Heightmap::lireHauteur(float16 x, float16 y)
{
  // algo de Seb

  float gridSquareSize = this->hauteur / ((float) this->hauteurs.size()); // 1.0

  int gridX = (int) floor(x / gridSquareSize);
  int gridY = (int) floor(y / gridSquareSize);

  if (gridX >= this->largeur || gridY >= this->hauteur || gridX < 0 || gridY < 0)
  {
    return 0;
  }

  float16 xCoord = fmod(x, gridSquareSize) / gridSquareSize;
  float16 yCoord = fmod(y, gridSquareSize) / gridSquareSize;

  //printf("x1: %f x2: %f\n", ((float16)(getpixel(this->heightmap, x, y) & 0xff)), this->hauteurs[x][y]);
  printf("x1: %f x2: %f\n", gridX, xCoord);

  if (xCoord <= (1 - yCoord))
  {
    Point3Float p1 = { 0, 0, ((float16)(getpixel(this->heightmap, gridX, gridY) & 0xff)) / this->atenuation };
    Point3Float p2 = { 1, 0, ((float16)(getpixel(this->heightmap, gridX + 1, gridY) & 0xff)) / this->atenuation };
    Point3Float p3 = { 0, 1, ((float16)(getpixel(this->heightmap, gridX, gridY + 1) & 0xff)) / this->atenuation };
    Point2Float pos = { xCoord, yCoord };

    return barycentre(p1, p2, p3, pos);
  }
  else
  {
    Point3Float p1 = { 1, 0, ((float16)(getpixel(this->heightmap, gridX + 1, gridY) & 0xff)) / this->atenuation };
    Point3Float p2 = { 1, 1, ((float16)(getpixel(this->heightmap, gridX + 1, gridY + 1) & 0xff)) / this->atenuation };
    Point3Float p3 = { 0, 1, ((float16)(getpixel(this->heightmap, gridX, gridY + 1) & 0xff)) / this->atenuation };
    Point2Float pos = { xCoord, yCoord };

    return barycentre(p1, p2, p3, pos);
  }

  /*
  // algo de David

  if (x >= this->largeur || y >= this->hauteur || x < 0 || y < 0)
  {
    return 0;
  }

  //printf("x1:%f x2:%f\n", this->hauteurs[x][y], ((float16) (getpixel(this->heightmap, x, y) & 0xff)));

  float16 a = (((float16)(getpixel(this->heightmap, x, y) & 0xff)) / this->atenuation);
  float16 b = (((float16)(getpixel(this->heightmap, x+1, y) & 0xff)) / this->atenuation);
  float16 c = (((float16)(getpixel(this->heightmap, x, y+1) & 0xff)) / this->atenuation);
  float16 d = (((float16)(getpixel(this->heightmap, x+1, y+1) & 0xff)) / this->atenuation);
  float16 e = a + (b - a) * (x - (uint32) x);
  float16 f = c + (d - c) * (x - (uint32) x);
  float16 g = e + (f - e) * (y - (uint32) y);
  return g;
  */
}
