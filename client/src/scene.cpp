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
#include "utils.h"
#include "gui/label.h"
#include <iomanip>
#include <cmath>
#include <stdio.h>
#include <iostream>

#define DURATION_BETWEEN_SNAPSHOT 50

Scene::Scene(SDL_Window* window)
{
  // Initialisation des pointeurs
  this->window = NULL;
  this->udpClient = NULL;
  this->skybox = NULL;
  this->map = NULL;
  this->heightmap = NULL;
  this->window = window;
  this->width = 0;
  this->height = 0;

  // Retrieving the dimensions of the window
  SDL_GetWindowSize(window, &this->width, &this->height);

  // Maintaining the mouse in the window
  SDL_SetWindowGrab(this->window, SDL_TRUE);

  // Put the mouse in the center of the screen
  SDL_WarpMouseInWindow(this->window, this->width / 2, this->height / 2);

  // The mouse is invisible
  SDL_ShowCursor(FALSE);

  this->skybox = new Objet3DStatique("assets/skybox.m3s");
  this->map = new Map("assets/map.bmp");
  this->heightmap = new Heightmap("assets/heightmap.bmp");

  // Activation du test de profondeur (desactive par le menu)
  glEnable(GL_DEPTH_TEST);

  // Adjust the clock
  clock.adjust(0);
}

Scene::~Scene()
{
  delete this->skybox;

  // La souris est visible
  SDL_ShowCursor(TRUE);

  // Ne maintien plus la souris dans la window
  SDL_SetWindowGrab(this->window, SDL_FALSE);
}

void Scene::run()
{
  // Initialisation des variables de gestion du temps
  this->timeLastLoop = 0;
  uint32 timeLastLoop = this->clock.getTicks();

  this->quit = FALSE;

  while(!this->quit)
  {
    this->currentTime = this->clock.getTime();

    this->handleEvents();
    this->animate();

    // Displaying player's position (debugging)
    char title[100] = {0};
    sprintf(title, "x=%f y=%f z=%f aH=%f aV=%f",
      this->players[this->playerId]->getX(),
      this->players[this->playerId]->getY(),
      this->players[this->playerId]->getZ(),
      this->players[this->playerId]->getHorizontalAngle(),
      this->players[this->playerId]->getVerticalAngle()
    );
    SDL_SetWindowTitle(this->window, title);

    draw();
    draw2D();
    display();

    // Update time
    this->timeLastLoop = this->clock.getTicks() - timeLastLoop;
    timeLastLoop += this->timeLastLoop;
  }
  // On se déconnecte
  this->udpClient->send("DECONNEXION");
}

void Scene::draw(void)
{
  //glEnable( GL_CULL_FACE ); // Activer le masquage des faces cachees
  glCullFace(GL_BACK); // Face cachees = faces arrieres
  glFrontFace(GL_CCW); // Face avant = sens trigo

  // Couleur rgba de vidage de l'ecran
  glClearColor(0.0, 0.0, 0.0, 0.0);

  // Definition of the window
  glViewport(0, 0, this->width, this->height);

  // Definition de la zone visible
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(ANGLE_VISION, (GLdouble) this->width / (GLdouble)this->height, PRET, LOIN);

  // Activation du tampon de profondeur
  glEnable(GL_DEPTH_TEST);

  glColor3f(1.0, 1.0, 1.0);

  // Vidage de l'ecran
  glClear(GL_DEPTH_BUFFER_BIT);

  // Place la camera
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  this->players[this->playerId]->look();

  // Dessin de la skybox
  this->skybox->dessiner();

  // Dessin de la skybox
  this->map->draw();

  // Dessin de la heightmap
  this->heightmap->draw();

  // Dessin des autres players
  for(uint32 i = 0; i < this->players.size(); ++i) {
    if (i != this->playerId) {
      // TODO : Ne pas afficher les players en reapparition, idem pour le test du tir cote serveur
      // Si le personnage n'est plus en mode "reapparition"
      if (this->currentTime > this->players[i]->getReapparitionTime()) {
        this->players[i]->draw();
      }
    }
  }
}

