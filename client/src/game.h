#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <SDL2/SDL.h>
#include "types.h"
#include "iniFile.h"

class Game
{
  public:
    Game(void);
    ~Game(void);
    bool8 run(void);

  private:
    void initSDL(void);
    void initSDL_ttf(void);
    bool8 createOpenGLWindow(void);
    void destroyOpengGLWindow(void);

    SDL_Window* window;
    SDL_GLContext ctx_opengl;
    uint32 width;
    uint32 height;

    INIFile configurationFile;
};

#endif // GAME_H_INCLUDED
