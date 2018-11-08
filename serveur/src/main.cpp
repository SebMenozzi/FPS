#include "serveurUDP.h"
#include "horloge.h"
#include <sstream>
#include <vector>
#include <iostream>
#include "ficheClient.h"
#include "fonctionsUtiles.h"

int main(int argc, char* argv[])
{
  // Initialisation du generateur pseudo aleatoire
  srand(time(NULL));
  {
    ServeurUDP serveurUdp(2712);

    sint32 heureProchainsMessages = 0;

    Horloge horlogeReference;

    std::vector<FicheClient> fichesClients;

    sint32 heureProchainePartie = 0;
    sint32 dureeProchainePartie = 90000;

    while(1)
    {
      std::string ip;
      uint16 port;
      std::string messageRecu = serveurUdp.recevoir(&ip, &port);

      // Si on a recu un message
      if(messageRecu != "")
      {
        // Lecture de l'entete
        std::string entete = decapsuler(&messageRecu);

        // Si un client demande l'heure
        if (entete == "DEMANDE_HEURE")
        {
          // On lui donne l'heure
          serveurUdp.envoyer("REPONSE_HEURE " + decapsuler(&messageRecu) + " " + sint32EnString(horlogeReference.heure()), ip, port);
        }

        // Si un client nous envoi son pseudo
        if (entete == "PSEUDO")
        {
          bool8 laFicheClientExiste = FALSE;

          // Recherche de la fiche client
          for(uint32 i = 0; i < fichesClients.size(); i++)
          {
            FicheClient& ficheClient = fichesClients[i];
            // Si on est en pocession de la fiche client
            if (ficheClient.ip == ip && ficheClient.port == port)
            {
              // Mise a jour du pseudo
              ficheClient.pseudo = decapsuler(&messageRecu);
              // Mise a jour de l'heure du dernier message
              ficheClient.heureDernierMessage = horlogeReference.heure();
              laFicheClientExiste = TRUE;
              break;
            }
          }

          // Si la fiche client n'existe pas
          if (laFicheClientExiste == FALSE)
          {
            // S'il a encore le temps de se connecter
            if (0 == heureProchainePartie || (heureProchainePartie != 0 && horlogeReference.heure() < heureProchainePartie - 2000 ) )
            {
              // Ajout d'une nouvelle fiche client
              FicheClient ficheClient;
              ficheClient.pseudo = decapsuler(&messageRecu);
              ficheClient.ip = ip;
              ficheClient.port = port;
              ficheClient.heureDernierMessage = horlogeReference.heure();
              fichesClients.push_back(ficheClient);
            }
          }
        }
      }

      // S'il est l'heure d'envoyer les prochains messages
      if(horlogeReference.heure() >= heureProchainsMessages)
      {
        // Suppression des fiches clients devenues obsoletes
        for(uint32 i = 0; i < fichesClients.size(); i++)
        {
          if(fichesClients[i].heureDernierMessage < horlogeReference.heure() - 1000)
          {
            fichesClients.erase(fichesClients.begin()+i);
            i--;
          }
        }

        // Affichage des fiches clients
        for(uint32 i = 0; i < fichesClients.size(); i++)
        {
          std::cout << fichesClients[i].ip << std::endl;
          std::cout << fichesClients[i].port << std::endl;
          std::cout << fichesClients[i].pseudo << std::endl;
          std::cout << fichesClients[i].heureDernierMessage << std::endl;
          std::cout << std::endl;
        }

        std::cout << "------------------" << std::endl;

        // Envoi de la liste des joueurs a tous les joueurs
        std::string listeJoueurs;
        listeJoueurs = "LISTE_JOUEURS";
        for(uint32 i = 0; i < fichesClients.size(); i++)
        {
          listeJoueurs += " ";
          listeJoueurs += fichesClients[i].pseudo;
        }
        for(uint32 i = 0; i < fichesClients.size(); i++)
        {
          serveurUdp.envoyer(listeJoueurs, fichesClients[i].ip, fichesClients[i].port);
        }

        // Envoi du numero de joueur a chacun des joueurs pour savoir qui ils sont
        for(uint32 i = 0; i < fichesClients.size(); i++)
        {
          std::string messageNumeroJoueur;
          messageNumeroJoueur = "NUMERO_JOUEUR ";
          messageNumeroJoueur += sint32EnString(i);
          serveurUdp.envoyer(messageNumeroJoueur, fichesClients[i].ip, fichesClients[i].port);
        }

        // Si l'heure de la prochaine partie n'est pas encore definie
        if (heureProchainePartie == 0)
        {
          // S'il y a au moins deux joueurs
          #define NB_JOUEUR_MIN_POUR_JOUER 2
          if (fichesClients.size() >= NB_JOUEUR_MIN_POUR_JOUER)
          {
            // Planification d'une heure de debut de partie
            heureProchainePartie = horlogeReference.heure() + 15000;
          }
        }
        // Si une partie est planifie
        else
        {
          // et qu'il n'y a pas au moins deux joueurs
          if (fichesClients.size() < NB_JOUEUR_MIN_POUR_JOUER)
          {
            // Annulation de la prochaine partie, en attente de joueurs
            heureProchainePartie = 0;
          }
        }

        // Envoi de l'heure de debut de la partie a tout les clients
        for(uint32 i = 0; i < fichesClients.size(); i++)
        {
          std::string message;
          message = "HEURE_PROCHAINE_PARTIE " + sint32EnString(heureProchainePartie);
          serveurUdp.envoyer(message, fichesClients[i].ip, fichesClients[i].port);

          // Duree de la partie
          message = "HEURE_FIN_PROCHAINE_PARTIE " + sint32EnString(heureProchainePartie + dureeProchainePartie);
          serveurUdp.envoyer(message, fichesClients[i].ip, fichesClients[i].port);
        }

        // Les prochains messages un peu plus tard
        heureProchainsMessages += 500;
      }

      // S'il est l'heure de jouer, on joue
      if (heureProchainePartie != 0 && horlogeReference.heure() >= heureProchainePartie)
      {
        /*
        Scene scene;
        //scene.reglerHorloge(horlogeReference.heure());
        //scene.serveurUDPAUtiliser(&serveurUdp);
        scene.creerPersonnages(fichesClients.size());
        scene.creerTableauScores(fichesClients.size());
        scene.creerFichesClients(fichesClients);
        scene.reglerHeureFinPartie(heureProchainePartie + dureeProchainePartie);
        scene.executer();

        heureProchainePartie = 0;

        heureProchainsMessages = horlogeReference.heure() + 1500;
        */
      }
    }
  }

  return EXIT_SUCCESS;
}
