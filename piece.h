#pragma once

namespace Piece
{
    inline constexpr int PAWN = 1;
    inline constexpr int KNIGHT = 2;
    inline constexpr int BISHOP = 3;
    inline constexpr int ROOK = 4;
    inline constexpr int QUEEN = 5;
    inline constexpr int KING = 6;
    inline constexpr int TYPE_MASK = 7;

    inline constexpr int WHITE = 0;
    inline constexpr int BLACK = 8;
    inline constexpr int COLOR_MASK = 8;
};
