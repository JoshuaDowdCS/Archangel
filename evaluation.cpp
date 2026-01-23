
#include "evaluation.h"
#include "piece.h"

double Evaluation::materialCount(Board &board, bool forWhite)
{
        double material = 0;

        int color = forWhite ? 0 : 1;

        material += 100 * std::popcount(board.bitboards[color][Piece::PAWN]);
        material += 300 * std::popcount(board.bitboards[color][Piece::BISHOP]);
        material += 300 * std::popcount(board.bitboards[color][Piece::KNIGHT]);
        material += 500 * std::popcount(board.bitboards[color][Piece::ROOK]);
        material += 900 * std::popcount(board.bitboards[color][Piece::KING]);

        return material;
}

double Evaluation::estimatedEvaluation(Board &board)
{
        return 0.0;
}

double Evaluation::evaluate(Board &board)
{
        double whiteMaterial = materialCount(board, true);
        double blackMaterial = materialCount(board, false);

        return board.isWhiteTurn ? whiteMaterial - blackMaterial : blackMaterial - whiteMaterial;
}
