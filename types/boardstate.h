#pragma once
struct BoardState
{
        int capturedPiece;
        int passantSquare;
        int castlingRights;
        int moveType; // Normal is 0, Castling 1, Passant 2, Promotion 3
};