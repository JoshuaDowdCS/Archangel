#pragma once

#include <string>
#include <vector>
#include "move.h"
#include "movelist.h"
#include "boardstate.h"
#include "types.h"

class Board
{

public:
    std::string startingFen;

    Bitboard bitboards[2][8]; // 0 is combined, 1-6 holds piece type, 7 is pinned. White is 0, 1 is black
    Bitboard allCombined;
    int castlingRights; // stored as 0bXXXX. 1000 is white short, 0100 is WL, 0010 is BS, and 0001 is BL
    int passantSquare;
    int halfmoveClock;
    bool isWhiteTurn;
    int fullmoveCounter;

    std::vector<Move> moveHistory;
    std::vector<BoardState> previousMoves;

public:
    Board(std::string fenSring);
    void makeMove(Move currMove);
    void normalMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove);
    bool isSquareWhite(int index);
    int getSquareType(int index);
    void rookKingMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove);
    void pawnMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove);
    void unmakeMove();
    void printChessBoard();
    Move parseMove(std::string moveStr);
    static std::string moveToString(Move move);
    bool isKingAttacked(bool isWhiteKing);
    void printMoveHistory();
};