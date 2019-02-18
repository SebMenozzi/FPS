#ifndef TEXTURES_CONTAINER_H_INCLUDED
#define TEXTURES_CONTAINER_H_INCLUDED

#include "types.h"
#include <map>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#include <string>
#include "utils.h"

class TexturesContainer
{
  private:
    struct Texture
    {
      GLuint texture;
      uint32 counter;
      sint32 width;
      sint32 height;
      float16 largeurPortionUtile;
      float16 hauteurPortionUtile;
    };

    typedef std::map<std::string, Texture> Textures;

    static Textures textures;
    void loadTexture(std::string file, GLuint* texture, sint32* width, sint32* height);

  public:
    Texture addTexture(std::string file);
    void deleteTexture(std::string file);
    Texture getTexture(std::string file);
};

#endif // TEXTURES_CONTAINER_H_INCLUDED
