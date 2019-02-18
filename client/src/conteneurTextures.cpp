#include "conteneurTextures.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>

ConteneurTextures::Textures ConteneurTextures::textures;

ConteneurTextures::Texture ConteneurTextures::ajouter(std::string nomFichier)
{
  Texture& texture = this->textures[nomFichier];

  // Si la texture n'existe pas
  if (0 == texture.compteur)
  {
    // Sinon on cree reellement la texture
    this->chargerTexture(nomFichier, &texture.texture, &texture.largeur, &texture.hauteur);
  }

  // On compte une texture supplementaire
  texture.compteur++;

  return texture;
}

void ConteneurTextures::supprimer(std::string nomFichier)
{
  // Recuperation de la texture
  Texture texture = this->textures[nomFichier];

  // Si la texture existe
  if (0 != texture.compteur)
  {
    texture.compteur--;
  }

  // Si la texture n'est plus utilisee
  if (0 == texture.compteur)
  {
    // Liberation de la texture
    glDeleteTextures(1, &texture.texture);
    // Suppression de la case de la texture
    this->textures.erase(nomFichier);
  }
}

ConteneurTextures::Texture ConteneurTextures::texture(std::string nomFichier)
{
  return this->textures[nomFichier];
}

void ConteneurTextures::chargerTexture(std::string nomFichier, GLuint* texture, sint32* largeur, sint32* hauteur)
{
  SDL_Surface *surface;
  GLenum formatTexture;
  GLint  octetsParPixel;

  // Creation de la surface SDL a partir du fichier de la texture
  //surface = SDL_LoadBMP(nomFichier);
  std::string fullpath = "assets/" + nomFichier;

  surface = IMG_Load(fullpath.c_str());

  if (NULL != surface)
  {

    // Verification de la largeur
    if ( 0 != (surface->w & (surface->w - 1)) )
    {
      printf("Attention : la largeur de %s n'est pas une puissance de 2\n ", nomFichier.c_str());
    }

    // Verification de la hauteur
    if ( 0 != (surface->h & (surface->h - 1)) )
    {
      printf("Attention : la hauteur de %s n'est pas une puissance de 2\n ", nomFichier.c_str());
    }

    // Recuperation du nombre d'octets par pixel
    octetsParPixel = surface->format->BytesPerPixel;

    if (octetsParPixel == 4) // Contient une composante alpha
    {
      if (surface->format->Rmask == 0x000000ff)
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
      if (surface->format->Rmask == 0x000000ff)
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
      SDL_FreeSurface(surface);
      return;
    }

    // Activation des textures
    glEnable(GL_TEXTURE_2D);

    // Generation de la texture
    glGenTextures(1, texture);

    // Selection de la texture
    glBindTexture(GL_TEXTURE_2D, *texture);

    // Initialisation des propri�t�s de la texture
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

    // Recuperation de la taille de la texture
    *largeur = surface->w;
    *hauteur = surface->h;

    // Chargement de la texture
    //glTexImage2D(GL_TEXTURE_2D, 0, 4, largeurTexture, hauteurTexture, 0, formatTexture, GL_UNSIGNED_BYTE, NULL);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->w, surface->h, formatTexture, GL_UNSIGNED_BYTE, surface->pixels);
    gluBuild2DMipmaps(GL_TEXTURE_2D, octetsParPixel, surface->w, surface->h, formatTexture,GL_UNSIGNED_BYTE, surface->pixels);//AJ

    // Liberation de la surface SDL de la texture
    SDL_FreeSurface(surface);
  }
  else
  {
    printf("SDL ne peut pas charger l'image %s : %s\n", nomFichier.c_str(), SDL_GetError());
    *texture = 0;
    *largeur = 1;
    *hauteur = 1;
  }
}
