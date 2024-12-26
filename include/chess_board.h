#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <string>
#include <vector>
#include <iostream>
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
    std::string getBestMove(char player);
    void printBoard() const;

private:
    std::string fen;
    char boardState[8][8];
    std::vector<std::string> generateLegalMoves(char player) const;
    std::vector<std::string> generateMovesForPiece(int row, int col, char piece) const;
    std::string convertToAlgebraic(int fromRow, int fromCol, int toRow, int toCol) const;
};

#endif
