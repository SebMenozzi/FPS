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
#include "carte.h"
#include "heightmap.h"
#include "personnage.h"
#include "horloge.h"
#include "clientUDP.h"
#include "gui/overlay.h"
#include "gui/imageOverlay.h"
#include "gui/etiquetteOverlay.h"

class Scene
{
  private:
    //typedef std::map<std::string, GLuint> Textures;
    SDL_Window* fenetre;
    int largeurFenetre;
    int hauteurFenetre;

    Horloge horloge;
    ClientUDP* clientUDP;

    bool8 continuer;
    uint32 tempsDernierPas;

    Objet3DStatique* skybox;
    Carte* carte;
    Heightmap* heightmap;

    TTF_Font* police;
    std::string texteAAfficher;

    void gererEvenements(void);
    void animer(void);
    void dessiner(void);
    void dessinerTerrain(void);
    void afficher(void);
    void dessiner2D(void);

    //Personnage* personnage;
    std::vector<Personnage*> personnages;
    uint32 numeroJoueur;

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
    void executer();
    void clientUDPAUtiliser(ClientUDP* clientUDP);
    void reglerHorloge(sint32 heure);
    void creerPersonnage();
    void reglerNumeroJoueur(uint32 numeroJoueur);
    void reglerHeureFinPartie(sint32 heureFin);
    void creerTableauScores(uint32 nbJoueurs);
    void reglerPseudosJoueurs(std::vector<std::string> listePseudosJoueurs);
};

#endif // SCENE_H_INCLUDED
