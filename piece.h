#ifndef PIECE_H
#define PIECE_H

#include <cstdint>

class Piece
{
public:
    static const uint8_t PAWN = 0b001;
    static const uint8_t KNIGHT = 0b010;
    static const uint8_t BISHOP = 0b011;
    static const uint8_t ROOK = 0b100;
    static const uint8_t QUEEN = 0b101;
    static const uint8_t KING = 0b110;
    static const uint8_t TYPE_MASK = 0b111;

    static const uint8_t WHITE = 0b0000;
    static const uint8_t BLACK = 0b1000;
    static const uint8_t COLOR_MASK = 0b1000;
};

#endif