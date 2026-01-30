#pragma once
#include <iostream>
struct BoardState
{
        int capturedPiece;
        int passantSquare;
        int castlingRights;
        int moveType; // 0 = normal, 1 = passant, 2 = castling, 3 = promotion
};