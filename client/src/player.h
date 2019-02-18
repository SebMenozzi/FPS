#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "objet3DStatique.h"
#include <string>
#include "historiqueDesPositions.h"
#include "point3Float.h"

#include "objet3D.h"

class Player : public Objet3DStatique
{
  private:
    float16 radius;
    float16 vertical_angle;

    HistoriqueDesPositions positions;

    sint32 reapparition_time;
    Point3Float reapparition_point;
    float16 reapparition_angle;

    uint32 winning_shots_count;
    uint32 received_shots_count;

    std::string username;

    Objet3D* objet3D;

  public:
    Player(float16 x, float16 y, float16 z, float16 horizontal_angle, float16 vertical_angle, float16 radius, std::string file);
    ~Player(void);

    // Deplacement
    void move(float16 distance, float16 direction, bool8 entourage[8], float16 heightmap_height);
    void horizontal_turn(float16 angle);
    void vertical_turn(float16 angle);
    void go_up();
    void go_down();

    void get_position(float16* x, float16* y, float16* z);
    void look(void);

    void set_position(float16 x, float16 y, float16 z);
    void set_direction(float16 horizontal_turn, float16 vertical_turn);

    float16 get_x();
    float16 get_y();
    float16 get_z();
    float16 get_horizontal_angle();
    float16 get_vertical_angle();

    sint32 get_reapparition_time(void);
    void set_reapparition_time(sint32 heure);

    Point3Float get_reapparition_point(void);
    void set_reapparition_point(Point3Float point);

    float16 get_reapparition_angle(void);
    void set_reapparition_angle(float16 angle);

    //void add_position(sint32 time, float16 x, float16 y, float z, float16 horizontal_angle, float16 vertical_angle);
    //bool8 positionnerDepuisLHistorique(sint32 heure);
    //bool8 heureDernierePositionConnue(sint32* heure);

    //void afficherHistorique(void);

    uint32 get_winning_shots();
    uint32 get_received_shots();
    void update_score(uint32 emitted, uint32 received);

    void set_username(std::string username);
    std::string get_username(void);

    void draw();
};

#endif
