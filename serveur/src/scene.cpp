#include "scene.h"
#include "configuration.h"
#include <sstream>
#include <iomanip>
#include "fonctionsUtiles.h"

#ifndef INT_MIN
  #define INT_MIN ((int) 0x80000000)
#endif

Scene::Scene()
{
  // Reglage de l'horloge
  //this->horloge.regler(0);

  //this->serveurUDP = new ServeurUDP(2712);

  this->heureDernierEnvoiReapparitions = INT_MIN;
}

Scene::~Scene()
{
    delete this->skybox;
    delete this->carte;

    delete this->etiquetteTempsRestant;
    /*delete this->etiquettesScores[0];
    delete this->etiquettesScores[1];
    delete this->etiquettesScores[2];*/

    // Destruction des personnages

    // Destruction des scores
    for(uint32 i = 0; i < this->etiquettesScores.size(); i++)
    {
        delete this->etiquettesScores[i];
    }
}

void Scene::executer()
{
    // Initialisation des variables de gestion du temps
    this->tempsDernierPas = 0;
    uint32 heureDernierPas = SDL_GetTicks();

    this->continuer = TRUE;

    while(this->continuer)
    {
        // Definition de l'heure actuelle pour la serie de calcule
        this->heureActuelle = horloge.heure();

        gererEvenements();
        animer();
        traiterLesTirs();
        dessiner();
        afficher();

        // On calcule le temps de construction de la derniere image
        this->tempsDernierPas = SDL_GetTicks() - heureDernierPas;
        heureDernierPas += this->tempsDernierPas;

        // Detection de la fin de la partie
        if (this->heureActuelle >= this->heureFinPartie)
        {
            this->continuer = FALSE;

            // Construction du message des scores
            std::stringstream message;
            message << "SCORES";
            for (uint32 i = 0; i < this->personnages.size(); i++)
            {
                message << " " << this->personnages[i]->emis() << " " << this->personnages[i]->recu();
            }

            // Envoi des scores aux clients, 3 fois, espaces de 500 ms
            for(uint32 j = 0; j < 3; j++)
            {
                for(uint32 i = 0; i < this->fichesClients.size(); i++)
                {
                    this->serveurUDP->envoyer(message.str(), this->fichesClients[i].ip, this->fichesClients[i].port);
                }

                SDL_Delay(500);
            }

            // TODO : Ecriture du score dans un fichier
        }
    }

}

