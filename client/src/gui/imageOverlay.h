#ifndef IMAGEOVERLAY_H_INCLUDED
#define IMAGEOVERLAY_H_INCLUDED

#include "widgetOverlay.h"
#include "../conteneurTextures.h"

class ImageOverlay : public WidgetOverlay
{
  public:
    ImageOverlay(SDL_Window* fenetre, std::string image, sint32 x, sint32 y);
    ~ImageOverlay(void);

    void dessiner(void);
  private:
    std::string fichierImage;
    //ConteneurTextures conteneurTextures;

    void genererTexture(void);

    GLuint textureImage;
    //uint32 largeurImage;
    //uint32 hauteurImage;
    //uint32 largeurTexture;
    //uint32 hauteurTexture;
    //float16 largeurPortionUtile;
    //float16 hauteurPortionUtile;

};

#endif // IMAGEOVERLAY_H_INCLUDED
