#include <SDL2/SDL.h>
#include "scene.h"
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#include "configuration.h"
#include <sstream>
#include "fonctionsUtiles.h"
#include "gui/etiquette.h"
#include <iomanip>
#include <cmath>
//#include <iostream>

Scene::Scene(SDL_Window* fenetre)
{
  // Initialisation des pointeurs
  this->fenetre = NULL;
  this->clientUDP = NULL;
  this->skybox = NULL;
  this->carte = NULL;
  this->police = NULL;
  this->overlay = NULL;
  this->viseur = NULL;
  this->fontProchaineApparition = NULL;
  this->fondTableauScores = NULL;
  this->etiquetteProchaineApparition = NULL;
  this->etiquetteTempsRestant = NULL;

  this->fenetre = fenetre;
  this->largeurFenetre = 0;
  this->hauteurFenetre = 0;
  this->overlay = 0;

  // Creation de l'overlay
  this->overlay = new Overlay(fenetre);

  // Recuperation des dimentions de la fenetre
  SDL_GetWindowSize(fenetre, &this->largeurFenetre, &this->hauteurFenetre);


  // Maintien de la souris dans la fenetre
  //SDL_WM_GrabInput(SDL_GRAB_ON);
  SDL_SetWindowGrab(this->fenetre, SDL_TRUE);

  // La souris est au centre de l'ecran
  //SDL_WarpMouse( (LARGEUR_FENETRE/2), (HAUTEUR_FENETRE/2) );
  SDL_WarpMouseInWindow(this->fenetre, (this->largeurFenetre / 2), (this->hauteurFenetre / 2));

  // La souris est invisible
  SDL_ShowCursor(FALSE);

  this->skybox = new Objet3DStatique("skybox.m3s");
  this->carte = new Carte("carte.bmp");
  //this->personnage = new Personnage(2, 2, 0, -130, "raphael.m3s");

  // Connexion au serveur
  //this->clientUDP = new ClientUDP();
  //this->clientUDP->connect("127.0.0.1", 2712);

  // Activation du test de profondeur (desactive par le menu)
  glEnable(GL_DEPTH_TEST);

  // Reglage de l'horloge
  horloge.regler(0);

  // Chargement de la police
  police = TTF_OpenFont("Cartonsix NC.ttf", 40);

  // Creation du viseur
  this->viseur = new ImageOverlay(this->fenetre, "viseur.bmp", (this->largeurFenetre / 2), (this->hauteurFenetre / 2));
  this->overlay->ajouter(this->viseur);

  // Etiquette pour afficher le temps restant
  this->etiquetteTempsRestant = new EtiquetteOverlay(this->fenetre, this->largeurFenetre-100, 50, "1:30");
  this->overlay->ajouter(this->etiquetteTempsRestant);

  // Fond de l'ecran de "prochaine apparition"
  this->fontProchaineApparition = new ImageOverlay(this->fenetre, "prochaine_apparition.bmp", (this->largeurFenetre / 2), (this->hauteurFenetre / 2));
  this->overlay->ajouter(this->fontProchaineApparition);
  this->fontProchaineApparition->rendreInvisible();

  // Etiquette de l'ecran de "prochaine apparition"
  this->etiquetteProchaineApparition = new EtiquetteOverlay(this->fenetre, (this->largeurFenetre / 2), (this->hauteurFenetre / 2), "Prochaine apparition sur scene dans 5 secondes", 35);
  this->overlay->ajouter(this->etiquetteProchaineApparition);
  this->etiquetteProchaineApparition->rendreInvisible();

  // Creation du tableau des scores
  this->fondTableauScores = new ImageOverlay(this->fenetre, "tableau_scores.bmp", (this->largeurFenetre / 2), (this->hauteurFenetre / 2));
  this->overlay->ajouter(this->fondTableauScores);
  this->fondTableauScores->rendreInvisible();
}

