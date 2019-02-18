#ifndef HEIGHTMAP_H_INCLUDED
#define HEIGHTMAP_H_INCLUDED

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#include <iostream>
#include "texturesContainer.h"
#include "utils.h"
#include "point3Float.h"
#include "point2Float.h"
#include <vector>

class Heightmap
{
  private:
    TexturesContainer texturesContainer;
    std::vector<std::vector<float16> > heights;
    SDL_Surface* heightmap;
    uint32 height;
    uint32 width;
    float16 atenuation;

  public:
    Heightmap(const char* file);
    ~Heightmap();
    void draw();
    float16 getHeight(float16 x, float16 y);
};

#endif // HEIGHTMAP_H_INCLUDED
