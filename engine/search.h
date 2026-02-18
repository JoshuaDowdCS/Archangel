#pragma once

#include "../movegen/board.h"
#include "../types/evaluatedmove.h"

class Search
{
public:
        bool abortSearch = false;

        EvaluatedMove simpleSearch(Board &board, std::chrono::steady_clock::time_point stopTime, int depth, bool isRoot);
};