Scene::~Scene()
{
  // Liberation des widgets de l'overlay
  delete this->viseur;
  delete this->fontProchaineApparition;
  delete this->etiquetteProchaineApparition;
  delete this->etiquetteTempsRestant;
  delete this->fondTableauScores;

  delete this->overlay;

  // Liberation des etiquettes du tableau des scores
  for(uint32 i = 0; i < this->etiquettesScoresPseudo.size(); i++)
  {
    delete this->etiquettesScoresPseudo[i];
    delete this->etiquettesScoresEmis[i];
    delete this->etiquettesScoresRecu[i];
  }

  // Liberation de la police
  TTF_CloseFont(police);

  delete this->skybox;

  // La souris est visible
  SDL_ShowCursor(TRUE);

  // Ne maintien plus la souris dans la fenetre
  //SDL_WM_GrabInput(SDL_GRAB_OFF);
  SDL_SetWindowGrab(this->fenetre, SDL_FALSE);
}

void Scene::executer()
{
  // Initialisation des variables de gestion du temps
  this->tempsDernierPas = 0;
  uint32 heureDernierPas = SDL_GetTicks();

  this->continuer = TRUE;

  while(this->continuer)
  {
    // On effectue la serie de calcule avec la meme heure actuelle
    this->heureActuelle = this->horloge.heure();
    gererEvenements();
    animer();

    char titre[100] = {0};
    sprintf(titre, "px=%f py=%f a=%f",
      this->personnages[this->numeroJoueur]->lirePositionX(),
      this->personnages[this->numeroJoueur]->lirePositionY(),
      this->personnages[this->numeroJoueur]->lireAngle()
    );

    //SDL_SetWindowTitle(this->fenetre, titre);
    dessiner();
    dessiner2D();
    afficher();

    // On calcule le temps de construction de la derniere image
    this->tempsDernierPas = SDL_GetTicks() - heureDernierPas;
    heureDernierPas += this->tempsDernierPas;

    // Detection de la fin de la partie
    if (this->heureActuelle >= this->heureFinPartie)
    {
      this->continuer = FALSE;

      bool8 afficherTableauScores = TRUE;

      // Affichage des scores, 10 secondes maximum apres la fin de partie
      while(afficherTableauScores && this->horloge.heure() < this->heureFinPartie + 10000)
      {
        // Lecture des evenements jusqu'a vider la file
        SDL_Event evenement;
        while (SDL_PollEvent(&evenement))
        {
          switch(evenement.type)
          {
            case SDL_KEYDOWN:
            // Permet de quitter grace a la touche Echap
            if (evenement.key.keysym.sym == SDLK_ESCAPE)
            {
                afficherTableauScores = FALSE;
            }
            break;
          }
        }

        // Reception des scores
        std::string message = this->clientUDP->recevoir();
        std::string entete = decapsuler(&message);
        if (entete == "SCORES")
        {
          // Mise a jour des scores des personnages
          for(uint32 i = 0; i < this->personnages.size(); i++)
          {
            uint32 emis = stringEnUint32(decapsuler(&message));
            uint32 recu = stringEnUint32(decapsuler(&message));
            this->personnages[i]->mettreAJourScore(emis, recu);
          }
        }

        // Affichage des scores
        this->fondTableauScores->rendreVisible();

        for(uint32 i = 0; i < this->personnages.size(); i++)
        {
          this->etiquettesScoresPseudo[i]->rendreVisible();
          this->etiquettesScoresEmis[i]->rendreVisible();
          this->etiquettesScoresRecu[i]->rendreVisible();
          std::stringstream flux;
          this->etiquettesScoresPseudo[i]->modifierTexte(this->personnages[i]->pseudo());
          flux << this->personnages[i]->emis();
          this->etiquettesScoresEmis[i]->modifierTexte(flux.str());
          flux.str("");
          flux << this->personnages[i]->recu();
          this->etiquettesScoresRecu[i]->modifierTexte(flux.str());
        }
        // Dessin de l'overlay
        this->dessiner2D();
        this->afficher();
      }
    }
  }
}

