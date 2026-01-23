#include "search.h"
#include "movelist.h"
#include "evaluation.h"
#include "movegen.h"
#include <iostream>

double Search::alphaBetaMax(Board &board, int depth, double alpha, double beta, bool isRoot, std::chrono::steady_clock::time_point stopTime)
{
        if (std::chrono::steady_clock::now() > stopTime)
        {
                abortSearch = true;
                return -std::numeric_limits<double>::max();
        }

        // Depth is 0, evaluate
        if (depth == 0)
        {
                std::cout << "Max Depth 0" << '\n';
                return searchAllCaptures(board, stopTime);
        }

        MoveList maxList;
        std::cout << "BETA MAX GENERATING MOVES" << (int)depth << '\n';
        MoveGen::generateMoves(board, maxList);
        std::cout << "BETA MAX MOVE LIST" << '\n';
        maxList.printMoveList();

        // Checkmate or stalemate
        if (maxList.count == 0)
        {
                if (board.isKingAttacked(board.isWhiteTurn))
                {
                        return -std::numeric_limits<double>::max();
                }
                return 0;
        }

        double bestEvaluation = -std::numeric_limits<double>::max();

        // Base evaluation for first move
        if (isRoot)
        {
                bestMove = maxList[0];
                board.makeMove(bestMove);
                bestEvaluation = Evaluation::evaluate(board);
                board.unmakeMove();
        }

        // For all moves
        for (Move move : maxList)
        {
                std::cout << "~~~~~~~~~~~  Starting new move ~~~~~~~~~~~" << '\n';
                board.makeMove(move);
                std::cout << "Max betaMin Call" << '\n';
                double currEvaluation = Search::alphaBetaMin(board, depth - 1, alpha, beta, false, stopTime);
                // If we have a new best evaluation
                if (currEvaluation > bestEvaluation)
                {
                        bestEvaluation = currEvaluation;
                        // Update alpha
                        if (currEvaluation > alpha)
                        {
                                alpha = currEvaluation;
                        }
                        if (isRoot)
                        {
                                bestMove = move;
                        }
                }
                // Update beta
                if (currEvaluation >= beta)
                {
                        return currEvaluation;
                }
                board.unmakeMove();
        }

        return bestEvaluation;
}
double Search::alphaBetaMin(Board &board, int depth, double alpha, double beta, bool isRoot, std::chrono::steady_clock::time_point stopTime)
{
        if (std::chrono::steady_clock::now() > stopTime)
        {
                abortSearch = true;
                return -std::numeric_limits<double>::max();
        }

        // Depth is 0, evaluate
        if (depth == 0)
        {
                std::cout << "Min Depth 0" << '\n';
                return -searchAllCaptures(board, stopTime);
        }

        MoveList minList;
        std::cout << "BETA MIN GENERATING MOVES" << (int)depth << '\n';
        MoveGen::generateMoves(board, minList);
        std::cout << "BETA MIN MOVE LIST" << '\n';
        minList.printMoveList();

        // Checkmate or stalemate
        if (minList.count == 0)
        {
                if (board.isKingAttacked(board.isWhiteTurn))
                {
                        return std::numeric_limits<double>::max();
                }
                return 0;
        }

        double bestEvaluation = std::numeric_limits<double>::max();

        // For all moves
        for (Move move : minList)
        {
                board.makeMove(move);
                std::cout << "Min betaMax Call " << (int)depth << '\n';
                double currEvaluation = Search::alphaBetaMax(board, depth - 1, alpha, beta, false, stopTime);
                // If we have a new best evaluation
                if (currEvaluation < bestEvaluation)
                {
                        bestEvaluation = currEvaluation;
                        // Update alpha
                        if (currEvaluation < beta)
                        {
                                beta = currEvaluation;
                        }
                }
                // Update beta
                if (currEvaluation <= alpha)
                {
                        return currEvaluation;
                }
                board.unmakeMove();
        }

        return bestEvaluation;
}

double Search::searchAllCaptures(Board &board, std::chrono::steady_clock::time_point stopTime)
{
        if (std::chrono::steady_clock::now() > stopTime)
        {
                abortSearch = true;
                return -std::numeric_limits<double>::max();
        }

        double startEvaluation = Evaluation::evaluate(board);

        MoveList list;
        MoveGen::generateMoves(board, list, true);

        if (list.count == 0)
        {
                return startEvaluation;
        }

        double bestEvaluation = startEvaluation;

        for (Move move : list)
        {
                board.makeMove(move);
                std::cout << "searchCap Recursive Call" << '\n';
                double currEvaluation = -Search::searchAllCaptures(board, stopTime);
                bestEvaluation = std::max(bestEvaluation, currEvaluation);
                board.unmakeMove();
        }

        return bestEvaluation;
}