#include "chess_board.h"

board::board(const std::string& initialFen) : fen(initialFen) {
    reset();
}

void board::reset() {
    setFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void board::setFromFEN(const std::string& newFen) {
    fen = newFen;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            boardState[row][col] = '\0';
        }
    }

    std::istringstream fenStream(newFen);
    std::string boardPart;
    fenStream >> boardPart;

    int row = 7;
    int col = 0;

    for (char c : boardPart) {
        if (c == '/') {
            row--;
            col = 0;
        } else if (isdigit(c)) {
            col += c - '0';
        } else {
            if (col < 8) {
                boardState[row][col] = c;
                col++;
            }
        }
    }
}
void board::applyMoves(const std::vector<std::string>& moves) {
    for (const auto& move : moves) {
        if (move.size() < 4) {
            std::cerr << "Invalid move format: " << move << "\n";
            continue;
        }

        int fromCol = move[0] - 'a'; 
        int fromRow = move[1] - '1';
        int toCol = move[2] - 'a';
        int toRow = move[3] - '1';

        // Check bounds
        if (fromCol < 0 || fromCol > 7 || fromRow < 0 || fromRow > 7 ||
            toCol < 0 || toCol > 7 || toRow < 0 || toRow > 7) {
            std::cerr << "Move out of bounds: " << move << "\n";
            continue;
        }

        char piece = boardState[fromRow][fromCol];
        boardState[fromRow][fromCol] = '\0';
        boardState[toRow][toCol] = piece;
    }
}


std::string board::getBestMove() {
    std::vector<std::string> legalMoves = {"e2e4", "d2d4", "g1f3"};
    return legalMoves[std::rand() % legalMoves.size()];
}

void board::printBoard() const {
    std::cout << "  +-----------------+" << std::endl;
    for (int row = 7; row >= 0; --row) {
        std::cout << row + 1 << " |";
        for (int col = 0; col < 8; ++col) {
            char piece = boardState[row][col];
            if (piece == '\0') {
                std::cout << " .";
            } else {
                std::cout << " " << piece;
            }
        }
        std::cout << " |" << std::endl;
    }
    std::cout << "  +-----------------+" << std::endl;
    std::cout << "    a b c d e f g h" << std::endl;
}