void Scene::animer(void)
{
  // Lecture de l'�tat des touches
  int nombreDeTouches;
  const Uint8* pTouches = SDL_GetKeyboardState(&nombreDeTouches);
  Uint8* touches = new Uint8[nombreDeTouches];
  memcpy(touches, pTouches, nombreDeTouches);

  // Evite les deplacements absurdes
  if (touches[SDL_GetScancodeFromKey(SDLK_z)] == touches[SDL_GetScancodeFromKey(SDLK_s)]) // Avancer et reculer
  {
    touches[SDL_GetScancodeFromKey(SDLK_z)] = 0u; // Ici, on travaille sur
    touches[SDL_GetScancodeFromKey(SDLK_s)] = 0u; // une copie des touches
  }
  if (touches[SDL_GetScancodeFromKey(SDLK_q)] == touches[SDL_GetScancodeFromKey(SDLK_d)]) // Gauche et droite
  {
    touches[SDL_GetScancodeFromKey(SDLK_q)] = 0u;
    touches[SDL_GetScancodeFromKey(SDLK_d)] = 0u;
  }

  float16 direction;
  bool8 deplacement = FALSE;

  if (touches[SDL_GetScancodeFromKey(SDLK_z)]) // Touche Z
  {
    // Avancer
    deplacement = TRUE;

    // En diagonale
    if (touches[SDL_GetScancodeFromKey(SDLK_q)]) direction = 45.0;
    else if (touches[SDL_GetScancodeFromKey(SDLK_d)]) direction = -45.0;

    // Droit
    else direction = 0.0;
  }
  else if (touches[SDL_GetScancodeFromKey(SDLK_s)]) // Touche S
  {
    // Reculer
    deplacement = TRUE;

    // En diagonale
    if (touches[SDL_GetScancodeFromKey(SDLK_q)]) direction = 135.0;
    else if (touches[SDL_GetScancodeFromKey(SDLK_d)]) direction = -135.0;

    // Droit
    else direction = 180.0;
  }
  if(FALSE == deplacement)
  {
    if (touches[SDL_GetScancodeFromKey(SDLK_q)]) // Touche Q
    {
      // Gauche
      direction = 90.0;
      deplacement = TRUE;
    }
    else if (touches[SDL_GetScancodeFromKey(SDLK_d)]) // Touche D
    {
      // Droite
      direction = -90.0;
      deplacement = TRUE;
    }
  }

  delete[] touches;

  // Si on est en mode reapparition
  if (this->personnages[this->numeroJoueur]->heureReapparition() > this->heureActuelle)
  {
    // Le menu de reapparition sera affiche
    this->fontProchaineApparition->rendreVisible();
    this->etiquetteProchaineApparition->rendreVisible();

    // Mise a jour du message de reapparition
    std::stringstream flux;
    sint32 secondesRestantes = (this->personnages[this->numeroJoueur]->heureReapparition() - this->heureActuelle) / 1000;
    flux << "Prochaine apparition sur sc�ne dans " << secondesRestantes << " seconde" << ( (secondesRestantes >= 2)? "s" : "") << ".";
    this->etiquetteProchaineApparition->modifierTexte(flux.str());
  }
  else // Pas en mode "reapparition"
  {
    // Le menu de reapparition sera masque
    this->fontProchaineApparition->rendreInvisible();
    this->etiquetteProchaineApparition->rendreInvisible();
  }

  // Si un deplacement est demande et qu'on n'est pas en mode "reapparition"
  // TODO : Un mort ne doit pas pouvoir tirer avant l'heure de sa reaparition
  if (deplacement && this->personnages[this->numeroJoueur]->heureReapparition() < this->heureActuelle)
  {
    #define VITESSE_DEPLACEMENT (5.0f)

    // Calcule de la distance a parcourir
    float16 distance = (float)tempsDernierPas * VITESSE_DEPLACEMENT / 1000.0f;

    // Calcule des segments de deplacement
    #define TAILLE_GRAND_SEGMENT (0.25f)
    uint32 nbGrandsSegments = (uint32)(distance / TAILLE_GRAND_SEGMENT);
    float16 taillePetitSegment = fmod(distance, TAILLE_GRAND_SEGMENT);

    // Pour chaque grand segment
    for(uint32 i = 0; i < nbGrandsSegments; i++)
    {
      // Recuperation de l'environnement
      sint32 positionCarteX = 0, positionCarteY = 0;
      bool8 entouragePersonnage[8] = {0};
      this->personnages[this->numeroJoueur]->positionSurLaCarte(&positionCarteX, &positionCarteY);
      this->carte->entourage(positionCarteX, positionCarteY, entouragePersonnage);

      // Deplacement du personnage dans la direction demande
      this->personnages[this->numeroJoueur]->deplacer(TAILLE_GRAND_SEGMENT, direction, entouragePersonnage);
    }

    // Pour le petit segment

    // Recuperation de l'environnement
    sint32 positionCarteX = 0, positionCarteY = 0;
    bool8 entouragePersonnage[8] = {0};
    this->personnages[this->numeroJoueur]->positionSurLaCarte(&positionCarteX, &positionCarteY);
    this->carte->entourage(positionCarteX, positionCarteY, entouragePersonnage);

    // Deplacement du personnage dans la direction demande
    this->personnages[this->numeroJoueur]->deplacer(taillePetitSegment, direction, entouragePersonnage);
  }

  // Mise a jour du texte affiche pour le temps restant
  uint32 tempsRestantEnSecondes = (this->heureFinPartie - this->heureActuelle) / 1000;
  uint32 minutesRestantes = tempsRestantEnSecondes / 60;
  uint32 secondesRestantes = tempsRestantEnSecondes % 60;
  std::stringstream flux;
  flux << std::setfill('0') << std::setw(2) << minutesRestantes << ":" << std::setfill('0') << std::setw(2) << secondesRestantes;
  this->etiquetteTempsRestant->modifierTexte(flux.str());

  #define DUREE_ENTRE_SNAPSHOT 50
  // Envoi de la position au serveur toutes les 50 ms en moyenne
  static sint32 heureDernierEnvoi = this->heureActuelle - DUREE_ENTRE_SNAPSHOT;
  if (this->heureActuelle >= heureDernierEnvoi + DUREE_ENTRE_SNAPSHOT)
  {
    heureDernierEnvoi = this->heureActuelle;

    std::stringstream message;
    message << "POSITION_JOUEUR " << this->heureActuelle << " " << this->personnages[this->numeroJoueur]->lirePositionX() << " " << this->personnages[this->numeroJoueur]->lirePositionY() << " " << this->personnages[this->numeroJoueur]->lireAngle();
    texteAAfficher = message.str();
    this->clientUDP->envoyer( message.str() );
  }

  // Reception d'un snapshot
  std::string message = this->clientUDP->recevoir();
  std::string entete = decapsuler(&message);
  if (entete == "SNAPSHOT")
  {
    sint32 heure = stringEnSint32(decapsuler(&message));

    // Remplissage de l'historique des personnages
    for(uint32 i = 0; i < this->personnages.size(); i++)
    {
      float16 positionX = stringEnFloat16(decapsuler(&message));
      float16 positionY = stringEnFloat16(decapsuler(&message));
      float16 angle = stringEnFloat16(decapsuler(&message));
      this->personnages[i]->ajouterPosition(heure, positionX, positionY, angle);
    }
  }
  else if (entete == "REAPPARITION")
  {
    // Lecture du message de reapparition
    uint32 numeroDuJoueur = stringEnUint32(decapsuler(&message));
    sint32 heureReapparition = stringEnSint32(decapsuler(&message));
    sint32 positionXReapparition = stringEnSint32(decapsuler(&message));
    sint32 positionYReapparition = stringEnSint32(decapsuler(&message));
    float16 angleReapparition = stringEnFloat16(decapsuler(&message));

    // Ajout du point de reapparition a l'historique du personnage (deux fois pour eviter l'extrapolation au moment de la reapparition)
    // 100 ms plus tot car on affiche ce qu'il y avait dans l'historique 100 ms plus tot
    this->personnages[numeroDuJoueur]->ajouterPosition(heureReapparition-101, positionXReapparition + 0.5, positionYReapparition + 0.5, angleReapparition);
    this->personnages[numeroDuJoueur]->ajouterPosition(heureReapparition-100, positionXReapparition + 0.5, positionYReapparition + 0.5, angleReapparition);

    // Passe le personnage en mode "reapparition"
    this->personnages[numeroDuJoueur]->heureReapparition(heureReapparition);
    Point pointReapparition = {positionXReapparition, positionYReapparition};
    //this->personnages[numeroDuJoueur]->pointReapparition(pointReapparition);
    //this->personnages[numeroDuJoueur]->angleReapparition(angleReapparition);

    // Si c'est notre personnage qui est en mode reapparition
    if (this->numeroJoueur == numeroDuJoueur)
    {
        // Repositionne notre personnage au point impose
        this->personnages[numeroDuJoueur]->positionner(pointReapparition.x + 0.5, pointReapparition.y + 0.5);
        this->personnages[numeroDuJoueur]->orienter(angleReapparition);
    }

    //this->personnages[numeroDuJoueur]->afficherHistorique();

    // TODO : ATTENTION : penser a interdire de tuer un joueur en mode "reapparition"
  }

  // Positionne les personnages a l'endroit ou ils sont a t-100ms
  for(uint32 i = 0; i < this->personnages.size(); i++)
  {
    // Autres personnages
    if (i != numeroJoueur)
    {
      this->personnages[i]->positionnerDepuisLHistorique(this->heureActuelle - 100);
    }
  }
}

