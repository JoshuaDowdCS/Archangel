#ifndef MOVE_H
#define MOVE_H

#include <cstdint>

struct Move
{
    uint16_t data;

    static const uint16_t FROM_MASK = 0b0000000000111111;
    static const uint16_t TO_MASK = 0b0000111111000000;

    // Flags to know if a move type is special or not
    static const uint16_t FLAG_MASK = 0b0011000000000000;
    static const uint16_t CASTLING = 0b0001000000000000;
    static const uint16_t PASSANT = 0b0010000000000000;
    static const uint16_t PROMOTION = 0b0011000000000000;

    // Flags to say what piece we're promoting to
    static const uint16_t PROMOTION_MASK = 0b1100000000000000;
    static const uint16_t KNIGHT = 0b0000000000000000;
    static const uint16_t BISHOP = 0b0100000000000000;
    static const uint16_t ROOK = 0b1000000000000000;
    static const uint16_t QUEEN = 0b1100000000000000;

public:
    Move()
    {
        data = 0;
    }
    Move(int from, int to, uint16_t flag = 0, uint16_t promotion = 0)
    {
        data = from | (to << 6) | flag | promotion;
    }

    int getFrom() const { return data & FROM_MASK; }
    int getTo() const { return (data & TO_MASK) >> 6; }
    int getPromotion() const { return data & PROMOTION_MASK; }
    bool isSpecial() const { return (data & FLAG_MASK) != 0; }

    bool isPromotion() const { return (data & FLAG_MASK) == PROMOTION; }
    bool isCastling() const { return (data & FLAG_MASK) == CASTLING; }
    bool isPassant() const { return (data & FLAG_MASK) == PASSANT; }
};

#endif