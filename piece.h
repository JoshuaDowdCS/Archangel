#ifndef PIECE_H
#define PIECE_H

#include <cstdint>

class Piece
{
public:
    static const uint8_t PAWN = 1;
    static const uint8_t KNIGHT = 2;
    static const uint8_t BISHOP = 3;
    static const uint8_t ROOK = 4;
    static const uint8_t QUEEN = 5;
    static const uint8_t KING = 6;
    static const uint8_t TYPE_MASK = 7;

    static const uint8_t WHITE = 0;
    static const uint8_t BLACK = 8;
    static const uint8_t COLOR_MASK = 8;
};

#endif