void Scene::dessiner(void)
{
  //glEnable( GL_CULL_FACE ); // Activer le masquage des faces cach�es
  glCullFace(GL_BACK); // Face cach�es = faces arri�res
  glFrontFace(GL_CCW); // Face avant = sens trigo

  // Couleur rgba de vidage de l'ecran
  glClearColor(0.0, 0.0, 0.0, 0.0);

  // D�finition de la fenetre
  glViewport(0, 0, this->largeurFenetre, this->hauteurFenetre);

  // D�finition de la zone visible
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(ANGLE_VISION, (GLdouble) this->largeurFenetre / (GLdouble)this->hauteurFenetre, PRET, LOIN);

  // Activation du tampon de profondeur
  glEnable(GL_DEPTH_TEST);

  glColor3f(1.0, 1.0, 1.0);

  // Vidage de l'ecran
  glClear(/*GL_COLOR_BUFFER_BIT |*/ GL_DEPTH_BUFFER_BIT);

  // Place la camera
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  //gluLookAt(1,2,4,2,3,0.7,0,0,1);
  this->personnages[this->numeroJoueur]->regarder();

  // Dessin de la skybox
  this->skybox->dessiner();

  // Dessin de la skybox
  this->carte->dessiner();

  // Dessin des autres personnages
  for(uint32 i = 0; i < this->personnages.size(); i++)
  {
    if (i != this->numeroJoueur)
    {
      // TODO : Ne pas afficher les personnages en reapparition, idem pour le test du tir cote serveur

      // Si le personnage n'est plus en mode "reapparition"
      if ( this->heureActuelle > this->personnages[i]->heureReapparition() )
      {
        this->personnages[i]->dessiner();
      }
    }
  }
}

