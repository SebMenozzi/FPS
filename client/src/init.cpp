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


/*void initSDL(void)
{
  // Demarrage de la SDL avec le module video
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  // Lecture des dimensions maximales de l'�cran
  const SDL_VideoInfo* informationsVideo;
  informationsVideo = SDL_GetVideoInfo();
  uint32 largeurMax = informationsVideo->current_w;
  uint32 hauteurMax = informationsVideo->current_h;

  // Creation de la fenetre initialisee pour fonctionner avec OpenGL
  SDL_SetVideoMode(LARGEUR_FENETRE, HAUTEUR_FENETRE, 32, SDL_OPENGL);

  // Titre de l'application
  SDL_WM_SetCaption(TITRE_APPLICATION, NULL);
}*/



void initOpenGL(void)
{
  //glEnable( GL_CULL_FACE ); // Activer le masquage des faces cachees
  glCullFace(GL_BACK); // Face cachees = faces arrieres
  glFrontFace(GL_CCW); // Face avant = sens trigo

  // Couleur rgba de vidage de l'ecran
  glClearColor(0, 0, 0, 0);

  // Definition de la fenetre
  glViewport(0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE);

  // Definition de la zone visible
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(ANGLE_VISION, (GLdouble) LARGEUR_FENETRE / (GLdouble)HAUTEUR_FENETRE, PRET, LOIN);

  // Activation du tampon de profondeur
  glEnable(GL_DEPTH_TEST);
}