void Scene::animer(void)
{
    // Lecture de l'�tat des touches
    /*int nombreDeTouches;
    Uint8* pTouches = SDL_GetKeyState(&nombreDeTouches);
    Uint8* touches = new Uint8[nombreDeTouches];
    memcpy(touches, pTouches, nombreDeTouches);

    // Evite les deplacements absurdes
    if (touches[SDLK_w] == touches[SDLK_s]) // Avancer et reculer
    {
        touches[SDLK_w] = 0u; // Ici, on travaille sur
        touches[SDLK_s] = 0u; // une copie des touches
    }
    if (touches[SDLK_a] == touches[SDLK_d]) // Gauche et droite
    {
        touches[SDLK_a] = 0u;
        touches[SDLK_d] = 0u;
    }

    float16 direction;
    bool8 deplacement = FALSE;

    if (touches[SDLK_b]) // Touche B
    {
        this->personnage->positionner(5.5, 5.5);
        this->personnage->orienter(270.0);
    }

    if (touches[SDLK_w]) // Touche Z
    {
        // Avancer

        deplacement = TRUE;

        // En diagonale
        if (touches[SDLK_a]) direction = 45.0;
        else if (touches[SDLK_d]) direction = -45.0;

        // Droit
        else direction = 0.0;
    }
    else if (touches[SDLK_s]) // Touche S
    {
        // Reculer

        deplacement = TRUE;

        // En diagonale
        if (touches[SDLK_a]) direction = 135.0;
        else if (touches[SDLK_d]) direction = -135.0;

        // Droit
        else direction = 180.0;
    }

    if(FALSE == deplacement)
    {
        if (touches[SDLK_a]) // Touche Q
        {
            // Gauche
            direction = 90.0;
            deplacement = TRUE;
        }
        else if (touches[SDLK_d]) // Touche D
        {
            // Droite
            direction = -90.0;
            deplacement = TRUE;
        }
    }

    delete[] touches;

    // Si un deplacement est demande
    if (deplacement)
    {
        #define VITESSE_DEPLACEMENT (5.0f)

        // Calcule de la distance � parcourir
        float16 distance = (float)tempsDernierPas * VITESSE_DEPLACEMENT / 1000.0f;

        // Recuperation de l'environnement
        sint32 positionCarteX = 0, positionCarteY = 0;
        bool8 entouragePersonnage[8];
        this->personnage->positionSurLaCarte(&positionCarteX, &positionCarteY);
        this->carte->entourage(positionCarteY, positionCarteX, entouragePersonnage);

        // Deplacement du personnage dans la direction demande
        this->personnage->deplacer(distance, direction, entouragePersonnage);
    }*/

    std::string messageRecu;
    std::string ipExpediteur;
    uint16 portExpediteur;

    do
    {
        messageRecu = this->serveurUDP->recevoir(&ipExpediteur, &portExpediteur);

        std::string message = messageRecu;


        // Si un message est recu
        if (messageRecu != "")
        {
            // Si c'est une position de joueur
            std::string entete = decapsuler(&message);

            if ("POSITION_JOUEUR" == entete)
            {
                //std::stringstream flux;
                //flux << messageRecu;

                // Lecture de la position du joueur
                sint32 heure = stringEnSint32(decapsuler(&message));
                float16 positionX = stringEnFloat16(decapsuler(&message));
                float16 positionY = stringEnFloat16(decapsuler(&message));
                float16 angle = stringEnFloat16(decapsuler(&message));

                // Identification du personnage
                for(uint32 i = 0; i < this->fichesClients.size(); i++)
                {
                    if (ipExpediteur == this->fichesClients[i].ip && portExpediteur == this->fichesClients[i].port)
                    {
                        // Ajout de la position � l'historique des position de ce joueur
                        this->personnages[i]->ajouterPosition(heure, positionX, positionY, angle);
                        break;
                    }
                }
            }

            // Un message de tir est recu
            else if ("TIR" == entete)
            {
                // Creation du tir
                Tir tir;
                tir.ip = ipExpediteur;
                tir.port = portExpediteur;
                tir.heure = stringEnSint32(decapsuler(&message));

                // Ajout du tir a la liste des tirs a traiter
                this->tirsATraiter.push(tir);
            }

            // Si un nouveau joueur cherche a se connecter
            else if ("PSEUDO" == entete)
            {
                // Une partie est en cours
                std::stringstream flux;
                flux << "PARTIE_EN_COURS " << sint32EnString((this->heureFinPartie - this->heureActuelle) / 1000);
                this->serveurUDP->envoyer(flux.str(), ipExpediteur, portExpediteur);
            }


        }
    }
    while (messageRecu != ""); // Tant qu'il y a des messages



    // Positionne les personnages a l'endroit ou ils sont a t-100ms (notre present)
    for (uint32 i = 0; i < this->personnages.size(); i++)
    {
        this->personnages[i]->positionnerDepuisLHistorique(this->heureActuelle - 100);
    }



    // Envoi des snapshots toutes les 50 ms en moyenne
    #define DUREE_ENTRE_SNAPSHOT 50
    static sint32 heureDernierEnvoiSnapshot = this->heureActuelle - DUREE_ENTRE_SNAPSHOT;
    if (this->heureActuelle >= heureDernierEnvoiSnapshot + DUREE_ENTRE_SNAPSHOT)
    {
        heureDernierEnvoiSnapshot = this->heureActuelle;

        // Construction du snapshot
        std::stringstream message;
        message << "SNAPSHOT " << this->heureActuelle;
        for(uint32 i = 0; i < this->personnages.size(); i++)
        {
            message << " " << this->personnages[i]->lirePositionX() << " " << this->personnages[i]->lirePositionY() << " " << this->personnages[i]->lireAngle();
        }

        // Envoi du snapshot aux clients
        for(uint32 i = 0; i < this->fichesClients.size(); i++)
        {
            this->serveurUDP->envoyer(message.str(), this->fichesClients[i].ip, this->fichesClients[i].port);
        }

    }

    // Envoi des heures/points de reapparition des joueurs en mode "reapparition", toutes les secondes
    #define DUREE_ENTRE_MESSAGES_DE_REAPPARITION 1000
    if (this->heureActuelle >= this->heureDernierEnvoiReapparitions + DUREE_ENTRE_MESSAGES_DE_REAPPARITION)
    {
        this->heureDernierEnvoiReapparitions = heureActuelle;

        // Pour chaque personnage
        for(uint32 numeroJoueur = 0; numeroJoueur < this->personnages.size(); numeroJoueur++)
        {
            // Si ce personnage est en mode "reapparition"
            if ( this->personnages[numeroJoueur]->heureReapparition() > this->heureActuelle )
            {
                // Recuperation du point de reapparition pour la construction du message
                Point pointReapparition = this->personnages[numeroJoueur]->pointReapparition();

                // Construction du message de reapparition
                std::stringstream message;
                message << "REAPPARITION " << numeroJoueur << " " << this->personnages[numeroJoueur]->heureReapparition() << " " << pointReapparition.x << " " << pointReapparition.y << " " << this->personnages[numeroJoueur]->angleReapparition();

                // Envoi du message de reapparition aux clients
                for(uint32 numeroClient = 0; numeroClient < this->fichesClients.size(); numeroClient++)
                {
                    this->serveurUDP->envoyer(message.str(), this->fichesClients[numeroClient].ip, this->fichesClients[numeroClient].port);
                }
            }
        }
    }


    // Si un joueur n'envoi plus ses positions depuis une seconde, on le deconnecte



    // S'il n'y a plus qu'un joueur, la partie est finie


    // Si le temps prevu pour la partie est impartit, la partie est finie




}