void Scene::afficher(void)
{
  glFlush();
  SDL_GL_SwapWindow(this->fenetre);
}

void Scene::gererEvenements(void)
{
  SDL_Event evenement;
  uint32 i = 0;
  while (SDL_PollEvent(&evenement))
  {
      switch(evenement.type)
      {
        case SDL_QUIT:
          this->continuer = FALSE;
          break;

        case SDL_MOUSEMOTION:
          // Tourne le personnage selon le deplacement de la souris
          this->personnages[this->numeroJoueur]->tournerHorizontalement(-evenement.motion.xrel*0.06);
          this->personnages[this->numeroJoueur]->tournerVerticalement(-evenement.motion.yrel*0.06);

          // Si la souris s'ecarte de plus de 10 px du centre de la fenetre
          if ( abs( evenement.motion.x - (this->largeurFenetre/2) ) > 10
            || abs( evenement.motion.y - (this->hauteurFenetre/2) ) > 10)
          {
            // Desactive les evenements de type SDL_MOUSEMOTION
            SDL_EventState(SDL_MOUSEMOTION,SDL_DISABLE);

            // Repositionne la souris au centre de la fenetre
            SDL_WarpMouseInWindow(this->fenetre, this->largeurFenetre/2, this->hauteurFenetre/2 );

            // Lecture de la position relative de la souris pour
            // ne pas detecter de deplacement de souris plus tard
            SDL_PumpEvents();
            SDL_GetRelativeMouseState(NULL, NULL);

            // Reactive les evenements de type SDL_MOUSEMOTION
            SDL_EventState(SDL_MOUSEMOTION,SDL_ENABLE);
          }

          break;

        case SDL_KEYDOWN:
          // S'il s'agit d'un enfoncement reel (pas par repetition de touche)
          if (evenement.key.repeat == 0)
          {
            printf("SDL_KEYDOWN\n");
            // Permet de quitter grace a la touche Echap
            if (evenement.key.keysym.sym == SDLK_ESCAPE)
            {
              this->continuer = FALSE;
            }
          }
          break;

        case SDL_MOUSEBUTTONDOWN:
          if (this->personnages[numeroJoueur]->heureReapparition() < this->heureActuelle)
          {
            std::stringstream message;
            message << "TIR " << sint32EnString(this->heureActuelle);
            this->clientUDP->envoyer( message.str() );
          }
          break;
      }

      i++;

      if (i >= 100) break; // Test de deblocage
  }

  // Replace la souris au centre
  //SDL_EventState(SDL_MOUSEMOTION, SDL_DISABLE);
  //SDL_WarpMouseInWindow(this->fenetre, (this->largeurFenetre/2), (this->hauteurFenetre/2));
  //SDL_EventState(SDL_MOUSEMOTION, SDL_DISABLE);
  //SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
}

