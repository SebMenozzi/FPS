#include <SDL2/SDL.h>
#include "imageOverlay.h"
#include "../configuration.h"

ImageOverlay::ImageOverlay(SDL_Window* fenetre, std::string image, sint32 x, sint32 y) : WidgetOverlay(fenetre, x, y)
{
  this->textureImage = 0;

  this->fichierImage = image;

  this->genererTexture();
  //this->conteneurTextures.ajouter(this->fichierImage);

  //this->_largeur = this->conteneurTextures.texture(this->fichierImage).largeur;
  //this->_hauteur = this->conteneurTextures.texture(this->fichierImage).hauteur;
}

ImageOverlay::~ImageOverlay(void)
{
  glDeleteTextures(1, &this->textureImage);
}

void ImageOverlay::genererTexture(void)
{
  // Creation de l'image
  SDL_Surface* image24Bits;
  SDL_Surface* image;
  GLenum formatTexture;
  GLint  octetsParPixel;
  Uint32 rmask, gmask, bmask, amask;

  // Creation de la surface SDL a partir du fichier de la texture
  image24Bits = SDL_LoadBMP(this->fichierImage.c_str());

  Uint32 pixelMagenta = SDL_MapRGB(image24Bits->format, 255, 0, 255);
  SDL_SetColorKey(image24Bits, SDL_TRUE, pixelMagenta);

  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
  #else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
  #endif

  SDL_PixelFormat format = *(image24Bits->format);
  format.BitsPerPixel = 32;
  format.BytesPerPixel = 4;
  format.Rmask = rmask;
  format.Gmask = gmask;
  format.Bmask = bmask;
  format.Amask = amask;

  image = SDL_ConvertSurface(image24Bits,&format,SDL_SWSURFACE);

  if (NULL != image)
  {
    // Verification de la largeur
    if ( 0 != (image->w & (image->w - 1)) )
    {
      printf("Attention : la largeur de %s n'est pas une puissance de 2\n ", this->fichierImage.c_str());
    }

    // Verification de la hauteur
    if ( 0 != (image->h & (image->h - 1)) )
    {
      printf("Attention : la hauteur de %s n'est pas une puissance de 2\n ", this->fichierImage.c_str());
    }

    // Recuperation du nombre d'octets par pixel
    octetsParPixel = image->format->BytesPerPixel;

    if (octetsParPixel == 4) // Contient une composante alpha
    {
      if (image->format->Rmask == 0x000000ff)
      {
        formatTexture = GL_RGBA;
      }
      else
      {
        #ifndef GL_BGRA
          #define GL_BGRA 0x80E1
        #endif
        formatTexture = GL_BGRA;
      }
    }
    else if (octetsParPixel == 3) // Pas de composante alpha
    {
      if (image->format->Rmask == 0x000000ff)
      {
        formatTexture = GL_RGB;
      }
      else
      {
        #ifndef GL_BGR
          #define GL_BGR 0x80E0
        #endif
        formatTexture = GL_BGR;
      }
    }
    else
    {
      printf("Attention : l'image n'est pas en couleur vraie\n");
    }

    // Generation de la texture
    glGenTextures(1, &this->textureImage);

    // Selection de la texture
    glBindTexture(GL_TEXTURE_2D, this->textureImage);

    // Initialisation des proprietes de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Rend les arretes invisibles
    #ifndef GL_CLAMP_TO_EDGE
      #define GL_CLAMP_TO_EDGE (0x812F)
    #endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Activation du MIP mapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Chargement de la texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, octetsParPixel, image->w, image->h, formatTexture,GL_UNSIGNED_BYTE, image->pixels);//AJ

    // Recuperation de la taille de la texture
    this->_largeur = image->w;
    this->_hauteur = image->h;
  }
  else
  {
    printf("SDL ne peut pas charger l'image %s : %s\n", this->fichierImage.c_str(), SDL_GetError());
    this->textureImage = 0;
    this->_largeur = 1;
    this->_hauteur = 1;
  }

  // Liberation de l'image du texte du bouton
  if (NULL != image)
  {
    SDL_FreeSurface(image);
  }
}

void ImageOverlay::dessiner(void)
{
  // Si l'image est visible
  if (TRUE == this->visible)
  {
    // Activation de la transparence
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glAlphaFunc(GL_ALWAYS, 0.0);

    glBindTexture(GL_TEXTURE_2D, this->textureImage);

    // Application de l'image de fond
    glBegin(GL_QUADS);
      glTexCoord2i(0, 0); glVertex2i( this->_positionX - (this->_largeur/2), this->hauteurFenetre - this->_positionY + (this->_hauteur/2));
      glTexCoord2i(0, 1); glVertex2i( this->_positionX - (this->_largeur/2), this->hauteurFenetre - this->_positionY - (this->_hauteur/2));
      glTexCoord2i(1, 1); glVertex2i( this->_positionX + (this->_largeur/2), this->hauteurFenetre - this->_positionY - (this->_hauteur/2));
      glTexCoord2i(1, 0); glVertex2i( this->_positionX + (this->_largeur/2), this->hauteurFenetre - this->_positionY + (this->_hauteur/2));
    glEnd();
  }
}