void Scene::handleEvents(void)
{
  SDL_Event event;
  uint32 i = 0;
  while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          this->quit = TRUE;
          break;

        case SDL_MOUSEMOTION:
          // Tourne le personnage selon le deplacement de la souris
          this->players[this->playerId]->horizontalTurn(-event.motion.xrel * 0.06);
          this->players[this->playerId]->verticalTurn(-event.motion.yrel * 0.06);

          // Si la souris s'ecarte de plus de 10 px du centre de la window
          if ( abs(event.motion.x - (this->width / 2)) > 10
            || abs(event.motion.y - (this->height / 2)) > 10)
          {
            // Desactive les evenements de type SDL_MOUSEMOTION
            SDL_EventState(SDL_MOUSEMOTION,SDL_DISABLE);

            // Repositionne la souris au centre de la window
            SDL_WarpMouseInWindow(this->window, this->width / 2, this->height / 2 );

            // Lecture de la position relative de la souris pour
            // ne pas detecter de deplacement de souris plus tard
            SDL_PumpEvents();
            SDL_GetRelativeMouseState(NULL, NULL);

            // Reactive les evenements de type SDL_MOUSEMOTION
            SDL_EventState(SDL_MOUSEMOTION,SDL_ENABLE);
          }

          break;

        case SDL_KEYDOWN:
          // No repetition, only "real click"
          if (event.key.repeat == 0) {
            // Quit by pressing the escape key
            if (event.key.keysym.sym == SDLK_ESCAPE) {
              this->quit = FALSE;
            }
          }
          break;

        case SDL_MOUSEBUTTONDOWN:
          if (this->players[playerId]->getReapparitionTime() < this->currentTime) {
            std::stringstream message;
            message << "TIR " << sint32ToString(this->currentTime);
            this->udpClient->send(message.str());
          }
          break;
      }

      i++;
      if (i >= 100) break; // Avoid getting to many events
  }
}

