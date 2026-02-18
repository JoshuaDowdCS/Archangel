
#include "evaluation.h"
#include "../types/piece.h"

double Evaluation::materialCount(Board &board, bool forWhite)
{
        double material = 0;

        material += 100 * std::popcount(board.bitboards[!forWhite][Piece::PAWN]);
        material += 300 * std::popcount(board.bitboards[!forWhite][Piece::BISHOP]);
        material += 300 * std::popcount(board.bitboards[!forWhite][Piece::KNIGHT]);
        material += 500 * std::popcount(board.bitboards[!forWhite][Piece::ROOK]);
        material += 900 * std::popcount(board.bitboards[!forWhite][Piece::QUEEN]);

        return material;
}

#include <random>
#include <ctime>

double Evaluation::evaluate(Board &board)
{
        static std::mt19937 generator(static_cast<unsigned>(std::time(nullptr)));

        std::uniform_real_distribution<double> distribution(-1, 1);
        double jiggle = distribution(generator);

        double whiteMaterial = materialCount(board, true);
        double blackMaterial = materialCount(board, false);

        double relativeScore = whiteMaterial - blackMaterial;

        return (board.isWhiteTurn ? relativeScore : -relativeScore) + jiggle;
}