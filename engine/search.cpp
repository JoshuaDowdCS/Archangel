#include "search.h"
#include "../containers/movelist.h"
#include "evaluation.h"
#include "../types/evaluatedmove.h"
#include "../movegen/movegen.h"

EvaluatedMove Search::alphaBetaSearch(Board &board, std::chrono::steady_clock::time_point stopTime, int depth, double alpha, double beta, bool maximizingPlayer, bool isRoot = false)
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
                return {Move(0, 0), (maximizingPlayer ? -1 : 1) * std::numeric_limits<double>::max() * board.isKingAttacked(board.isWhiteTurn)};
        }

        EvaluatedMove returner = {Move(0, 0), 0};

        if (maximizingPlayer)
        {
                returner.evaluation = -std::numeric_limits<double>::max();

                for (Move move : moveList)
                {
                        board.makeMove(move);
                        double currEval = alphaBetaSearch(board, stopTime, depth - 1, alpha, beta, false).evaluation;
                        board.unmakeMove();

                        if (abortSearch)
                                break;

                        if (currEval > returner.evaluation)
                        {
                                returner.evaluation = currEval;
                                if (isRoot)
                                {
                                        returner.move = move;
                                }
                        }

                        alpha = std::max(alpha, currEval);

                        if (beta <= alpha)
                        {
                                break;
                        }
                }

                return returner;
        }
        else
        {
                returner.evaluation = std::numeric_limits<double>::max();

                for (Move move : moveList)
                {
                        board.makeMove(move);
                        double currEval = alphaBetaSearch(board, stopTime, depth - 1, alpha, beta, true).evaluation;
                        board.unmakeMove();

                        if (abortSearch)
                                break;

                        if (currEval < returner.evaluation)
                        {
                                returner.evaluation = currEval;
                                if (isRoot)
                                {
                                        returner.move = move;
                                }
                        }

                        beta = std::min(beta, currEval);

                        if (beta <= alpha)
                        {
                                break;
                        }
                }

                return returner;
        }
}