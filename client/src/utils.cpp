#include "utils.h"
#include <SDL2/SDL.h>
#include <sstream>

std::string decapsulate(std::string* message)
{
  std::string header;

  size_t endHeader = message->find(' ');

  // If there is not space
  if (std::string::npos == endHeader) {
    // The header is the message it self
    header = *message;

    // The message is then clear because it doesn't encapsulate anything
    message->clear();

    return header;
  }

  // Retrieve the header
  header = message->substr(0, endHeader);

  // delete header to the message
  *message = message->substr(endHeader + 1);

  return header;
}

sint32 stringToSint32(std::string string)
{
  std::stringstream flux;
  flux << string;
  sint32 value;
  flux >> value;

  return value;
}

uint32 stringToUint32(std::string string)
{
  std::stringstream flux;
  flux << string;
  uint32 value;
  flux >> value;

  return value;
}

float16 stringToFloat16(std::string string)
{
  std::stringstream flux;
  flux << string;
  float16 value;
  flux >> value;

  return value;
}

std::string sint32ToString(sint32 value)
{
  std::stringstream flux;
  flux << value;
  std::string string;
  flux >> string;

  return string;
}

Uint32 getPixel(SDL_Surface *surface, int x, int y)
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

// https://en.wikipedia.org/wiki/Barycentric_coordinate_system
float16 barycenter(Point3Float p1, Point3Float p2, Point3Float p3, Point2Float pos)
{
  float16 det = (p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y);
	float16 l1 = ((p2.y - p3.y) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.y)) / det; // https://wikimedia.org/api/rest_v1/media/math/render/svg/dd9199d9fee273a1c2cc7f208ada749f46cec051
	float16 l2 = ((p3.y - p1.y) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.y)) / det; // https://wikimedia.org/api/rest_v1/media/math/render/svg/544fbe020e93b8fcd52c79e41b41a376cf92f2ab
	float16 l3 = 1.0f - l1 - l2; // https://wikimedia.org/api/rest_v1/media/math/render/svg/0c9dc80639f0e46b1cea744cf3ce90ba406ff352
	return l1 * p1.z + l2 * p2.z + l3 * p3.z;
}
