#include "board.h"
#include "movegen.h"
#include <sstream>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
        auto start = std::chrono::steady_clock::now();

        int depth = argc > 1 ? std::stoi(argv[1]) : 1;
        std::string fen = argc > 2 ? argv[2] : "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        std::string moveList = argc > 3 ? argv[3] : "";

        Board testBoard(fen);

        if (!moveList.empty())
        {
                std::stringstream ss(moveList);
                std::string mStr;
                while (ss >> mStr)
                {
                        Move m = testBoard.parseMove(mStr);
                        testBoard.makeMove(m);
                }
        }

        uint64_t totalNodes = MoveGen::perft(testBoard, depth, true);

        auto end = std::chrono::steady_clock::now();

        int duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << totalNodes / (duration / (float)1000) << " Moves Per Second" << std::endl;

        std::cout << "\n"
                  << totalNodes << std::endl;
}