void Scene::animate(void)
{
  int numberKeys;
  const Uint8* pKeys = SDL_GetKeyboardState(&numberKeys);
  Uint8* keys = new Uint8[numberKeys];
  memcpy(keys, pKeys, numberKeys);

  // Avoid absurd behaviors

  // Going forwards and backwards
  if (keys[SDL_GetScancodeFromKey(SDLK_z)] == keys[SDL_GetScancodeFromKey(SDLK_s)]) {
    keys[SDL_GetScancodeFromKey(SDLK_z)] = 0u;
    keys[SDL_GetScancodeFromKey(SDLK_s)] = 0u;
  }

  // Going left and right
  if (keys[SDL_GetScancodeFromKey(SDLK_q)] == keys[SDL_GetScancodeFromKey(SDLK_d)]) {
    keys[SDL_GetScancodeFromKey(SDLK_q)] = 0u;
    keys[SDL_GetScancodeFromKey(SDLK_d)] = 0u;
  }

  float16 direction;
  bool8 movement = FALSE;

  if (keys[SDL_GetScancodeFromKey(SDLK_z)]) { // Going forwards
    movement = TRUE;

    // In diagonal
    if (keys[SDL_GetScancodeFromKey(SDLK_q)])
      direction = 45.0; // left forwards
    else if (keys[SDL_GetScancodeFromKey(SDLK_d)])
      direction = -45.0; // right forwards
    else
      direction = 0.0; // forwards
  } else if (keys[SDL_GetScancodeFromKey(SDLK_s)]) { // Going backwards
    movement = TRUE;

    // In diagonal
    if (keys[SDL_GetScancodeFromKey(SDLK_q)])
      direction = 135.0; // left backwards
    else if (keys[SDL_GetScancodeFromKey(SDLK_d)])
      direction = -135.0; // right backwards
    else
      direction = 180.0; // backwards
  }

  if (movement == FALSE) {
    if (keys[SDL_GetScancodeFromKey(SDLK_q)]) { // Going left
      movement = TRUE;
      direction = 90.0;
    } else if (keys[SDL_GetScancodeFromKey(SDLK_d)]) { // Going right
      movement = TRUE;
      direction = -90.0;
    }
  }

  if (keys[SDL_GetScancodeFromKey(SDLK_UP)]) {
    this->players[this->playerId]->goUp();
  } else if (keys[SDL_GetScancodeFromKey(SDLK_DOWN)]) {
    this->players[this->playerId]->goDown();
  }

  delete[] keys;

  if (movement) {
    #define VELOCITY (30.0f)

    float16 distance = (float) this->timeLastLoop * VELOCITY / 1000.0f;

    // cutting the distance into segments
    #define DELTA_DISTANCE (0.25f)

    uint32 nbSegments = (uint32) (distance / DELTA_DISTANCE);
    float16 remainingDistance = fmod(distance, DELTA_DISTANCE);

    // For each segment
    for(uint32 i = 0; i < nbSegments; i++) {
      float16 x = 0, y = 0, z = 0;
      this->players[this->playerId]->getPosition(&x, &y, &z);

      bool8 entouragePersonnage[8] = {0};
      this->map->contourn(x, y, entouragePersonnage);

      float16 heightmapHeight = this->heightmap->getHeight(x, y);

      this->players[this->playerId]->move(DELTA_DISTANCE, direction, entouragePersonnage, heightmapHeight);
    }

    // For the remaining distance
    float16 x = 0, y = 0, z = 0;
    this->players[this->playerId]->getPosition(&x, &y, &z);

    bool8 entouragePersonnage[8] = {0};
    this->map->contourn(x, y, entouragePersonnage);

    float16 heightmapHeight = this->heightmap->getHeight(x, y);

    this->players[this->playerId]->move(remainingDistance, direction, entouragePersonnage, heightmapHeight);

    /*
    // SANS GERER LES LAGS

    float16 x = 0, y = 0, z = 0;
    this->players[this->playerId]->getPosition(&x, &y, &z);

    bool8 entouragePersonnage[8] = {0};
    this->map->contourn(x, y, entouragePersonnage);

    float16 heightmapHeight = this->heightmap->getHeight(x, y);

    this->players[this->playerId]->move(distance, direction, entouragePersonnage, heightmapHeight);
    */
  }

  // Envoi de la position au serveur toutes les 50 ms en moyenne
  static sint32 timeLastSnapshot = this->currentTime - DURATION_BETWEEN_SNAPSHOT;

  if (this->currentTime >= timeLastSnapshot + DURATION_BETWEEN_SNAPSHOT) {
    timeLastSnapshot = this->currentTime;

    std::stringstream message;

    message << "POSITION_JOUEUR ";
    message << this->currentTime << " ";
    message << this->players[this->playerId]->getX() << " ";
    message << this->players[this->playerId]->getY() << " ";
    message << this->players[this->playerId]->getZ() << " ";
    message << this->players[this->playerId]->getHorizontalAngle() << " ";
    message << this->players[this->playerId]->getVerticalAngle() << " ";

    this->udpClient->send(message.str());
  }


  // Si on recoit un message
  std::string message = this->udpClient->receive();
  std::string entete = decapsulate(&message);

  if (entete == "POSITION_JOUEUR") {
    std::cout << "RECOIS LA POSITION" << std::endl;

    sint32 heure = stringToSint32(decapsulate(&message));
    float16 positionX = stringToFloat16(decapsulate(&message));
    float16 positionY = stringToFloat16(decapsulate(&message));
    float16 positionZ = stringToFloat16(decapsulate(&message));
    float16 angleHorizontal = stringToFloat16(decapsulate(&message));
    float16 angleVertical = stringToFloat16(decapsulate(&message));

    this->players[playerId]->setPosition(
      positionX,
      positionY,
      positionZ
    );

    this->players[playerId]->setDirection(angleHorizontal, angleVertical);
  }
}

void Scene::display(void)
{
  glFlush();
  SDL_GL_SwapWindow(this->window);
}

void Scene::setUDPClient(UDPClient* udpClient)
{
  this->udpClient = udpClient;
}

void Scene::setClock(sint32 heure)
{
  this->clock.adjust(heure);
}

void Scene::createPlayer()
{
  // Lecture de l'heure actuelle pour determiner l'heure de la premiere apparition
  sint32 currentTime = this->clock.getTime();

  this->udpClient->send("DEMANDE_POSITION_JOUEUR");

  // Ajout de notre personnage (position temporaire si le joueur s'est deja connecte)
  this->players.push_back(new Player(40.0, 40.0, 0.55, -45.0, 0, 0.3, "assets/raphael.m3s"));
}

void Scene::setPlayerId(uint32 playerId)
{
  this->playerId = playerId;
}

void Scene::draw2D(void)
{

}
