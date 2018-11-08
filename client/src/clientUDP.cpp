#include "clientUDP.h"
#include <cstdio>
#include <cstring>

ClientUDP::ClientUDP()
{
  static bool8 reseauInitialise = FALSE;

  // Si le reseau n'a pas ete initialise
  if (FALSE == reseauInitialise)
  {
    #ifdef __WIN32__
      // Initialisation du reseau
      WSADATA WSAData;
      WSAStartup(MAKEWORD(2,0), &WSAData);
    #endif
      // Le reseau a ete initialise
      reseauInitialise = TRUE;
  }

  printf("Creation du ClientUDP...\n");

  // Socket
  this->socketUdp = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  // Socket non-bloquant
  #ifdef __WIN32__
      uint32 arg = 1;
      ioctlsocket (this->socketUdp, FIONBIO, &arg);
  #else
      fcntl (this->socketUdp, F_SETFL, fcntl (this->socketUdp, F_GETFL) | O_NONBLOCK);
  #endif

  printf("ClientUDP cree avec succes\n");
}

void ClientUDP::connect(std::string ipServeur, uint16 portServeur)
{
  // Adresse
  this->adresseServeur.sin_family = AF_INET;
  this->adresseServeur.sin_port = htons (portServeur);
  memset(&this->adresseServeur.sin_zero, 0, 8);

  hostent* infosServeur = gethostbyname(ipServeur.c_str());
  if(NULL != infosServeur)
  {
    this->adresseServeur.sin_addr.s_addr = ((in_addr*)infosServeur->h_addr)->s_addr;
    if (0 == this->adresseServeur.sin_addr.s_addr)
    {
      this->adresseServeur.sin_addr.s_addr = inet_addr(ipServeur.c_str());
    }
  }
  else
  {
    // Erreur dans la resolution du nom de serveur
    this->adresseServeur.sin_addr.s_addr = 0;
  }
}

std::string ClientUDP::recevoir()
{
  SOCKADDR_IN adresseExpediteur;
  SOCKLEN_T tailleAdresseExpediteur;
  char message[TAILLE_MAX_MESSAGE] = {0};

  // Reception d'un eventuel message
  tailleAdresseExpediteur = sizeof(adresseExpediteur);
  if (0 < recvfrom(this->socketUdp, message, TAILLE_MAX_MESSAGE, 0, (struct sockaddr*)&adresseExpediteur, &tailleAdresseExpediteur))
  {
    return message;
  }

  return "";
}

void ClientUDP::envoyer(std::string message)
{
  // Si on est connecte
  if(0 != this->adresseServeur.sin_addr.s_addr)
  {
    // Envoi
    if(-1 == sendto(this->socketUdp, message.c_str(), message.length() + 1, 0, (sockaddr*)&this->adresseServeur, sizeof(this->adresseServeur)))
    {
      printf("Erreur lors de l'envoi du message\n");
    }
  }
}
