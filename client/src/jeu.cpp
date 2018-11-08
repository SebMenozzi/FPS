#include "jeu.h"
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

Jeu::Jeu(void)
{
  // Initialisation des attributs
  this->largeurFenetre = LARGEUR_FENETRE_PAR_DEFAUT;
  this->hauteurFenetre = HAUTEUR_FENETRE_PAR_DEFAUT;
  this->fenetre = 0;

  // Initialisation de la SDL
  this->initialiserSDL();

  // Initialisation de SDL_ttf
  this->initialiserSDL_ttf();
}

Jeu::~Jeu(void)
{
  // Arret de SDL_ttf
  TTF_Quit();

  // Arret de la SDL
  SDL_Quit();
}


// NOTE : Pour savoir ce qu'il faut mettre ici ou dans le constructeur, il faut
// se dire que le jeu peut etre execute plusieurs fois a partir de la meme instance
// Prevoir la posibilite de recharger la configuration du jeu en cas de modification
// du mode d'affichage

bool8 Jeu::executer(void)
{
  // Chargement du fichier d'initialisation du jeu
  if (FALSE == fichierIniJeu.charger("jeu.ini"))
  {
    // En cas de probleme, affichage d'un message d'erreur
    std::cerr << "Erreur lors du chargement du fichier jeu.ini" << std::endl;
  }

  // Creation de la fenetre OpenGL
  if (FALSE == this->creerFenetreOpenGL())
  {
    // En cas de probleme, arret de l'execution du jeu
    std::cerr << "Erreur lors de la creation de la fenetre OpenGL" << std::endl;
    return FALSE;
  }

  Menu menuConnexion(this->fenetre, "fond_menu.bmp");

  Etiquette etiquetteServeur(this->fenetre, this->largeurFenetre/2, 50, "Serveur :");
  menuConnexion.ajouter(&etiquetteServeur);

  ZoneTexte zoneTexteServeur(this->fenetre, this->largeurFenetre/2, 90);
  zoneTexteServeur.modifierTexte(fichierIniJeu.lire("serveur"));
  menuConnexion.ajouter(&zoneTexteServeur);

  Etiquette etiquettePseudo(this->fenetre, this->largeurFenetre/2, 150, "Pseudo :");
  menuConnexion.ajouter(&etiquettePseudo);

  ZoneTexte zoneTextePseudo(this->fenetre, this->largeurFenetre / 2, 190);
  zoneTextePseudo.modifierTexte(fichierIniJeu.lire("pseudo"));
  menuConnexion.ajouter(&zoneTextePseudo);

  Etiquette etiquettePleinEcran(this->fenetre, (this->largeurFenetre / 2) - 30, 250, "Plein ecran :");
  menuConnexion.ajouter(&etiquettePleinEcran);

  CaseACocher casePleinEcran((this->largeurFenetre / 2) + 70, 250, (fichierIniJeu.lire("pleinEcran") == "1") ? TRUE : FALSE);
  bool8 peCoche = (fichierIniJeu.lire("pleinEcran") == "1") ? TRUE : FALSE;
  menuConnexion.ajouter(&casePleinEcran);

  Bouton boutonJouer(this->fenetre, this->largeurFenetre / 2, 320, "Jouer");
  menuConnexion.ajouter(&boutonJouer);

  Bouton boutonQuitter(this->fenetre, this->largeurFenetre / 2, 400, "Quitter");
  menuConnexion.ajouter(&boutonQuitter);

  menuConnexion.dessiner();

  while(TRUE)
  {
    // Si la case a change d'etat
    if (peCoche != casePleinEcran.cochee())
    {
      peCoche = casePleinEcran.cochee();

      // Met la fenetre dans l'etat demande
      SDL_SetWindowFullscreen(this->fenetre, (TRUE == peCoche) ? SDL_WINDOW_FULLSCREEN : 0);
    }

    if (boutonQuitter.clique())
    {
      break;
    }

    if (boutonJouer.clique())
    {
      Menu menuSynchronisation(this->fenetre, "fond_menu.bmp");

      Bouton boutonQuitter(this->fenetre, this->largeurFenetre / 2, 400, "Quitter");
      menuSynchronisation.ajouter(&boutonQuitter);

      Etiquette etiquetteHorlogeServeur(this->fenetre, this->largeurFenetre / 2, 200, "Horloge : 0");
      menuSynchronisation.ajouter(&etiquetteHorlogeServeur);
      etiquetteHorlogeServeur.visible(FALSE);

      Etiquette etiquettePseudosJoueurs(this->fenetre, this->largeurFenetre / 2, 300, "Liste des joueurs : ");
      menuSynchronisation.ajouter(&etiquettePseudosJoueurs);

      Etiquette etiquetteNbJoueurs(this->fenetre, this->largeurFenetre / 2, 250, "Nombre de joueurs : 0");
      menuSynchronisation.ajouter(&etiquetteNbJoueurs);

      Etiquette partieEnCours(this->fenetre, this->largeurFenetre / 2, 350, "");
      menuSynchronisation.ajouter(&partieEnCours);

      ClientUDP clientUdp;
      clientUdp.connect(zoneTexteServeur.texte(), 2712);

      Horloge horlogeClient;
      horlogeClient.regler(0);

      sint32 heureProchainsMessages = 0;

      Horloge horlogeSynchronisee;

      sint32 heureProchainePartie = 0;

      uint32 numeroJoueur = 0;

      uint32 heureFinPartie;

      std::vector<std::string> listePseudosJoueurs;

      while(TRUE)
      {
        std::string messageRecu = clientUdp.recevoir();

        // Si on a recu un message
        if(messageRecu != "")
        {
          // Lecture de l'entete
          std::string entete = decapsuler(&messageRecu);

          if (entete == "REPONSE_HEURE")
          {
            // Lecture de l'heure serveur
            std::string heureDemande = decapsuler(&messageRecu);
            std::string heureRecu = decapsuler(&messageRecu);

            // Calcule de l'heure actuelle sur le serveur
            sint32 heureDeLaDemande = stringEnSint32(heureDemande);
            sint32 heureReglage = stringEnSint32(heureRecu) + ( (horlogeClient.heure() - heureDeLaDemande) / 2);

            // Reglage de notre horloge synchronisee
            horlogeSynchronisee.regler(heureReglage);
          }

          if (entete == "HEURE_FIN_PROCHAINE_PARTIE")
          {
            // Lecture de la duree de la partie
            heureFinPartie = stringEnUint32(decapsuler(&messageRecu));
          }

          if (entete == "LISTE_JOUEURS")
          {
            etiquettePseudosJoueurs.modifierTexte("Liste des joueurs : " + messageRecu);

            listePseudosJoueurs.clear();

            while(1)
            {
              std::string pseudo = decapsuler(&messageRecu);
              if (pseudo == "")
              {
                break;
              }

              //nbJoueurs++;
              listePseudosJoueurs.push_back(pseudo);
            }

            etiquetteNbJoueurs.modifierTexte("Nombre de joueurs : " + sint32EnString(listePseudosJoueurs.size()));
          }

          if (entete == "HEURE_PROCHAINE_PARTIE")
          {
            // Memorisation de l'heure de la prochaine partie
            heureProchainePartie = stringEnSint32(decapsuler(&messageRecu));
          }

          if (entete == "NUMERO_JOUEUR")
          {
            // Memorisation du numero de joueur
            numeroJoueur = stringEnSint32(decapsuler(&messageRecu));

            // On est inscrit, partie en cours doit etre effacee
            partieEnCours.modifierTexte("");
          }

          // Si une partie est en cours sur ce serveur
          if (entete == "PARTIE_EN_COURS")
          {
            // Lecture de la duree approximative restante
            std::string secondesAvantFinPartieEnCours = "Partie en cours... "
                                                        + decapsuler(&messageRecu)
                                                        + " secondes restantes.";
            partieEnCours.modifierTexte(secondesAvantFinPartieEnCours);
          }
        }

        // S'il est l'heure d'envoyer les prochains messages
        if(horlogeClient.heure() >= heureProchainsMessages)
        {
          // Envoi du pseudo
          std::string messagePseudo;
          std::string pseudo = zoneTextePseudo.texte();
          if (pseudo == "")
          {
              pseudo = "x";
          }
          for(uint32 i = 0; i < pseudo.size(); i++)
          {
            if (pseudo[i] == ' ' || pseudo[i] == '\t')
            {
              pseudo[i] = '_';
            }
          }
          messagePseudo = "PSEUDO " + pseudo;
          clientUdp.envoyer(messagePseudo);

          // Demande de l'heure au serveur
          std::string messageDemandeHeure = "DEMANDE_HEURE " + sint32EnString(horlogeClient.heure());
          clientUdp.envoyer(messageDemandeHeure);

          // Les prochains messages un peu plus tard
          heureProchainsMessages += 200;
        }

        if (boutonQuitter.clique())
        {
          break;
        }

        // S'il est l'heure de la prochaine partie
        if (heureProchainePartie != 0 && horlogeSynchronisee.heure() >= heureProchainePartie)
        {
          SDL_RaiseWindow(this->fenetre);

          // On joue
          initOpenGL();
          Scene scene(this->fenetre);
          scene.reglerHorloge(horlogeSynchronisee.heure());
          scene.clientUDPAUtiliser(&clientUdp);
          scene.creerPersonnages(listePseudosJoueurs.size());
          scene.reglerNumeroJoueur(numeroJoueur);
          scene.reglerHeureFinPartie(heureFinPartie);
          scene.creerTableauScores(listePseudosJoueurs.size());
          scene.reglerPseudosJoueurs(listePseudosJoueurs);
          scene.executer();

          heureProchainePartie = 0;

          heureProchainsMessages = horlogeClient.heure() + 200;

          // Vide la boite a lettre
          while("" != clientUdp.recevoir())
          {

          }
        }
        etiquetteHorlogeServeur.modifierTexte("Horloge : " + sint32EnString(horlogeSynchronisee.heure()));
        menuSynchronisation.dessiner();
      }
      //initOpenGL();
      //Scene scene;
      //scene.executer();

      // Force le menu a etre redessine
      menuConnexion.dessiner(TRUE);
    }
    menuConnexion.dessiner();
  }
  // Destruction de la fenetre
  this->detruireFenetreOpenGL();

  // Enregistrement d'une eventuelle modification
  this->fichierIniJeu.modifier("pseudo", zoneTextePseudo.texte());
  this->fichierIniJeu.modifier("serveur", zoneTexteServeur.texte());
  this->fichierIniJeu.modifier("pleinEcran", (TRUE == peCoche) ? "1" : "0");

  this->fichierIniJeu.enregistrer("jeu.ini");

  return TRUE;
}

