#include "texturesContainer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>

TexturesContainer::Textures TexturesContainer::textures;

TexturesContainer::Texture TexturesContainer::addTexture(std::string file)
{
  Texture& texture = this->textures[file];

  // If the texture doesn't exist
  if (0 == texture.counter) {
    // Create the texture
    this->loadTexture(file, &texture.texture, &texture.width, &texture.height);
  }

  texture.counter++;

  return texture;
}

void TexturesContainer::deleteTexture(std::string file)
{
  // Retrieve the texture
  Texture texture = this->textures[file];

  // If the texture exists
  if (0 != texture.counter) {
    texture.counter--;
  }

  // If the texture is not used anymore
  if (0 == texture.counter) {
    // Free the texture
    glDeleteTextures(1, &texture.texture);

    this->textures.erase(file);
  }
}

TexturesContainer::Texture TexturesContainer::getTexture(std::string file)
{
  return this->textures[file];
}

void TexturesContainer::loadTexture(std::string file, GLuint* texture, sint32* width, sint32* height)
{
  SDL_Surface *surface;
  GLenum textureFormat;
  GLint bytesPerPixel;

  std::string fullpath = "assets/" + file;
  surface = IMG_Load(fullpath.c_str());

  if (NULL != surface) {
    /*
    // Verification de la largeur
    if (0 != (surface->w & (surface->w - 1))) {
      printf("Attention : la largeur de %s n'est pas une puissance de 2\n ", file.c_str());
    }

    // Verification de la hauteur
    if ( 0 != (surface->h & (surface->h - 1)) ) {
      printf("Attention : la hauteur de %s n'est pas une puissance de 2\n ", file.c_str());
    }
    */

    bytesPerPixel = surface->format->BytesPerPixel;

    if (bytesPerPixel == 4) { // alpha composant is used (rgba)
      if (surface->format->Rmask == 0x000000ff) {
        textureFormat = GL_RGBA;
      } else {
        #ifndef GL_BGRA
          #define GL_BGRA 0x80E1
        #endif
        textureFormat = GL_BGRA;
      }
    } else if (bytesPerPixel == 3) { // no alpha composant (rgb)
      if (surface->format->Rmask == 0x000000ff) {
        textureFormat = GL_RGB;
      } else {
        #ifndef GL_BGR
          #define GL_BGR 0x80E0
        #endif
        textureFormat = GL_BGR;
      }
    } else {
      SDL_FreeSurface(surface);
      return;
    }

    // Activation des textures
    glEnable(GL_TEXTURE_2D);

    // Generation de la texture
    glGenTextures(1, texture);

    // Selection de la texture
    glBindTexture(GL_TEXTURE_2D, *texture);

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

    // Recuperation de la taille de la texture
    *width = surface->w;
    *height = surface->h;

    // Chargement de la texture
    //glTexImage2D(GL_TEXTURE_2D, 0, 4, largeurTexture, hauteurTexture, 0, formatTexture, GL_UNSIGNED_BYTE, NULL);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->w, surface->h, formatTexture, GL_UNSIGNED_BYTE, surface->pixels);
    gluBuild2DMipmaps(GL_TEXTURE_2D, bytesPerPixel, surface->w, surface->h, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);//AJ

    // Liberation de la surface SDL de la texture
    SDL_FreeSurface(surface);
  } else {
    printf("SDL cannot load the image %s : %s\n", file.c_str(), SDL_GetError());
    *texture = 0;
    *width = 1;
    *height = 1;
  }
}
