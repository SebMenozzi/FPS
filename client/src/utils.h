#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <SDL2/SDL.h>
#include "types.h"
#include "point3Float.h"
#include "point2Float.h"

std::string decapsulate(std::string* message);
sint32 stringToSint32(std::string string);
uint32 stringToUint32(std::string string);
float16 stringToFloat16(std::string string);
std::string sint32ToString(sint32 value);
Uint32 getPixel(SDL_Surface *surface, int x, int y);
float16 barycenter(Point3Float p1, Point3Float p2, Point3Float p3, Point2Float pos);

#endif // UTILS_H_INCLUDED
