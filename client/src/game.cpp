#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <sstream>
#include <iostream>

// gui
#include "gui/menu.h"
#include "gui/bouton.h"
#include "gui/zoneTexte.h"
#include "gui/etiquette.h"
#include "gui/caseACocher.h"
#include "gui/champSaisieNumerique.h"

// autres
#include "scene.h"
#include "init.h"
#include "configuration.h"
#include "clientUDP.h"
#include "horloge.h"
#include "fonctionsUtiles.h"
#include "fichierINI.h"

// Toute les secondes
#define DUREE_ENTRE_DEMANDE_JOUEURS 1000

Game::Game(void)
{
  // Initialisation des attributs
  this->width = LARGEUR_FENETRE_PAR_DEFAUT;
  this->height = HAUTEUR_FENETRE_PAR_DEFAUT;
  this->window = 0;

  // Initialisation de la SDL
  this->init_SDL();
  this->init_SDL_ttf();
}

Game::~Game(void)
{
  TTF_Quit();
  SDL_Quit();
}

// NOTE : Pour savoir ce qu'il faut mettre ici ou dans le constructeur, il faut
// se dire que le jeu peut etre execute plusieurs fois a partir de la meme instance
// Prevoir la posibilite de recharger la configuration du jeu en cas de modification
// du mode d'affichage

bool8 Game::run(void)
{
  // Chargement du fichier d'initialisation du jeu
  if (FALSE == init_file.charger("jeu.ini")) {
    std::cerr << "Erreur lors du chargement du fichier jeu.ini" << std::endl;
  }

  // Creation de la window OpenGL
  if (FALSE == this->create_opengl_window()) {
    std::cerr << "Erreur lors de la creation de la window OpenGL" << std::endl;
    return FALSE;
  }

  // Creation du menu principal
  Menu menuPrincipal(this->window, "fond_menu.bmp");

  Etiquette etiquetteServeur(this->window, this->width/2, 50, "Serveur :");
  menuPrincipal.ajouter(&etiquetteServeur);

  ZoneTexte zoneTexteServeur(this->window, this->width/2, 90);
  zoneTexteServeur.modifierTexte(init_file.lire("serveur"));
  menuPrincipal.ajouter(&zoneTexteServeur);

  Etiquette etiquettePseudo(this->window, this->width/2, 150, "Pseudo :");
  menuPrincipal.ajouter(&etiquettePseudo);

  ZoneTexte zoneTextePseudo(this->window, this->width / 2, 190);
  zoneTextePseudo.modifierTexte(init_file.lire("pseudo"));
  menuPrincipal.ajouter(&zoneTextePseudo);

  Etiquette etiquettePleinEcran(this->window, (this->width / 2) - 30, 250, "Plein ecran :");
  menuPrincipal.ajouter(&etiquettePleinEcran);

  CaseACocher casePleinEcran((this->width / 2) + 70, 250, (init_file.lire("pleinEcran") == "1") ? TRUE : FALSE);
  bool8 pleinEcranCoche = (init_file.lire("pleinEcran") == "1") ? TRUE : FALSE;
  menuPrincipal.ajouter(&casePleinEcran);

  Bouton boutonJouer(this->window, this->width / 2, 320, "Jouer");
  menuPrincipal.ajouter(&boutonJouer);

  Bouton boutonQuitter(this->window, this->width / 2, 400, "Quitter");
  menuPrincipal.ajouter(&boutonQuitter);

  menuPrincipal.dessiner();

  while (TRUE) {
    // Si la case a change d'etat
    if (pleinEcranCoche != casePleinEcran.cochee()) {
      pleinEcranCoche = casePleinEcran.cochee();

      // Met la window dans l'etat demande
      SDL_SetWindowFullscreen(this->window, (TRUE == pleinEcranCoche) ? SDL_WINDOW_FULLSCREEN : 0);
    }
    // Si on clique sur Quitter
    if (boutonQuitter.clique()) {
      break;
    }
    // Si on clique sur Jouer
    if (boutonJouer.clique()) {
      // Création du menu de Connexion
      Menu menuConnexion(this->window, "fond_menu.bmp");

      Etiquette etiquetteNbJoueurs(this->window, this->width / 2, 100, "Nombre de joueurs : 0");
      menuConnexion.ajouter(&etiquetteNbJoueurs);

      Etiquette etiquettePseudosJoueurs(this->window, this->width / 2, 150, "Liste des joueurs : EN ATTENTE...");
      menuConnexion.ajouter(&etiquettePseudosJoueurs);

      Bouton boutonSeConnecter(this->window, this->width / 2, 320, "Se connecter");
      menuConnexion.ajouter(&boutonSeConnecter);

      Bouton boutonRetour(this->window, this->width / 2, 400, "Retour");
      menuConnexion.ajouter(&boutonRetour);

      // On initialise le client UDP
      ClientUDP clientUdp;
      clientUdp.connect(zoneTexteServeur.texte(), 2712);

      // On initalise l'horloge
      Clock horlogeClient;
      horlogeClient.adjust(0);

      Clock horlogeSynchronisee;

      uint32 numeroJoueur = 0;

      std::vector<std::string> listePseudosJoueurs;

      // Traitement du pseudo
      std::string messagePseudo;
      std::string pseudo = zoneTextePseudo.texte();

      // Si le pseudo n'est pas vide
      if (pseudo != "") {
        // mettre en snake case
        for(uint32 i = 0; i < pseudo.size(); i++) {
          if (pseudo[i] == ' ' || pseudo[i] == '\t')
            pseudo[i] = '_';
        }

        // On demande la liste des joueurs
        clientUdp.envoyer("DEMANDE_JOUEURS");

        while(TRUE) {

          /*
          // On rafraichit la liste des joueurs connectés
          static sint32 heureDernierEnvoi = horlogeSynchronisee.heure() - DUREE_ENTRE_DEMANDE_JOUEURS;

          if (horlogeSynchronisee.heure() >= heureDernierEnvoi + DUREE_ENTRE_DEMANDE_JOUEURS)
          {
            heureDernierEnvoi = horlogeSynchronisee.heure();

            // On demande la liste des joueurs
            clientUdp.envoyer("DEMANDE_JOUEURS");
          }
          */

          std::string messageRecu = clientUdp.recevoir();

          // Si on a recu un message
          if(messageRecu != "") {
            // Lecture de l'entete
            std::string entete = decapsuler(&messageRecu);

            if (entete == "REPONSE_HEURE") {
              // Lecture de l'heure serveur
              std::string heureDemande = decapsuler(&messageRecu);
              std::string heureRecu = decapsuler(&messageRecu);

              // Calcule de l'heure actuelle sur le serveur
              sint32 heureDeLaDemande = stringEnSint32(heureDemande);
              sint32 heureReglage = stringEnSint32(heureRecu) + ((horlogeClient.get_time() - heureDeLaDemande) / 2);

              // Reglage de notre horloge synchronisee
              horlogeSynchronisee.adjust(heureReglage);
            }

            if (entete == "LISTE_JOUEURS") {
              etiquettePseudosJoueurs.modifierTexte("Liste des joueurs : " + messageRecu);

              listePseudosJoueurs.clear();

              while(TRUE) {
                std::string pseudo = decapsuler(&messageRecu);
                if (pseudo == "") {
                  break;
                }
                listePseudosJoueurs.push_back(pseudo);
              }

              etiquetteNbJoueurs.modifierTexte("Nombre de joueurs : " + sint32EnString(listePseudosJoueurs.size()));
            }
          }

          // Si on clique sur Se connecter
          if (boutonSeConnecter.clique()) {
            // On se connecte
            clientUdp.envoyer("CONNEXION " + pseudo);

            SDL_RaiseWindow(this->window);

            // On joue
            initOpenGL();
            Scene scene(this->window);
            scene.reglerHorloge(horlogeSynchronisee.get_time());
            scene.clientUDPAUtiliser(&clientUdp);
            scene.reglerNumeroJoueur(numeroJoueur);
            scene.creerPersonnage();
            //scene.creerTableauScores(listePseudosJoueurs.size());
            //scene.reglerPseudosJoueurs(listePseudosJoueurs);
            scene.executer();
          }

          if (boutonRetour.clique()) {
            break;
          }
          menuConnexion.dessiner();
        }
      }

      // Force le menu a etre redessine
      //menuPrincipal.dessiner(TRUE);
    }
    menuPrincipal.dessiner();
  }

  // Destruction de la window
  this->destroy_opengl_window();

  // Enregistrement d'une eventuelle modification
  this->init_file.modifier("pseudo", zoneTextePseudo.texte());
  this->init_file.modifier("serveur", zoneTexteServeur.texte());
  this->init_file.modifier("pleinEcran", (TRUE == pleinEcranCoche) ? "1" : "0");

  this->init_file.enregistrer("jeu.ini");

  return TRUE;
}

