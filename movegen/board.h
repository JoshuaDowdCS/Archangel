#pragma once

#include "../containers/movelist.h"
#include "../types/boardstate.h"
#include "../types/move.h"
#include "../types/types.h"
#include <string>
#include <vector>

class Board
{

public:
        std::string startingFen;

        Bitboard bitboards[2][8]; // 0 is empty, 1-6 holds piece type, 7 is combined. White is 0, 1 is black
        Bitboard allCombined;
        int castlingRights;                  // stored as 0bXXXX. 1000 is white short, 0100 is WL, 0010 is BS, and 0001 is BL
        bool hasCastled[2] = {false, false}; // White is 0, 1 is black
        int passantSquare;
        bool resetPassantSquare;
        int halfmoveClock;
        bool isWhiteTurn;
        int fullmoveCounter;
        int threefoldCount = 0;
        uint64_t threefoldArr[21] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
        int hash = 0b00110100111000100110011110110100;

        std::vector<Move> moveHistory;
        std::vector<BoardState> previousMoves;

        static inline constexpr uint64_t castleRookMasks[64] = {
            [2] = 0x9ULL,                // White Queenside (1ULL << 3 | 1ULL << 0)
            [6] = 0xA0ULL,               // White Kingside  (1ULL << 5 | 1ULL << 7)
            [58] = 0x900000000000000ULL, // Black Queenside (1ULL << 59 | 1ULL << 56)
            [62] = 0xA000000000000000ULL // Black Kingside  (1ULL << 61 | 1ULL << 63)
        };

public:
        Board(std::string fenSring);
        void makeMove(Move currMove);
        void normalMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove);
        int getSquareType(int index);
        void rookKingMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove);
        void pawnMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove);
        void unmakeMove();
        void printChessBoard();
        Move parseMove(std::string moveStr);
        static std::string moveToString(Move move);
        bool isKingAttacked(bool isWhiteKing);
        void printMoveHistory();
        bool isThreefold();
        void addThreefold();
        void removeThreefold();
};