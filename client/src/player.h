#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "objet3DStatique.h"
#include <string>
#include "point3Float.h"

class Player : public Objet3DStatique
{
  private:
    float16 radius;
    float16 verticalAngle;

    sint32 reapparitionTime;
    Point3Float reapparitionPoint;
    float16 reapparitionAngle;

    uint32 winningShotsCount;
    uint32 receivedShotsCount;

    std::string username;

  public:
    Player(float16 x, float16 y, float16 z, float16 horizontalAngle, float16 verticalAngle, float16 radius, std::string file);
    ~Player(void);

    // Deplacement
    void move(float16 distance, float16 direction, bool8 entourage[8], float16 heightmapHeight);
    void horizontalTurn(float16 angle);
    void verticalTurn(float16 angle);
    void goUp();
    void goDown();

    void getPosition(float16* x, float16* y, float16* z);
    void look(void);

    void setPosition(float16 x, float16 y, float16 z);
    void setDirection(float16 horizontalAngle, float16 verticalAngle);

    float16 getX();
    float16 getY();
    float16 getZ();
    float16 getHorizontalAngle();
    float16 getVerticalAngle();

    sint32 getReapparitionTime(void);
    void setReapparitionTime(sint32 heure);

    Point3Float getReapparitionPoint(void);
    void setReapparitionPoint(Point3Float point);

    float16 getReapparitionAngle(void);
    void setReapparitionAngle(float16 angle);

    //void add_position(sint32 time, float16 x, float16 y, float z, float16 horizontal_angle, float16 vertical_angle);
    //bool8 positionnerDepuisLHistorique(sint32 heure);
    //bool8 heureDernierePositionConnue(sint32* heure);

    //void afficherHistorique(void);

    uint32 getWinningShots();
    uint32 getReceivedShots();
    void updateScore(uint32 emitted, uint32 received);

    void setUsername(std::string username);
    std::string getUsername(void);

    void draw();
};

#endif
