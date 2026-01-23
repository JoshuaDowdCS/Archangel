#include "board.h"
#include "movegen.h"
#include "search.h"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Helper to split string by spaces
std::vector<std::string> split(const std::string &str)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ss(str);
    while (ss >> token)
        tokens.push_back(token);
    return tokens;
}

int main(int argc, char *argv[])
{
    std::setbuf(stdout, NULL); // Critical for GUI communication

    Board board(START_FEN);
    std::string line;

    while (std::getline(std::cin, line))
    {
        std::vector<std::string> tokens = split(line);
        if (tokens.empty())
            continue;

        std::string command = tokens[0];

        if (command == "uci")
        {
            std::cout << "id name Archangel" << std::endl;
            std::cout << "id author Joshua Dowd" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (command == "isready")
        {
            std::cout << "readyok" << std::endl;
        }
        else if (command == "ucinewgame")
        {
            board = Board(START_FEN);
        }
        else if (command == "position")
        {
            // position [startpos | fen] moves ....
            size_t moveIndex = 0;

            if (tokens[1] == "startpos")
            {
                board = Board(START_FEN);
                moveIndex = 2; // "moves" would be at tokens[2]
            }
            else if (tokens[1] == "fen")
            {
                // FENs are 6 tokens long. Reconstruct it.
                std::string fen = tokens[2] + " " + tokens[3] + " " + tokens[4] + " " +
                                  tokens[5] + " " + tokens[6] + " " + tokens[7];
                board = Board(fen);
                moveIndex = 8; // "moves" would be at tokens[8]
            }

            // Apply moves if they exist in the command
            for (size_t i = moveIndex; i < tokens.size(); ++i)
            {
                if (tokens[i] == "moves")
                    continue;
                Move m = board.parseMove(tokens[i]);
                board.makeMove(m);
            }
        }
        else if (command == "go")
        {
            // 1. Determine search depth or time (parsing logic)
            int depth = 4;
            for (size_t i = 1; i < tokens.size(); i++)
            {
                if (tokens[i] == "depth")
                    depth = std::stoi(tokens[i + 1]);
            }

            // 2. Call your actual search function (not perft)
            // Your search should return a 'Move' object
            Move bestMoveFound = Search::findBestMove(board, depth);

            // 3. Convert that Move object to a string (e.g., "e2e4")
            std::string moveStr = board.moveToString(bestMoveFound);

            // 4. Tell the GUI what to play
            std::cout << "bestmove " << moveStr << std::endl;
        }
        else if (command == "quit")
        {
            break;
        }
    }
    return 0;
}