#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED


#include "types.h"
#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include "objet3DStatique.h"
#include "carte.h"
#include "personnage.h"
#include "serveurUDP.h"
#include "horloge.h"
#include "ficheClient.h"
#include <queue>
#include "tir.h"
#include "overlay.h"
#include "etiquetteOverlay.h"

class Scene
{

    private:
        //typedef std::map<std::string, GLuint> Textures;
        ServeurUDP* serveurUDP;

        Horloge horloge;

        bool8 continuer;
        uint32 tempsDernierPas;

        Objet3DStatique* skybox;
        Carte* carte;

        void gererEvenements(void);
        void animer(void);
        void traiterLesTirs(void);
        void dessiner(void);
        void afficher(void);



        //Personnage* personnage;
        std::vector<Personnage*> personnages;
        std::vector<FicheClient> fichesClients;

        //HistoriqueDesPositions historiqueDuPersonnage;

        sint32 heureFinPartie;

        std::queue<Tir> tirsATraiter;

        Overlay overlay;
        EtiquetteOverlay* etiquetteTempsRestant;
        std::vector<EtiquetteOverlay*> etiquettesScores;

        sint32 heureDernierEnvoiReapparitions;

        sint32 heureActuelle;

    public:

        Scene();
        ~Scene();
        void executer();
        void serveurUDPAUtiliser(ServeurUDP* serveurUDP);
        void reglerHorloge(sint32 heure);
        void creerPersonnages(uint32 quantite);
        void creerTableauScores(uint32 quantite);
        void creerFichesClients(std::vector<FicheClient> ficheClients);
        void reglerHeureFinPartie(sint32 heureFinPartie);
};

#endif // SCENE_H_INCLUDED

