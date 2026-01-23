#ifndef MOVELIST_H
#define MOVELIST_H

#include <cstdint>
#include "move.h"

struct MoveList
{
    Move moveList[256];
    int count = 0;

    void add(Move m)
    {
        moveList[count++] = m;
    }

    void clear()
    {
        count = 0;
    }
};

#endif