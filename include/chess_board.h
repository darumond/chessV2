#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <string>
#include <vector>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <sstream>
#include <iomanip>

class board
{
public:
    board(const std::string &initialFen);
    void reset();
    void setFromFEN(const std::string &newFen);
    void applyMoves(const std::vector<std::string> &moves);
    std::string getBestMove();
    void printBoard() const;

private:
    std::string fen;
    char boardState[8][8];
};

#endif
