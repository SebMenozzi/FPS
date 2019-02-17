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
#include "personnage.h"
#include "clock.h"
#include "clientUDP.h"
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
    ClientUDP* clientUDP;

    bool8 cont;
    uint32 time_last_move;

    Objet3DStatique* skybox;
    Map* map;
    Heightmap* heightmap;

    TTF_Font* font;
    std::string displayed_text;

    void handle_events(void);
    void animer(void);
    void dessiner(void);
    void dessinerTerrain(void);
    void afficher(void);
    void dessiner2D(void);

    //Personnage* personnage;
    std::vector<Personnage*> personnages;
    uint32 player_id;

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
    Scene(SDL_Window* fenetre);
    ~Scene();
    void run();
    void clientUDPAUtiliser(ClientUDP* clientUDP);
    void set_clock(sint32 heure);
    void create_player();
    void set_player_id(uint32 player_id);
    void set_endtime(sint32 heureFin);
    void creerTableauScores(uint32 nbJoueurs);
    void set_usernames(std::vector<std::string> listePseudosJoueurs);
};

#endif
