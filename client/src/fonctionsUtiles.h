#ifndef FONCTIONSUTILES_H_INCLUDED
#define FONCTIONSUTILES_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <SDL2/SDL.h>
#include "types.h"
#include "point3Float.h"
#include "point2Float.h"

std::string decapsuler(std::string* message);
sint32 stringEnSint32(std::string chaine);
uint32 stringEnUint32(std::string chaine);
float16 stringEnFloat16(std::string chaine);
std::string sint32EnString(sint32 valeur);
Uint32 get_pixel(SDL_Surface *surface, int x, int y);
float16 barycentre(Point3Float p1, Point3Float p2, Point3Float p3, Point2Float pos);
char* createPath(char* directory, char* name);

#endif // FONCTIONSUTILES_H_INCLUDED
