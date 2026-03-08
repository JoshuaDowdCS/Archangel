#include "movegen/board.h"
#include "engine/search.h"
#include "types/evaluatedmove.h"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

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
    std::setbuf(stdout, NULL);

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

            size_t moveIndex = 0;

            if (tokens[1] == "startpos")
            {
                board = Board(START_FEN);
                moveIndex = 2;
            }
            else if (tokens[1] == "fen")
            {

                std::string fen = tokens[2] + " " + tokens[3] + " " + tokens[4] + " " +
                                  tokens[5] + " " + tokens[6] + " " + tokens[7];
                board = Board(fen);
                moveIndex = 8;
            }

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

            int depthLimit = 100;
            int timeLimit = 5000;

            for (size_t i = 1; i < tokens.size(); i++)
            {
                if (tokens[i] == "depth" && i + 1 < tokens.size())
                {
                    depthLimit = std::stoi(tokens[i + 1]);
                    timeLimit = std::numeric_limits<int>::max();
                }
                else if (tokens[i] == "movetime" && i + 1 < tokens.size())
                {
                    timeLimit = std::stoi(tokens[i + 1]);
                    depthLimit = std::numeric_limits<int>::max();
                }
                else if (tokens[i] == "wtime" && board.isWhiteTurn && i + 1 < tokens.size())
                {
                    timeLimit = std::stoi(tokens[i + 1]) / 20;
                    depthLimit = std::numeric_limits<int>::max();
                }
                else if (tokens[i] == "btime" && !board.isWhiteTurn && i + 1 < tokens.size())
                {
                    timeLimit = std::stoi(tokens[i + 1]) / 20;
                    depthLimit = std::numeric_limits<int>::max();
                }
            }

            auto startTime = std::chrono::steady_clock::now();
            auto stopTime = startTime + std::chrono::milliseconds(timeLimit);

            std::string moveStr = "a1a1";
            Search mySearch;

            for (int d = 1; d <= depthLimit; d++)
            {
                std::vector<Move> moveLine;

                EvaluatedMove bestMove = mySearch.alphaBetaSearch(board, stopTime, d, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), board.isWhiteTurn, moveLine, true);

                if (mySearch.abortSearch)
                    break;

                moveStr = board.moveToString(bestMove.move) == "a1a1" ? moveStr : board.moveToString(bestMove.move);

                std::cout << "On depth " << d << " the current favorite Move is: " << moveStr << " with a score of: " << bestMove.evaluation << " ";

                for (Move currMove : moveLine)
                    std::cout << board.moveToString(currMove) << " ";

                std::cout << std::endl;

                if (std::abs(bestMove.evaluation) > 1000000)
                {
                    break;
                }
            }
            std::cout << "bestmove " << moveStr << "\n";
        }
        else if (command == "quit")
        {
            break;
        }
    }
    return 0;
}