#include "fonctionsUtiles.h"
#include <SDL2/SDL.h>
#include <sstream>

std::string decapsuler(std::string* message)
{
  std::string entete;

  // Recherche de la position de la premiere espace
  size_t finEntete = message->find(' ');

  // S'il n'y a pas d'espace
  if (std::string::npos == finEntete)
  {
    // L'entete est le message lui meme
    entete = *message;

    // Le message n'encapsule rien. Il devient vide
    message->clear();

    return entete;
  }

  // Recuperation de l'entete
  entete = message->substr(0, finEntete);

  // Suppression de l'entete au message
  *message = message->substr(finEntete+1);

  return entete;
}

sint32 stringEnSint32(std::string chaine)
{
  std::stringstream flux;
  flux << chaine;
  sint32 valeur;
  flux >> valeur;

  return valeur;
}

uint32 stringEnUint32(std::string chaine)
{
  std::stringstream flux;
  flux << chaine;
  uint32 valeur;
  flux >> valeur;

  return valeur;
}

float16 stringEnFloat16(std::string chaine)
{
  std::stringstream flux;
  flux << chaine;
  float16 valeur;
  flux >> valeur;

  return valeur;
}

std::string sint32EnString(sint32 valeur)
{
  std::stringstream flux;
  flux << valeur;
  std::string chaine;
  flux >> chaine;

  return chaine;
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp) {
  case 1:
    return *p;
    break;

  case 2:
    return *(Uint16 *)p;
    break;

  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[0] << 16 | p[1] << 8 | p[2];
    else
      return p[0] | p[1] << 8 | p[2] << 16;
    break;

  case 4:
    return *(Uint32 *)p;
    break;

  default:
    return 0; /* shouldn't happen, but avoids warnings */
  }
}

float16 barycentre(Point3Float p1, Point3Float p2, Point3Float p3, Point2Float pos)
{
  float16 det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float16 l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float16 l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float16 l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}
