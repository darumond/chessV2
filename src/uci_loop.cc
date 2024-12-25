#include "uci_loop.h"
#include "chess_board.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void uciLoop()
{
    std::string line;
    board chessBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "uci")
        {
            std::cout << "id name RandomUCIBot\n";
            std::cout << "id author YourName\n";
            std::cout << "uciok\n";
        }
        else if (command == "isready")
        {
            std::cout << "readyok\n";
        }
        else if (command == "ucinewgame")
        {
        }
        else if (command == "position")
        {
            std::string positionType;
            iss >> positionType;

            std::vector<std::string> moves;
            if (positionType == "startpos")
            {
                chessBoard.reset();
                std::string token;
                iss >> token;
                while (iss >> token)
                {
                    moves.push_back(token);
                }
            }
            else if (positionType == "fen")
            {
                std::string fen;
                std::getline(iss, fen);
                chessBoard.setFromFEN(fen);
            }

            chessBoard.applyMoves(moves);
            chessBoard.printBoard();
        }
        else if (command == "go")
        {
            auto bestMove = chessBoard.getBestMove();
            std::cout << "bestmove " << bestMove << "\n";
        }
        else if (command == "quit" || command == "stop")
        {
            break;
        }
        else
        {
            std::cerr << "Unknown command: " << command << "\n";
        }
    }
}
