#pragma once

#include "../movegen/board.h"
#include "../types/evaluatedmove.h"
#include "../containers/movelist.h"
#include "../types/move.h"

class Search
{
public:
        bool abortSearch = false;
        bool foundMate = false;
        Board board;

        Search(Board &board);

        EvaluatedMove alphaBetaSearch(std::chrono::steady_clock::time_point stopTime, int depth, double alpha, double beta, bool maximizingPlayer, std::vector<Move> &moveLine, bool isRoot = false);

        double Quiesce(int alpha, int beta);
};