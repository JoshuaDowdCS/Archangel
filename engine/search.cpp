#include "search.h"
#include "movelist.h"
#include "evaluation.h"
#include "movegen.h"

EvaluatedMove Search::simpleSearch(Board &board, std::chrono::steady_clock::time_point stopTime, int depth, bool isRoot = false)
{
        if (std::chrono::steady_clock::now() > stopTime)
        {
                abortSearch = true;
                return {Move(0, 0), 0};
        }

        if (depth == 0)
        {
                return {Move(0, 0), Evaluation::evaluate(board)};
        }

        MoveList moveList;
        MoveGen::generateMoves(board, moveList);

        if (!moveList.count)
        {
                return {Move(0, 0), -std::numeric_limits<double>::max() * board.isKingAttacked(board.isWhiteTurn)};
        }

        double bestEvaluation = -std::numeric_limits<double>::max();
        Move bestMove = Move(0, 0);

        for (Move move : moveList)
        {
                board.makeMove(move);
                double evaluation = -simpleSearch(board, stopTime, depth - 1).evaluation;
                board.unmakeMove();

                if (evaluation > bestEvaluation)
                {
                        if (isRoot)
                        {
                                bestMove = move;
                        }
                        bestEvaluation = evaluation;
                }
        }

        return {bestMove, bestEvaluation};
}
