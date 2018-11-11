#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "serveurUDP.h"
#include "horloge.h"
#include "types.h"
#include "ficheClient.h"
#include "fonctionsUtiles.h"
#include "fichierINI.h"

FichierINI fichierIni;

int main(int argc, char* argv[])
{
  ServeurUDP serveurUdp(2712);

  sint32 heureProchainsMessages = 0;

  Horloge horlogeReference;

  std::vector<FicheClient> fichesClients;

  while(TRUE)
  {
    std::string ip;
    uint16 port;
    std::string messageRecu = serveurUdp.recevoir(&ip, &port);

    // Si on a recu un message
    if(messageRecu != "")
    {
      // Lecture de l'entete
      std::string entete = decapsuler(&messageRecu);

      // le client demande l'heure
      if (entete == "DEMANDE_HEURE")
      {
        // On lui donne l'heure
        serveurUdp.envoyer("REPONSE_HEURE " + decapsuler(&messageRecu) + " " + sint32EnString(horlogeReference.heure()), ip, port);
      }

      // le client se connecte
      if (entete == "CONNEXION")
      {
        std::string pseudo = decapsuler(&messageRecu);
        // pseudo en minuscule
        std::transform(pseudo.begin(), pseudo.end(), pseudo.begin(), ::tolower);

        bool8 laFicheClientExiste = FALSE;

        // Recherche de la fiche client
        for(uint32 i = 0; i < fichesClients.size(); i++)
        {
          FicheClient& ficheClient = fichesClients[i];
          // Si on est en pocession de la fiche client (meme ip et meme pseudo)
          if (ficheClient.ip == ip && ficheClient.pseudo == pseudo)
          {
            // Mise a jour du port
            ficheClient.port = port;
            // Mise a jour de l'heure du dernier message
            ficheClient.heureDernierMessage = horlogeReference.heure();
            laFicheClientExiste = TRUE;
            break;
          }
        }

        // Si la fiche client n'existe pas
        if (laFicheClientExiste == FALSE)
        {
          std::cout << ">> Bienvenue au joueur: " << pseudo << " !";
          std::cout << "(" << ip << ":" << port << ")" << std::endl;

          // Ajout d'une nouvelle fiche client
          FicheClient ficheClient;
          ficheClient.pseudo = pseudo;
          ficheClient.ip = ip;
          ficheClient.port = port;
          ficheClient.heureDernierMessage = horlogeReference.heure();
          fichesClients.push_back(ficheClient);
        }
      }

      if (entete == "DEMANDE_JOUEURS")
      {
        // Envoi de la liste des joueurs
        std::string listeJoueurs = "LISTE_JOUEURS";

        for(uint32 i = 0; i < fichesClients.size(); i++)
        {
          listeJoueurs += " ";
          listeJoueurs += fichesClients[i].pseudo;
        }

        // On lui donne la liste
        serveurUdp.envoyer(listeJoueurs, ip, port);
      }

      if (entete == "DECONNEXION")
      {
        // Recherche de la fiche client
        for(uint32 i = 0; i < fichesClients.size(); i++)
        {
          FicheClient& ficheClient = fichesClients[i];
          // Si on est en pocession de la fiche client (meme ip et meme port)
          if (ficheClient.ip == ip && ficheClient.port == port)
          {
            std::cout << ">> Aurevoir " << ficheClient.pseudo << " !";
            std::cout << "(" << ficheClient.ip << ":" << ficheClient.port << ")" << std::endl;
            // On supprime le client de la fiche clients
            fichesClients.erase(fichesClients.begin() + i);
            // On enregistre sa dernière position
            std::string chemin = "joueurs/" + ficheClient.pseudo + "/";
            fichierIni.enregistrer(chemin + "position.ini");
            break;
          }
        }
      }

      if (entete == "POSITION_JOUEUR")
      {
        std::string heure = decapsuler(&messageRecu);
        std::string positionX = decapsuler(&messageRecu);
        std::string positionY = decapsuler(&messageRecu);
        std::string positionZ = decapsuler(&messageRecu);
        std::string angleHorizontal = decapsuler(&messageRecu);
        std::string angleVertical = decapsuler(&messageRecu);

        for(uint32 i = 0; i < fichesClients.size(); i++)
        {
          FicheClient& ficheClient = fichesClients[i];
          // Si on est en pocession de la fiche client (meme ip et meme port)
          if (ficheClient.ip == ip && ficheClient.port == port)
          {
            // Mise a jour de l'heure du dernier message
            ficheClient.heureDernierMessage = horlogeReference.heure();

            // le répertoire joueurs n'existe pas
            if(dossierExiste("joueurs") != 0)
            {
              creerDossier("joueurs");
            }

            std::string chemin = "joueurs/" + ficheClient.pseudo + "/";
            // le répertoire joueurs/pseudo n'existe pas
            if(dossierExiste(chemin) != 0)
            {
              creerDossier(chemin);
            }

            // On initialise et on charge le fichierIni
            fichierIni.charger(chemin + "position.ini");

            // On modifie le fichier avec la position du joueur
            fichierIni.modifier("heure", heure);
            fichierIni.modifier("x", positionX);
            fichierIni.modifier("y", positionY);
            fichierIni.modifier("z", positionZ);
            fichierIni.modifier("angleHorizontal", angleHorizontal);
            fichierIni.modifier("angleVertical", angleVertical);
            break;
          }
        }
      }

      if (entete == "DEMANDE_POSITION_JOUEUR")
      {
        std::cout << "DEMANDE LA POSITION" << std::endl;

        for(uint32 i = 0; i < fichesClients.size(); i++)
        {
          FicheClient& ficheClient = fichesClients[i];
          // Si on est en pocession de la fiche client (meme ip et meme port)
          if (ficheClient.ip == ip && ficheClient.port == port)
          {
            std::string chemin = "joueurs/" + ficheClient.pseudo + "/";

            if (fichierIni.charger(chemin + "position.ini"))
            {
              // Envoi de la position
              std::string position = "POSITION_JOUEUR ";
              position += fichierIni.lire("heure") + " ";
              position += fichierIni.lire("x") + " ";
              position += fichierIni.lire("y") + " ";
              position += fichierIni.lire("z") + " ";
              position += fichierIni.lire("angleHorizontal") + " ";
              position += fichierIni.lire("angleVertical");

              serveurUdp.envoyer(position, ip, port);
            }
            break;
          }
        }
      }
    }

    // Suppression des fiches clients devenues obsoletes
    for(uint32 i = 0; i < fichesClients.size(); i++)
    {
      if(fichesClients[i].heureDernierMessage < horlogeReference.heure() - 20000)
      {
        std::cout << ">> Deconnexion automatique: " << fichesClients[i].pseudo << " ";
        std::cout << "(" << fichesClients[i].ip << ":" << fichesClients[i].port << ")" << std::endl;
        // On supprime le client de la fiche clients
        fichesClients.erase(fichesClients.begin() + i);
        // On enregistre sa dernière position
        std::string chemin = "joueurs/" + fichesClients[i].pseudo + "/";
        fichierIni.enregistrer(chemin + "position.ini");
        i--;
      }
    }
  }

  return EXIT_SUCCESS;
}
