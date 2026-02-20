
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

double Evaluation::evaluate(Board &board)
{
        return materialCount(board, true) - materialCount(board, false);
}