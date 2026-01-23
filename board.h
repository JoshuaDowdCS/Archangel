#pragma once

#include <string>
#include <vector>
#include "move.h"
#include "movelist.h"
#include "types.h"

class Board
{

public:
    Bitboard bitboardsInitial[2][8];
    Bitboard allCombinedInitial;
    int pieceArrayInitial[64];
    bool whiteShortCastleInitial;
    bool whiteLongCastleInitial;
    bool blackShortCastleInitial;
    bool blackLongCastleInitial;
    int passantSquareInitial;
    int halfmoveClockInitial;
    bool isWhiteTurnInitial;
    int fullmoveCounterInitial;

    Bitboard bitboards[2][8]; // 0 is combined, 1-6 holds piece type, 7 is pinned. White is 0, 1 is black
    Bitboard allCombined;
    int pieceArray[64];
    bool whiteShortCastle;
    bool whiteLongCastle;
    bool blackShortCastle;
    bool blackLongCastle;
    int passantSquare;
    int halfmoveClock;
    bool isWhiteTurn;
    int fullmoveCounter;

    std::vector<Move> moveHistory;

public:
    Board(std::string fenSring);
    void makeMove(Move currMove);
    void normalMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove);
    void rookKingMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove);
    void pawnMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove);
    void unmakeMove();
    void printChessBoard();
    Move parseMove(std::string moveStr);
    static std::string moveToString(Move move);
    bool isKingAttacked(bool isWhiteKing);
    void printMoveHistory();
};