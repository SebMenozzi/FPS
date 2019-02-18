#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <sstream>
#include <iostream>

// gui
#include "gui/menu.h"
#include "gui/button.h"
#include "gui/textInput.h"
#include "gui/label.h"

// autres
#include "scene.h"
#include "init.h"
#include "configuration.h"
#include "udpClient.h"
#include "clock.h"
#include "utils.h"
#include "iniFile.h"

// Toutes 5 secondes
#define DUREE_ENTRE_DEMANDE_JOUEURS 5000

Game::Game(void)
{
  // Initialisation des attributs
  this->width = WINDOW_WIDTH;
  this->height = WINDOW_HEIGHT;
  this->window = 0;

  // Initialisation de la SDL
  this->initSDL();
  this->initSDL_ttf();
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
  if (FALSE == configurationFile.load("game.ini")) {
    std::cerr << "Erreur lors du chargement du fichier game.ini" << std::endl;
  }

  // Creation de la window OpenGL
  if (FALSE == this->createOpenGLWindow()) {
    std::cerr << "Erreur lors de la creation de la window OpenGL" << std::endl;
    return FALSE;
  }

  Menu mainMenu(this->window, "fond_menu.bmp");

  Label serverLabel(this->window, this->width/2, 50, "Server :");
  mainMenu.add(&serverLabel);

  TextInput serverInput(this->window, this->width/2, 90);
  serverInput.changeText(configurationFile.read("server"));
  mainMenu.add(&serverInput);

  Label usernameLabel(this->window, this->width/2, 150, "Username :");
  mainMenu.add(&usernameLabel);

  TextInput usernameInput(this->window, this->width / 2, 190);
  usernameInput.changeText(configurationFile.read("username"));
  mainMenu.add(&usernameInput);

  Button playButton(this->window, this->width / 2, 290, "Play");
  mainMenu.add(&playButton);

  Button exitButton(this->window, this->width / 2, 360, "Exit");
  mainMenu.add(&exitButton);

  while (true) {
    mainMenu.draw();

    // Si on clique sur Quitter
    if (exitButton.click()) {
      break;
    }

    // Si on clique sur Jouer
    if (playButton.click()) {
      // Création du menu de Connexion
      Menu connectionMenu(this->window, "fond_menu.bmp");

      Label numberPlayersLabel(this->window, this->width / 2, 100, "Number of players : 0");
      connectionMenu.add(&numberPlayersLabel);

      Label listPlayersLabel(this->window, this->width / 2, 150, "List of players : ");
      connectionMenu.add(&listPlayersLabel);

      Button connectButton(this->window, this->width / 2, 320, "Connect");
      connectionMenu.add(&connectButton);

      Button backButton(this->window, this->width / 2, 400, "Back");
      connectionMenu.add(&backButton);

      // On initialise le client UDP
      UDPClient udpClient;
      udpClient.connect(serverInput.getText(), 2712);

      // On initalise l'horloge
      Clock horlogeClient;
      horlogeClient.adjust(0);

      Clock adjustedClock;

      uint32 playerId = 0;

      std::vector<std::string> playerUsernames;

      // Traitement du pseudo
      std::string messagePseudo;
      std::string username = usernameInput.getText();

      // Si le pseudo n'est pas vide
      if (username != "") {
        // mettre en snake case
        for(uint32 i = 0; i < username.size(); i++) {
          if (username[i] == ' ' || username[i] == '\t')
            username[i] = '_';
        }

        while(TRUE) {
          connectionMenu.draw();

          // refresh the connected players
          static sint32 lastTimeUpdated = adjustedClock.getTime() - DUREE_ENTRE_DEMANDE_JOUEURS;

          if (adjustedClock.getTime() >= lastTimeUpdated + DUREE_ENTRE_DEMANDE_JOUEURS) {
            lastTimeUpdated = adjustedClock.getTime();
            udpClient.send("DEMANDE_JOUEURS");
          }

          std::string receivedMessage = udpClient.receive();

          // Si on a recu un message
          if(receivedMessage != "") {
            // Lecture de l'header
            std::string header = decapsulate(&receivedMessage);

            if (header == "REPONSE_HEURE") {
              // Lecture de l'heure serveur
              std::string timeAsked = decapsulate(&receivedMessage);
              std::string timeReceived = decapsulate(&receivedMessage);

              // Calcule de l'heure actuelle sur le serveur
              sint32 time = stringToSint32(timeAsked);
              sint32 timeAjusted = stringToSint32(timeReceived) + ((horlogeClient.getTime() - time) / 2);

              // Adjust the clock according the server clock
              adjustedClock.adjust(timeAjusted);
            }

            if (header == "LISTE_JOUEURS") {
              listPlayersLabel.changeText("Liste des joueurs : " + receivedMessage);

              playerUsernames.clear();

              while(TRUE) {
                std::string username = decapsulate(&receivedMessage);
                if (username == "") {
                  break;
                }
                playerUsernames.push_back(username);
              }

              numberPlayersLabel.changeText("Number of players : " + sint32ToString(playerUsernames.size()));
            }
          }

          // Si on clique sur Se connecter
          if (connectButton.click()) {
            // On se connecte
            udpClient.send("CONNEXION " + username);

            SDL_RaiseWindow(this->window);

            // On joue
            initOpenGL();
            Scene scene(this->window);
            scene.setClock(adjustedClock.getTime());
            scene.setUDPClient(&udpClient);
            scene.setPlayerId(playerId);
            scene.createPlayer();
            //scene.creerTableauScores(playerUsernames.size());
            //scene.reglerPseudosJoueurs(playerUsernames);
            scene.run();
          }

          if (backButton.click()) {
            break;
          }
        }
      }

      // Force le menu a etre redessine
      mainMenu.draw(TRUE);
    }
  }

  // Destruction de la window
  this->destroyOpengGLWindow();

  // Enregistrement d'une eventuelle modification
  this->configurationFile.change("username", usernameInput.getText());
  this->configurationFile.change("server", serverInput.getText());

  this->configurationFile.save("game.ini");

  return TRUE;
}

void Game::initSDL(void)
{
  // Demarrage de la SDL avec le module video
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
}

void Game::initSDL_ttf(void)
{
  // Initialise SDL_ttf
  TTF_Init();
}

bool8 Game::createOpenGLWindow(void)
{
  // Si l'utilisateur veut jouer en mode "plein ecran"
  if ("1" == configurationFile.read("fullScreen")) {
    // Creation de la window en mode "plein ecran"
    this->window = SDL_CreateWindow(TITLE_APPLICATION, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width, this->height, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
  }
  else {
    // Creation de la window en mode "window"
    this->window = SDL_CreateWindow(TITLE_APPLICATION, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
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
  if(SDL_GL_SetSwapInterval( 1 ) < 0 ) {
    printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
  }
  //std::cerr << "Version OpenGL " << glGetString(GL_VERSION) << std::endl;
  return TRUE;
}

void Game::destroyOpengGLWindow(void)
{
  // Destruction de la window
  SDL_DestroyWindow(this->window);
  this->window = NULL;
}
