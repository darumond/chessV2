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

std::string board::getBestMove(char player) {
    std::vector<std::string> legalMoves = generateLegalMoves(player);

    if (legalMoves.empty()) {
        return "0000";
    }

    return legalMoves[std::rand() % legalMoves.size()];
}

std::vector<std::string> board::generateLegalMoves(char player) const {
    std::vector<std::string> moves;

    char currentPlayer = player;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            char piece = boardState[row][col];
            if (piece == '\0') continue;

            if ((currentPlayer == 'w' && std::isupper(piece)) ||
                (currentPlayer == 'b' && std::islower(piece))) {
                std::vector<std::string> pieceMoves = generateMovesForPiece(row, col, piece);
                moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
            }
        }
    }

    return moves;
}

std::vector<std::string> board::generateMovesForPiece(int row, int col, char piece) const {
    std::vector<std::string> moves;
    if (piece == 'P') {
        if (row < 7 && boardState[row + 1][col] == '\0') {
            moves.push_back(convertToAlgebraic(row, col, row + 1, col));
        }
        if (row == 1 && boardState[row + 2][col] == '\0' && boardState[row + 1][col] == '\0') {
            moves.push_back(convertToAlgebraic(row, col, row + 2, col));
        }
    } else if (piece == 'p') {
        if (row > 0 && boardState[row - 1][col] == '\0') {
            moves.push_back(convertToAlgebraic(row, col, row - 1, col));
        }
        if (row == 6 && boardState[row - 2][col] == '\0' && boardState[row - 1][col] == '\0') {
            moves.push_back(convertToAlgebraic(row, col, row - 2, col));
        }
    }

    return moves;
}

std::string board::convertToAlgebraic(int fromRow, int fromCol, int toRow, int toCol) const {
    std::string move;
    move += ('a' + fromCol);
    move += ('1' + fromRow);
    move += ('a' + toCol);
    move += ('1' + toRow);
    return move;
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
