#pragma once

#include "../movegen/board.h"

class Evaluation
{
public:
        static double materialCount(Board &board, bool forWhite);
        static double evaluate(Board &board);
};