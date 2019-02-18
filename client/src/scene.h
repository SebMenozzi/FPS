#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include "types.h"
#include <vector>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "objet3DStatique.h"
#include "map.h"
#include "heightmap.h"
#include "player.h"
#include "clock.h"
#include "udpClient.h"

class Scene
{
  private:
    //typedef std::map<std::string, GLuint> Textures;
    SDL_Window* window;
    int width;
    int height;

    Clock clock;
    UDPClient* udpClient;

    bool8 quit;
    uint32 timeLastLoop;

    Objet3DStatique* skybox;
    Map* map;
    Heightmap* heightmap;

    void handleEvents(void);
    void animate(void);
    void draw(void);
    void display(void);
    void draw2D(void);

    //Personnage* personnage;
    std::vector<Player*> players;
    uint32 playerId;

    sint32 currentTime;

  public:
    Scene(SDL_Window* window);
    ~Scene();
    void run();
    void setUDPClient(UDPClient* udpClient);
    void setClock(sint32 time);
    void createPlayer();
    void setPlayerId(uint32 playerId);
};

#endif
