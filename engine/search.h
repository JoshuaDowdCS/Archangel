#pragma once

#include "../movegen/board.h"
#include "../types/evaluatedmove.h"
#include "../types/move.h"

#include <chrono>
#include <vector>

class Search
{
public:
        bool abortSearch = false;
        bool foundMate = false;
        uint64_t nodesVisited = 0;
        Board board;

        Search(Board& board);

        EvaluatedMove alphaBetaSearch(std::chrono::steady_clock::time_point stopTime, int depth, double alpha, double beta, bool maximizingPlayer, std::vector<Move>& moveLine, bool isRoot = false);

        double Quiesce(int alpha, int beta);
};