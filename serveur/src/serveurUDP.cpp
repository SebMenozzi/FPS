#include "serveurUDP.h"
#include <stdio.h>
#include <cstring>

ServeurUDP::ServeurUDP(uint16 portEcoute)
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

  printf("Creation du ServeurUDP...\n");

  // Socket
  this->socketUdp = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  // Socket non-bloquant
  uint32 arg = 1;
  #ifdef __WIN32__
    ioctlsocket (this->socketUdp, FIONBIO, &arg);
  #else
    fcntl (this->socketUdp, F_SETFL, fcntl (this->socketUdp, F_GETFL) | O_NONBLOCK);
  #endif

  // Adresse
  this->adresse_udp.sin_family = AF_INET;
  this->adresse_udp.sin_port = htons (portEcoute);
  this->adresse_udp.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(&this->adresse_udp.sin_zero, 0, 8);

  // Ligature
  if (-1 == bind(this->socketUdp, (SOCKADDR*)&this->adresse_udp, sizeof(this->adresse_udp)))
  {
    printf("Erreur lors de la creation du ServeurUDP\n");
    return;
  }

  printf("ServeurUDP cree avec succes\n");
}

std::string ServeurUDP::recevoir(std::string* ipExpediteur, uint16* portExpediteur)
{
  SOCKADDR_IN adresseExpediteur;
  SOCKLEN_T tailleAdresseExpediteur;
  char message[TAILLE_MAX_MESSAGE] = {0};

  // Reception d'un eventuel message
  tailleAdresseExpediteur = sizeof(adresseExpediteur);
  if (0 < recvfrom(this->socketUdp, message, TAILLE_MAX_MESSAGE, 0, (struct sockaddr*)&adresseExpediteur, &tailleAdresseExpediteur))
  {
    *portExpediteur = ntohs(adresseExpediteur.sin_port);
    *ipExpediteur = inet_ntoa(adresseExpediteur.sin_addr);

    return message;
  }

  return "";
}

void ServeurUDP::envoyer(std::string message, std::string ipDestinataire, uint16 portDestinataire)
{
  // Adresse du destinataire
  SOCKADDR_IN adresseDestinataire;
  adresseDestinataire.sin_family = AF_INET;
  adresseDestinataire.sin_port = htons(portDestinataire);
  memset(&adresseDestinataire.sin_zero, 0, 8);

  hostent* infosDestinataire = gethostbyname(ipDestinataire.c_str());
  adresseDestinataire.sin_addr.s_addr = ((in_addr*)infosDestinataire->h_addr)->s_addr;

  if (0 == adresseDestinataire.sin_addr.s_addr)
  {
    adresseDestinataire.sin_addr.s_addr = inet_addr(ipDestinataire.c_str());
  }

  // Envoi
  if(-1 == sendto(this->socketUdp, message.c_str(), message.length() + 1, 0, (sockaddr*)&adresseDestinataire, sizeof(adresseDestinataire)))
  {
    printf("Erreur lors de l'envoi du message\n");
  }
}
