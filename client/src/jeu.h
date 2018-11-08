#ifndef JEU_H_INCLUDED
#define JEU_H_INCLUDED

#include <SDL2/SDL.h>
#include "types.h"
#include "fichierINI.h"

class Jeu
{
  public:
    Jeu(void);
    ~Jeu(void);
    bool8 executer(void);

  private:
    void initialiserSDL(void);
    void initialiserSDL_ttf(void);
    bool8 creerFenetreOpenGL(void);
    void detruireFenetreOpenGL(void);

    // Definition de la fenetre
    SDL_Window* fenetre;
    SDL_GLContext contexteOpenGL;
    uint32 largeurFenetre;
    uint32 hauteurFenetre;

    // Fichier de configuration
    FichierINI fichierIniJeu;
};

#endif // JEU_H_INCLUDED
