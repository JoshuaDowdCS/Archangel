#include "search.h"
#include "../containers/movelist.h"
#include "evaluation.h"
#include "../types/evaluatedmove.h"
#include "../movegen/movegen.h"

Search::Search(Board &board) : board(board) {}

EvaluatedMove Search::alphaBetaSearch(std::chrono::steady_clock::time_point stopTime, int depth, double alpha, double beta, bool maximizingPlayer, std::vector<Move> &moveLine, bool isRoot)
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
        MoveGen moveGenerator;
        moveGenerator.generateMoves(board, moveList);

        double playerExtreme = (maximizingPlayer ? -1 : 1) * 1000000000;

        if (!moveList.count)
        {
                if (!board.isKingAttacked(board.isWhiteTurn))
                        return {Move(0, 0), 0};

                return {Move(0, 0), playerExtreme - (playerExtreme / 100) * depth};
        }

        EvaluatedMove returner = {Move(0, 0), playerExtreme};

        for (Move move : moveList)
        {
                std::vector<Move> localMoveLine;

                board.makeMove(move);
                double currEval = alphaBetaSearch(stopTime, depth - 1, alpha, beta, !maximizingPlayer, localMoveLine).evaluation;
                board.unmakeMove();

                if (abortSearch)
                {
                        break;
                }

                if (maximizingPlayer ? currEval > returner.evaluation : currEval < returner.evaluation)
                {
                        returner.evaluation = currEval;
                        if (isRoot)
                        {
                                returner.move = move;
                        }

                        moveLine.clear();
                        moveLine.push_back(move);
                        moveLine.insert(moveLine.end(), localMoveLine.begin(), localMoveLine.end());
                }

                if (maximizingPlayer)
                        alpha = std::max(alpha, currEval);
                else
                        beta = std::min(beta, currEval);

                if (beta <= alpha)
                {
                        break;
                }
        }

        return returner;
}

double Search::Quiesce(int alpha, int beta)
{
        double static_eval = Evaluation::evaluate(board);

        // Stand Pat
        double best_value = static_eval;

        if (best_value >= beta)
                return best_value;
        if (best_value > alpha)
                alpha = best_value;

        MoveList captureList;
        MoveGen moveGenerator(true);
        moveGenerator.generateMoves(board, captureList);

        for (Move captureMove : captureList)
        {
                board.makeMove(captureMove);
                double score = -Quiesce(-beta, -alpha);
                board.unmakeMove();

                if (score >= beta)
                        return score;
                if (score > best_value)
                        best_value = score;
                if (score > alpha)
                        alpha = score;
        }

        return best_value;
}