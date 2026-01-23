#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "movelist.h"
#include "movegen.h"

class Search
{

public:
        static Move findBestMove(Board &board, uint8_t depth);
};

#endif