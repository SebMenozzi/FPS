#include "fonctionsUtiles.h"

#include <sstream>

std::string decapsuler(std::string* message)
{
    std::string entete;

    // Recherche de la position de la premiere espace
    size_t finEntete = message->find(' ');

    // S'il n'y a pas d'espace
    if (std::string::npos == finEntete)
    {
        // L'entete est le message lui meme
        entete = *message;

        // Le message n'encapsule rien. Il devient vide
        message->clear();

        return entete;
    }

    // Recuperation de l'entete
    entete = message->substr(0, finEntete);

    // Suppression de l'entete au message
    *message = message->substr(finEntete+1);

    return entete;
}

sint32 stringEnSint32(std::string chaine)
{
    std::stringstream flux;
    flux << chaine;
    sint32 valeur;
    flux >> valeur;

    return valeur;
}

float16 stringEnFloat16(std::string chaine)
{
    std::stringstream flux;
    flux << chaine;
    float16 valeur;
    flux >> valeur;

    return valeur;
}

std::string sint32EnString(sint32 valeur)
{
    std::stringstream flux;
    flux << valeur;
    std::string chaine;
    flux >> chaine;

    return chaine;
}

std::string uint32EnString(uint32 valeur)
{
    std::stringstream flux;
    flux << valeur;
    std::string chaine;
    flux >> chaine;

    return chaine;
}

