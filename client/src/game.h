#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <SDL2/SDL.h>
#include "types.h"
#include "fichierINI.h"

class Game
{
  public:
    Game(void);
    ~Game(void);
    bool8 run(void);

  private:
    void init_SDL(void);
    void init_SDL_ttf(void);
    bool8 create_opengl_window(void);
    void destroy_opengl_window(void);

    // Definition de la fenetre
    SDL_Window* window;
    SDL_GLContext ctx_opengl;
    uint32 width;
    uint32 height;

    // Fichier de configuration
    FichierINI init_file;
};

#endif // JEU_H_INCLUDED
