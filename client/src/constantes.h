#ifndef CONSTANTES_H_INCLUDED
#define CONSTANTES_H_INCLUDED

/*
enum
{
    BOUTON_CLIQUE = SDL_USEREVENT,
    REDESSINER_MENU,
    ETAT_CONNEXION,
};
*/

// Les differents etats que peu prendre la connexion au serveur
enum { DECONNECTE, CONNECTE, CONNEXION_EN_COURS, ERREUR_CONNEXION };

// Les differents etats que peu prendre la partie
enum { EN_ATTENTE_DEMARRAGE, EN_COURS };

#endif // CONSTANTES_H_INCLUDED