void Jeu::initialiserSDL(void)
{
  // Demarrage de la SDL avec le module video
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
}

void Jeu::initialiserSDL_ttf(void)
{
  // Initialise SDL_ttf
  TTF_Init();
}

bool8 Jeu::creerFenetreOpenGL(void)
{
  // Si l'utilisateur veut jouer en mode "plein ecran"
  if ("1" == fichierIniJeu.lire("pleinEcran"))
  {
    // Lecture de la plus grande resolution disponnible
    /*SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
    if (0 != SDL_GetDisplayMode(0, 0, &mode))
    {
        std::cout << "Impossile de detecter la resolution" << std::endl;
    }
    this->largeurFenetre = mode.w;
    this->hauteurFenetre = mode.h;*/

    // Creation de la fenetre en mode "plein ecran"
    this->fenetre = SDL_CreateWindow(TITRE_APPLICATION, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->largeurFenetre, this->hauteurFenetre, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
  }
  else
  {
    // Creation de la fenetre en mode "fenetre"
    this->fenetre = SDL_CreateWindow(TITRE_APPLICATION, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARGEUR_FENETRE_PAR_DEFAUT, HAUTEUR_FENETRE_PAR_DEFAUT, SDL_WINDOW_OPENGL);
  }

  // Si la fenetre n'a pas pu etre creee
  if (NULL == this->fenetre)
  {
    // Affichage d'un message d'erreur
    std::cerr << "La fenetre n'a pas pu etre creee" << std::endl;
    return FALSE;
  }

  // Creation du contexte OpenGL
  this->contexteOpenGL = SDL_GL_CreateContext( this->fenetre );
  if( this->contexteOpenGL == NULL )
  {
    printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
    return FALSE;
  }

  // Activation de la synchronisation verticale
  if( SDL_GL_SetSwapInterval( 1 ) < 0 )
  {
    printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
    //return FALSE;
  }
  //std::cerr << "Version OpenGL" << glGetString(GL_VERSION) << std::endl;
  return TRUE;
}

void Jeu::detruireFenetreOpenGL(void)
{
  // Destruction de la fenetre
  SDL_DestroyWindow(this->fenetre);
  this->fenetre = NULL;
}
