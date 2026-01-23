#pragma once

#include "board.h"

class Search
{
public:
        Move bestMove = Move(0, 0);
        bool abortSearch = false;

        double alphaBetaMax(Board &board, int depth, double alpha, double beta, bool isRoot, std::chrono::steady_clock::time_point stopTime);
        double alphaBetaMin(Board &board, int depth, double alpha, double beta, bool isRoot, std::chrono::steady_clock::time_point stopTime);

        double searchAllCaptures(Board &board, std::chrono::steady_clock::time_point stopTime);
};