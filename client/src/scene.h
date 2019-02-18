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
#include "gui/overlay.h"
#include "gui/imageOverlay.h"
#include "gui/etiquetteOverlay.h"

class Scene
{
  private:
    //typedef std::map<std::string, GLuint> Textures;
    SDL_Window* window;
    int width;
    int height;

    Clock clock;
    UDPClient* udpClient;

    bool8 cont;
    uint32 timeLastMove;

    Objet3DStatique* skybox;
    Map* map;
    Heightmap* heightmap;

    TTF_Font* font;
    std::string displayed_text;

    void handle_events(void);
    void animate(void);
    void draw(void);
    void display(void);
    void draw2D(void);

    //Personnage* personnage;
    std::vector<Player*> players;
    uint32 playerId;

    sint32 heureFinPartie;

    Overlay* overlay;
    ImageOverlay* viseur;
    ImageOverlay* fontProchaineApparition;
    ImageOverlay* fondTableauScores;
    EtiquetteOverlay* etiquetteProchaineApparition;
    EtiquetteOverlay* etiquetteTempsRestant;

    sint32 heureActuelle;

    std::vector<EtiquetteOverlay*> etiquettesScoresPseudo;
    std::vector<EtiquetteOverlay*> etiquettesScoresEmis;
    std::vector<EtiquetteOverlay*> etiquettesScoresRecu;

  public:
    Scene(SDL_Window* window);
    ~Scene();
    void run();
    void setUDPClient(UDPClient* udpClient);
    void setClock(sint32 time);
    void createPlayer();
    void setPlayerId(uint32 playerId);
    //void setEndtime(sint32 heureFin);
    //void creerTableauScores(uint32 nbJoueurs);
    void set_usernames(std::vector<std::string> listePseudosJoueurs);
};

#endif