void Scene::dessiner(void)
{
    // D�finition de la fenetre
    glViewport(0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE);

    // D�finition de la zone visible
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(ANGLE_VISION, (GLdouble) LARGEUR_FENETRE / (GLdouble)HAUTEUR_FENETRE, PRET, LOIN);

    // Activation du tampon de profondeur
    glEnable(GL_DEPTH_TEST);


    // Vidage de l'�cran
    glClear(/*GL_COLOR_BUFFER_BIT |*/ GL_DEPTH_BUFFER_BIT);

    // Place la camera
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    gluLookAt(27,37,40,27,37,0.7,-1,0,0);
    //this->personnage->regarder();

    // Dessin de la skybox
    this->skybox->dessiner();

    // Dessin de la carte
    this->carte->dessiner();

    // Dessin du personnage
    //this->personnage->dessiner();

    // Dessin des personnages
    for(uint32 i = 0; i < this->personnages.size(); i++)
    {
        this->personnages[i]->dessiner();
    }


    // En 2D
    // Dessin de l'overlay
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);

    // Definition de la fenetre
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)LARGEUR_FENETRE, 0.0, (GLdouble)HAUTEUR_FENETRE);

    // Desactivation du test de profondeur
    glDisable(GL_DEPTH_TEST);



    // Temps restant
    uint32 tempsRestantEnSecondes = (this->heureFinPartie - this->heureActuelle) / 1000;
    uint32 minutesRestantes = tempsRestantEnSecondes / 60;
    uint32 secondesRestantes = tempsRestantEnSecondes % 60;
    std::stringstream flux;
    flux << std::setfill('0') << std::setw(2) << minutesRestantes << ":" << std::setfill('0') << std::setw(2) << secondesRestantes;
    this->etiquetteTempsRestant->modifierTexte(flux.str());


    // Mise a jour du tableau des scores
    for(uint32 i = 0; i < this->personnages.size(); i++)
    {
        flux.str("");
        flux << this->fichesClients[i].pseudo << " "
        << std::setfill('0') << std::setw(2) << this->personnages[i]->emis()
        << " " << std::setfill('0') << std::setw(2) << this->personnages[i]->recu();
        this->etiquettesScores[i]->modifierTexte(flux.str());
    }

    /*flux.str("");
    flux << this->fichesClients[1].pseudo << " "
    << std::setfill('0') << std::setw(2) << this->personnages[1]->emis()
    << " " << std::setfill('0') << std::setw(2) << this->personnages[1]->recu();
    this->etiquettesScores[1]->modifierTexte(flux.str());

    flux.str("");
    flux << this->fichesClients[2].pseudo << " "
    << std::setfill('0') << std::setw(2) << this->personnages[2]->emis()
    << " " << std::setfill('0') << std::setw(2) << this->personnages[2]->recu();
    this->etiquettesScores[2]->modifierTexte(flux.str());*/

    this->overlay.dessiner();
}

void Scene::afficher(void)
{
    glFlush();
    SDL_GL_SwapBuffers();
}

