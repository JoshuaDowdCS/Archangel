#include "movelist.h"
#include <iostream>
#include <iomanip>

void MoveList::printMoveList()
{
        if (count == 0)
        {
                std::cout << "Move list is empty." << '\n';
                return;
        }

        std::cout << "Move List (" << count << " moves):" << '\n';
        std::cout << std::string(50, '-') << '\n';

        for (int i = 0; i < count; i++)
        {
                Move &move = moveList[i];

                int from = move.getFrom();
                int to = move.getTo();

                // Convert square indices to algebraic notation (0-63 -> a1-h8)
                char fromFile = 'a' + (from % 8);
                char fromRank = '1' + (from / 8);
                char toFile = 'a' + (to % 8);
                char toRank = '1' + (to / 8);

                std::cout << std::setw(3) << i + 1 << ". "
                          << fromFile << fromRank << toFile << toRank;

                // Print special move types
                if (move.isPromotion())
                {
                        std::cout << "=";
                        int promotion = move.getPromotion();
                        if (promotion == Move::KNIGHT)
                                std::cout << "N";
                        else if (promotion == Move::BISHOP)
                                std::cout << "B";
                        else if (promotion == Move::ROOK)
                                std::cout << "R";
                        else if (promotion == Move::QUEEN)
                                std::cout << "Q";
                        std::cout << " (promotion)";
                }
                else if (move.isCastling())
                {
                        std::cout << " (castling)";
                }
                else if (move.isPassant())
                {
                        std::cout << " (en passant)";
                }

                std::cout << '\n';
        }

        std::cout << std::string(50, '-') << '\n';
}
