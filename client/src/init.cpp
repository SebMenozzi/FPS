#include "init.h"
#include "configuration.h"
#include <SDL2/SDL.h>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#include "types.h"

void init_openGL(void)
{
  glClearColor(0, 0, 0, 0);

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(ANGLE_VISION, (GLdouble) WINDOW_WIDTH / (GLdouble) WINDOW_HEIGHT, PRET, LOIN);

  glEnable(GL_DEPTH_TEST);
}