void Scene::gererEvenements(void)
{
    SDL_Event evenement;
    while (SDL_PollEvent(&evenement))
    {
        switch(evenement.type)
        {
            case SDL_QUIT:
                this->continuer = FALSE;
                break;

            /*case SDL_MOUSEMOTION:
                // Si le mouvement de la souris est physique (pas par SDL_WarpMouse)
                if ((LARGEUR_FENETRE/2) != evenement.motion.x || (HAUTEUR_FENETRE/2) != evenement.motion.y)
                {
                    // Fait tourner le personnage
                    this->personnage->tournerHorizontalement(-evenement.motion.xrel * 0.06);
                    this->personnage->tournerVerticalement(-evenement.motion.yrel * 0.06);

                    // Replace la souris au milieu de l'ecran
                    SDL_WarpMouse( (LARGEUR_FENETRE/2), (HAUTEUR_FENETRE/2) );
                }
                break;*/

            case SDL_KEYDOWN:
                // Permet de quitter grace a la touche Echap
                if (evenement.key.keysym.sym == SDLK_ESCAPE)
                {
                    this->continuer = FALSE;
                }
                break;
        }
    }
}

/*void Scene::attenteJoueurs()
{
    // Cette fonction accueil les joueurs,
    // synchronise leur horloge,
    // memorise leur pseudo, ip, port,
    // indique le temps de la partie,
    // l'heure de debut de la partie,
    // recois les messages de chat,
    // transmet l'historique du chat (5 derniers messages)
}*/

void Scene::serveurUDPAUtiliser(ServeurUDP* serveurUDP)
{
    this->serveurUDP = serveurUDP;
}

void Scene::reglerHorloge(sint32 heure)
{
    this->horloge.regler(heure);
}

void Scene::creerPersonnages(uint32 quantite)
{

    // TODO : Verifier qu'il n'y a pas deja des personnages dans la liste
    // Sinon, detruire les personnages et retailler le tableau


    #define TEMPS_AVANT_REAPPARITION_EN_MS 5000
    sint32 heureReapparition = this->horloge.heure() + TEMPS_AVANT_REAPPARITION_EN_MS;

    // Creation des personnages, pour le moment tous de la meme couleur
    for(uint32 i = 0; i < quantite; i++)
    {
        // Creation du personnage
        this->personnages.push_back(new Personnage(2, 2, 0, -130, "raphael.m3s"));

        // Recherche d'un point de reapparition
        Point pointReapparition = this->carte->unPointDeReapparitionAleatoire();

        // Angle de reapparition
        // TODO : Definir l'angle vers l'interieur de la map
        float16 angle = this->personnages[i]->lireAngle();

        // Ajout du point de reapparition a l'historique du personnage (deux fois pour eviter l'extrapolation lors de la reapparition)
        // Et -200 ms car les positions envoy� aux clients sont les positions � t-100 qui lui meme affiche des positions a t-100
        // donc il faut que le client arrive 100ms plus tot, soit 200ms dans l'historique du serveur
        this->personnages[i]->ajouterPosition(heureReapparition-201, pointReapparition.x + 0.5, pointReapparition.y + 0.5, angle);
        this->personnages[i]->ajouterPosition(heureReapparition-200, pointReapparition.x + 0.5, pointReapparition.y + 0.5, angle);

        // Le personnage passe en mode "reapparition"
        this->personnages[i]->heureReapparition(heureReapparition);
        this->personnages[i]->pointReapparition(pointReapparition);
        this->personnages[i]->angleReapparition(angle);
    }
}

void Scene::creerTableauScores(uint32 quantite)
{
    // Creation des scores
    for(uint32 i = 0; i < quantite; i++)
    {
        this->etiquettesScores.push_back( new EtiquetteOverlay(150, 120+30*i, "0 0") );
        this->overlay.ajouter(this->etiquettesScores[i]);
    }
}

void Scene::creerFichesClients(std::vector<FicheClient> fichesClients)
{
    this->fichesClients = fichesClients;
}

void Scene::reglerHeureFinPartie(sint32 heureFinPartie)
{
    this->heureFinPartie = heureFinPartie;
}

