#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include <set>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#include <string>
#include "conteneurTextures.h"
#include "types.h"

class Map
{
  private:
    ConteneurTextures conteneurTextures;

    bool8* map;
    uint32 width;
    uint32 height;
    GLuint draw_list;

    void create_draw_list();

  public:
    Map(const char* name);
    ~Map();
    void draw();

    void contourn(sint32 x, sint32 y, bool8 entourage[8]);
};

#endif
