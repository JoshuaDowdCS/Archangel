#pragma once

#include "board.h"

class Evaluation
{
public:
        static double materialCount(Board &board, bool forWhite);
        static double estimatedEvaluation(Board &board);
        static double evaluate(Board &board);
};