void Scene::traiterLesTirs(void)
{
    while( ! this->tirsATraiter.empty() )
    {
        Tir tir = this->tirsATraiter.front();

        // Si le tir a ete emis dernierement
        if ( tir.heure > (this->heureActuelle - 1000)  && tir.heure <= this->heureActuelle + 100 )
        {
            // Identification du tireur a partir de l'IP et du port
            uint32 tireur;
            for(tireur = 0; tireur < this->fichesClients.size(); tireur++)
            {
                if (tir.ip == this->fichesClients[tireur].ip && tir.port == this->fichesClients[tireur].port)
                {
                    // Le numero du tireur est trouve
                    break;
                }
            }

            // Repositionne les personnages tels qu'ils etaient au moment du tir sur l'ecran du tireur
            for(uint32 i = 0; i < this->personnages.size(); i++)
            {
                // Le tireur
                if (i == tireur)
                {
                    this->personnages[i]->positionnerDepuisLHistorique(tir.heure);
                }
                // Les autres personnages
                else
                {
                    // La position des personnages a l'ecran du tireur est la position telle qu'elle ete 2x100 ms plus tot
                    this->personnages[i]->positionnerDepuisLHistorique(tir.heure - 200);
                }
            }


            // TODO : Si le tireur existe
            if (1)
            {
                // D�finition de la fenetre
                glViewport(0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE);

                // D�finition de la zone visible
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(ANGLE_VISION, (GLdouble) LARGEUR_FENETRE / (GLdouble)HAUTEUR_FENETRE, PRET, LOIN);

                // Activation du tampon de profondeur
                glEnable(GL_DEPTH_TEST);

                // Vidage de l'�cran
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Place la camera
                glMatrixMode( GL_MODELVIEW );
                glLoadIdentity();

                // Regarde sous l'angle de vue du tireur
                this->personnages[tireur]->regarder();

                // Dessin de la skybox (pas necessaire)
                this->skybox->dessiner();

                // Dessin de la carte
                this->carte->dessiner();

                sint32 victime = -1;
                // Dessin des personnages
                for(uint32 i = 0; i < this->personnages.size(); i++)
                {
                    // Si ce personnage est le tireur, on passe
                    if (i == tireur)
                    {
                        continue;
                    }

                    // Si le personnage est en mode "reapparition", on passe
                    if (this->personnages[i]->heureReapparition() > this->heureActuelle)
                    {
                        continue;
                    }

                    GLfloat distancePointAvant, distancePointApres;

                    // Mesure la distance du point
                    glReadPixels (LARGEUR_FENETRE/2, HAUTEUR_FENETRE/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &distancePointAvant);

                    // Dessine le pesonnage
                    this->personnages[i]->dessiner();

                    // Remesure la distance du point
                    glReadPixels (LARGEUR_FENETRE/2, HAUTEUR_FENETRE/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &distancePointApres);

                    if (distancePointAvant != distancePointApres)
                    {
                        //this->continuer = FALSE;
                        SDL_WM_SetCaption("Touche", NULL);
                        victime = i;
                    }
                    else
                    {
                        SDL_WM_SetCaption("Pas Touche", NULL);
                    }
                }

                // S'il y a eu une victime
                if (-1 != victime)
                {
                    // Memorisation des coups emis / recu
                    this->personnages[tireur]->aTouche();
                    this->personnages[victime]->aEteTouche();

                    // Recherche d'un point de reapparition
                    Point pointReapparition = this->carte->unPointDeReapparitionAleatoire();

                    // Calcule de l'heure de reapparition
                    #define TEMPS_AVANT_REAPPARITION_EN_MS 5000
                    sint32 heureReapparition = tir.heure + TEMPS_AVANT_REAPPARITION_EN_MS;

                    // Angle de reapparition
                    float16 angle = this->personnages[victime]->lireAngle();

                    // Ajout du point de reapparition a l'historique du personnage (deux fois pour eviter l'extrapolation lors de la reapparition)
                    // Et -200 ms car les positions envoy� aux clients sont les positions � t-100 qui lui meme affiche des positions a t-100
                    // donc il faut que le client arrive 100ms plus tot, soit 200ms dans l'historique du serveur
                    this->personnages[victime]->ajouterPosition(heureReapparition-201, pointReapparition.x + 0.5, pointReapparition.y + 0.5, angle);
                    this->personnages[victime]->ajouterPosition(heureReapparition-200, pointReapparition.x + 0.5, pointReapparition.y + 0.5, angle);

                    // Le personnage passe en mode "reapparition"
                    this->personnages[victime]->heureReapparition(heureReapparition);
                    this->personnages[victime]->pointReapparition(pointReapparition);
                    this->personnages[victime]->angleReapparition(angle);

                    // Force l'envoi immediat du message de reaparition pour faire disparaitre immediatement le personnage
                    this->heureDernierEnvoiReapparitions = INT_MIN;
                }

            }
        }

        this->tirsATraiter.pop();
    }
}
