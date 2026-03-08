#pragma once

#include "../movegen/board.h"
#include "../types/evaluatedmove.h"
#include "../containers/movelist.h"

class Search
{
public:
        bool abortSearch = false;

        EvaluatedMove alphaBetaSearch(Board &board, std::chrono::steady_clock::time_point stopTime, int depth, double alpha, double beta, bool maximizingPlayer, bool isRoot);
};