void Scene::clientUDPAUtiliser(ClientUDP* clientUDP)
{
  this->clientUDP = clientUDP;
}

void Scene::reglerHorloge(sint32 heure)
{
  this->horloge.regler(heure);
}

void Scene::creerPersonnages(uint32 nbJoueurs)
{
  // Lecture de l'heure actuelle pour determiner l'heure de la premiere apparition
  sint32 heureActuelle = this->horloge.heure();

  // Pour chaque joueur
  for(uint32 i = 0; i < nbJoueurs; i++)
  {
    // Creation du personnage
    this->personnages.push_back(new Personnage(2, 2, 0, -130, "raphael.m3s"));

    // Le personnage est immediatement plonge dans le mode "reapparition"
    // pendant 5 secondes, le temps d'obtenir les ordres du serveur
    this->personnages[i]->heureReapparition(heureActuelle+5000);
  }
}

void Scene::reglerNumeroJoueur(uint32 numeroJoueur)
{
  this->numeroJoueur = numeroJoueur;
}

void Scene::dessiner2D(void)
{
  // En 2D
  // Dessin de l'overlay
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);

  // Definition de la fenetre
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble)this->largeurFenetre, 0.0, (GLdouble)this->hauteurFenetre);

  // Desactivation du test de profondeur
  glDisable(GL_DEPTH_TEST);


  this->overlay->dessiner();


  glColor3f(1.0, 1.0, 1.0);
}

void Scene::reglerHeureFinPartie(sint32 heureFin)
{
  this->heureFinPartie = heureFin;
}

void Scene::creerTableauScores(uint32 nbJoueurs)
{
  // Pour chaque joueur
  for(uint32 i = 0; i < nbJoueurs; i++)
  {
    // Pseudo
    this->etiquettesScoresPseudo.push_back(new EtiquetteOverlay(this->fenetre, (this->largeurFenetre/2) - 150, 150+30*i, "Pseudo"));
    this->etiquettesScoresPseudo[i]->rendreInvisible();
    this->overlay->ajouter(this->etiquettesScoresPseudo[i]);

    // Emis
    this->etiquettesScoresEmis.push_back(new EtiquetteOverlay(this->fenetre, (this->largeurFenetre/2), 150+30*i, "Emis"));
    this->etiquettesScoresEmis[i]->rendreInvisible();
    this->overlay->ajouter(this->etiquettesScoresEmis[i]);

    // Recu
    this->etiquettesScoresRecu.push_back(new EtiquetteOverlay(this->fenetre, (this->largeurFenetre/2) + 155, 150+30*i, "Recus"));
    this->etiquettesScoresRecu[i]->rendreInvisible();
    this->overlay->ajouter(this->etiquettesScoresRecu[i]);
  }
}

void Scene::reglerPseudosJoueurs(std::vector<std::string> listePseudosJoueurs)
{
  // Pour chaque joueur
  for(uint32 i = 0; i < this->personnages.size(); i++)
  {
    // Mise en place du pseudo
    this->personnages[i]->pseudo(listePseudosJoueurs[i]);
  }
}
