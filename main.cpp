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
            std::cout << "id name Archangel" << '\n';
            std::cout << "id author Joshua Dowd" << '\n';
            std::cout << "uciok" << '\n';
        }
        else if (command == "isready")
        {
            std::cout << "readyok" << '\n';
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
            // Default values
            int depthLimit = 100;     // Search "forever" unless a limit is found
            int timeLimit = 99999999; // means no time limit set yet

            // 1. Enhanced Parsing Logic
            for (size_t i = 1; i < tokens.size(); i++)
            {
                if (tokens[i] == "depth" && i + 1 < tokens.size())
                {
                    depthLimit = std::stoi(tokens[i + 1]);
                }
                else if (tokens[i] == "movetime" && i + 1 < tokens.size())
                {
                    timeLimit = std::stoi(tokens[i + 1]);
                }
                else if (tokens[i] == "wtime" && board.isWhiteTurn && i + 1 < tokens.size())
                {
                    // Spend 5% of remaining time
                    timeLimit = std::stoi(tokens[i + 1]) / 20;
                }
                else if (tokens[i] == "btime" && !board.isWhiteTurn && i + 1 < tokens.size())
                {
                    timeLimit = std::stoi(tokens[i + 1]) / 20;
                }
            }

            // 2. Execute Search
            auto startTime = std::chrono::steady_clock::now();
            auto stopTime = startTime + std::chrono::milliseconds(timeLimit);

            Search mySearch;
            std::string moveStr = "a1a1";
            // We search depth by depth (Iterative Deepening)

            for (int d = 1; d <= depthLimit; d++)
            {
                std::cout << "-----------------" << '\n';
                std::cout << "STARTING DEPTH " << d << '\n';
                std::cout << "-----------------" << '\n';
                double eval = mySearch.alphaBetaMax(board, d, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), true, stopTime);

                // If the search returned because it ran out of time, don't print this depth
                if (mySearch.abortSearch)
                    break;

                moveStr = board.moveToString(mySearch.bestMove);
            }
            // 4. Final UCI Output

            std::cout << "bestmove " << moveStr << '\n';
        }
        else if (command == "quit")
        {
            break;
        }
    }
    return 0;
}