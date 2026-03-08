
#include "evaluation.h"
#include "../types/piece.h"

double Evaluation::evaluate(Board &board)
{
        double evaluation = materialCount(board, board.isWhiteTurn) - materialCount(board, !board.isWhiteTurn);

        evaluation += isCastled(board);

        evaluation += doubledPawns(board);

        evaluation += pieceSquareEvaluation(board);

        return board.isWhiteTurn ? evaluation : -evaluation;
}

double Evaluation::materialCount(Board &board, bool forWhite)
{
        double material = 0;

        material += pawnWeight * std::popcount(board.bitboards[!forWhite][Piece::PAWN]);
        material += knightWeight * std::popcount(board.bitboards[!forWhite][Piece::KNIGHT]);
        material += bishopWeight * std::popcount(board.bitboards[!forWhite][Piece::BISHOP]);
        material += rookWeight * std::popcount(board.bitboards[!forWhite][Piece::ROOK]);
        material += queenWeight * std::popcount(board.bitboards[!forWhite][Piece::QUEEN]);

        return material;
}

double Evaluation::doubledPawns(Board &board)
{
        int doubledCount = 0;
        uint64_t pawnBoard = board.bitboards[!board.isWhiteTurn][Piece::PAWN];
        for (int i = 0; i < 8; ++i)
        {
                uint64_t fileMask = 0x0101010101010101ULL << i;
                int pawnsInFile = std::popcount(pawnBoard & fileMask);
                if (pawnsInFile > 1)
                {
                        doubledCount += (pawnsInFile - 1);
                }
        }

        return doubledWeight * doubledCount;
}

double Evaluation::isCastled(Board &board)
{
        return board.hasCastled[!board.isWhiteTurn] * castledWeight;
}

double Evaluation::pieceSquareEvaluation(Board &board)
{
        double evaluation = 0;

        for (int currPiece = Piece::KNIGHT; currPiece < Piece::QUEEN; currPiece++)
        {
                Bitboard currBoard = board.bitboards[!board.isWhiteTurn][currPiece];
                while (currBoard)
                {
                        evaluation += PST[currPiece][std::countr_zero(currBoard)];
                        currBoard &= currBoard - 1;
                }
        }

        return evaluation;
}