void Game::init_SDL(void)
{
  // Demarrage de la SDL avec le module video
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
}

void Game::init_SDL_ttf(void)
{
  // Initialise SDL_ttf
  TTF_Init();
}

bool8 Game::create_opengl_window(void)
{
  // Si l'utilisateur veut jouer en mode "plein ecran"
  if ("1" == init_file.lire("pleinEcran")) {
    // Creation de la window en mode "plein ecran"
    this->window = SDL_CreateWindow(TITRE_APPLICATION, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width, this->height, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
  } 
  else {
    // Creation de la window en mode "window"
    this->window = SDL_CreateWindow(TITRE_APPLICATION, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARGEUR_FENETRE_PAR_DEFAUT, HAUTEUR_FENETRE_PAR_DEFAUT, SDL_WINDOW_OPENGL);
  }

  // Si la window n'a pas pu etre creee
  if (NULL == this->window) {
    // Affichage d'un message d'erreur
    std::cerr << "La window n'a pas pu etre creee" << std::endl;
    return FALSE;
  }

  // Creation du contexte OpenGL
  this->ctx_opengl = SDL_GL_CreateContext( this->window );
  if( this->ctx_opengl == NULL ) {
    printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
    return FALSE;
  }

  // Activation de la synchronisation verticale
  if( SDL_GL_SetSwapInterval( 1 ) < 0 ) {
    printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
  }
  std::cerr << "Version OpenGL " << glGetString(GL_VERSION) << std::endl;
  return TRUE;
}

void Game::destroy_opengl_window(void)
{
  // Destruction de la window
  SDL_DestroyWindow(this->window);
  this->window = NULL;
}