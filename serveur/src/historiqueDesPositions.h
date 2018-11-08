#ifndef HISTORIQUEDESPOSITIONS_H_INCLUDED
#define HISTORIQUEDESPOSITIONS_H_INCLUDED

#include <vector>
#include "types.h"

class HistoriqueDesPositions
{
    struct Position
    {
        sint32 heure;
        float16 x;
        float16 y;
        float16 angle;
    };

    public:
        HistoriqueDesPositions();
        ~HistoriqueDesPositions();

        void ajouter(sint32 heure, float16 x, float16 y, float16 angle);
        bool8 calculer(sint32 heure, float16* x, float16* y, float16* angle);

        void afficher();


    private:
        std::vector<HistoriqueDesPositions::Position> positions;


};

#endif // HISTORIQUEDESPOSITIONS_H_INCLUDED
