#include "chess_board.h"

board::board(const std::string &initialFen) : fen(initialFen)
{
    reset();
}

void board::reset()
{
    setFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void board::setFromFEN(const std::string &newFen)
{
    fen = newFen;

    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            boardState[row][col] = '\0';
        }
    }

    std::istringstream fenStream(newFen);
    std::string boardPart;
    fenStream >> boardPart;

    int row = 7;
    int col = 0;

    for (char c : boardPart)
    {
        if (c == '/')
        {
            row--;
            col = 0;
        }
        else if (isdigit(c))
        {
            col += c - '0';
        }
        else
        {
            if (col < 8)
            {
                boardState[row][col] = c;
                col++;
            }
        }
    }
}

static bool whiteKingMoved = false, blackKingMoved = false;
static bool whiteRookMoved[2] = {false, false}; // [queen-side, king-side]
static bool blackRookMoved[2] = {false, false};
void board::applyMoves(const std::vector<std::string> &moves)
{
    static std::pair<int, int> enPassantTarget = {-1, -1}; 

    for (const auto &move : moves)
    {
        if (move.size() < 4)
        {
            std::cerr << "Invalid move format: " << move << "\n";
            continue;
        }

        int fromCol = move[0] - 'a';
        int fromRow = move[1] - '1';
        int toCol = move[2] - 'a';
        int toRow = move[3] - '1';

        if (fromCol < 0 || fromCol > 7 || fromRow < 0 || fromRow > 7 ||
            toCol < 0 || toCol > 7 || toRow < 0 || toRow > 7)
        {
            std::cerr << "Move out of bounds: " << move << "\n";
            continue;
        }

        char piece = boardState[fromRow][fromCol];
        boardState[fromRow][fromCol] = '\0';

        // Handle promotion
        if (move.size() == 5)
        {
            char promotionPiece = move[4]; 
            if (piece == 'P' && toRow == 7) 
            {
                piece = std::toupper(promotionPiece);
            }
            else if (piece == 'p' && toRow == 0) 
            {
                piece = std::tolower(promotionPiece); 
            }
        }

        // Handle en passant capture
        if (piece == 'P' || piece == 'p')
        {
            if (toCol != fromCol && boardState[toRow][toCol] == '\0')
            {
                if (piece == 'P' && toRow == fromRow + 1 && enPassantTarget == std::make_pair(toRow - 1, toCol))
                {
                    boardState[toRow - 1][toCol] = '\0'; 
                }
                if (piece == 'p' && toRow == fromRow - 1 && enPassantTarget == std::make_pair(toRow + 1, toCol))
                {
                    boardState[toRow + 1][toCol] = '\0'; 
                }
            }
            // Set en passant target if pawn moves two squares forward
            if (abs(fromRow - toRow) == 2)
            {
                enPassantTarget = {fromRow + (toRow - fromRow) / 2, toCol};
            }
            else
            {
                enPassantTarget = {-1, -1};
            }
        }
        else
        {
            enPassantTarget = {-1, -1}; // Reset en passant target if not a pawn move
        }

        // Handle castling
        if (piece == 'K' && abs(fromCol - toCol) == 2)
        {
            if (toCol == 6)
            {                                        
                boardState[0][5] = boardState[0][7]; 
                boardState[0][7] = '\0';
            }
            else if (toCol == 2)
            {                                        
                boardState[0][3] = boardState[0][0]; 
                boardState[0][0] = '\0';
            }
            whiteKingMoved = true;
        }
        else if (piece == 'k' && abs(fromCol - toCol) == 2)
        {
            if (toCol == 6)
            {                                       
                boardState[7][5] = boardState[7][7]; 
                boardState[7][7] = '\0';
            }
            else if (toCol == 2)
            {                                        
                boardState[7][3] = boardState[7][0]; 
                boardState[7][0] = '\0';
            }
            blackKingMoved = true;
        }

        // Track rook moves for castling rights
        if (piece == 'R')
        {
            if (fromRow == 7 && fromCol == 0)
                whiteRookMoved[0] = true;
            if (fromRow == 7 && fromCol == 7)
                whiteRookMoved[1] = true;
        }
        else if (piece == 'r')
        {
            if (fromRow == 0 && fromCol == 0)
                blackRookMoved[0] = true;
            if (fromRow == 0 && fromCol == 7)
                blackRookMoved[1] = true;
        }

        // Update the destination square
        boardState[toRow][toCol] = piece;
    }
}

std::string board::getBestMove(char player)
{
    std::vector<std::string> legalMoves = generateLegalMoves(player);

    if (legalMoves.empty())
    {
        return "0000";
    }

    return legalMoves[std::rand() % legalMoves.size()];
}

std::vector<std::string> board::generateLegalMoves(char player) const
{
    std::vector<std::string> moves;

    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            char piece = boardState[row][col];
            if (piece == '\0')
                continue;

            if ((player == 'w' && std::isupper(piece)) ||
                (player == 'b' && std::islower(piece)))
            {
                std::vector<std::string> pieceMoves = generateMovesForPiece(row, col, piece);
                for (const auto &move : pieceMoves)
                {
                    board testBoard = *this;
                    testBoard.applyMoves({move});
                    if (!testBoard.isKingInCheck(player))
                    {
                        moves.push_back(move);
                    }
                }
            }
        }
    }

    return moves;
}

bool board::isKingInCheck(char player) const
{
    int kingRow = -1, kingCol = -1;
    char king = (player == 'w') ? 'K' : 'k';

    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            if (boardState[row][col] == king)
            {
                kingRow = row;
                kingCol = col;
                break;
            }
        }
    }

    if (kingRow == -1 || kingCol == -1)
    {
        std::cerr << "Error: King not found for player " << player << "\n";
        return true;
    }

    char opponent = (player == 'w') ? 'b' : 'w';

    if (player == 'w')
    {
        if (kingRow < 7 && kingCol > 0 && boardState[kingRow + 1][kingCol - 1] == 'p')
            return true;
        if (kingRow < 7 && kingCol < 7 && boardState[kingRow + 1][kingCol + 1] == 'p')
            return true;
    }
    else
    {
        if (kingRow > 0 && kingCol > 0 && boardState[kingRow - 1][kingCol - 1] == 'P')
            return true;
        if (kingRow > 0 && kingCol < 7 && boardState[kingRow - 1][kingCol + 1] == 'P')
            return true;
    }

    const std::vector<std::pair<int, int>> knightMoves = {{2, 1}, {2, -1}, {1, 2}, {1, -2}, {-2, 1}, {-2, -1}, {-1, 2}, {-1, -2}};
    for (const auto &move : knightMoves)
    {
        int r = kingRow + move.first;
        int c = kingCol + move.second;
        if (r >= 0 && r < 8 && c >= 0 && c < 8)
        {
            char piece = boardState[r][c];
            if ((player == 'w' && piece == 'n') || (player == 'b' && piece == 'N'))
                return true;
        }
    }

    const std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (const auto &dir : directions)
    {
        int r = kingRow + dir.first;
        int c = kingCol + dir.second;
        while (r >= 0 && r < 8 && c >= 0 && c < 8)
        {
            char piece = boardState[r][c];
            if (piece != '\0')
            {
                if ((player == 'w' && std::islower(piece)) || (player == 'b' && std::isupper(piece)))
                {
                    if ((dir.first == 0 || dir.second == 0) && (piece == 'r' || piece == 'R' || piece == 'q' || piece == 'Q'))
                        return true;
                    if ((dir.first != 0 && dir.second != 0) && (piece == 'b' || piece == 'B' || piece == 'q' || piece == 'Q'))
                        return true;
                }
                break;
            }
            r += dir.first;
            c += dir.second;
        }
    }

    const std::vector<std::pair<int, int>> kingMoves = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (const auto &move : kingMoves)
    {
        int r = kingRow + move.first;
        int c = kingCol + move.second;
        if (r >= 0 && r < 8 && c >= 0 && c < 8)
        {
            char piece = boardState[r][c];
            if ((player == 'w' && piece == 'k') || (player == 'b' && piece == 'K'))
                return true;
        }
    }

    return false;
}

bool board::isSquareAttacked(int row, int col, char opponent) const
{
    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            char piece = boardState[r][c];
            if ((opponent == 'w' && std::isupper(piece)) || (opponent == 'b' && std::islower(piece)))
            {
                std::vector<std::string> opponentMoves = generateMovesForPiece(r, c, piece);
                for (const auto &move : opponentMoves)
                {
                    int targetRow = move[2] - '1';
                    int targetCol = move[3] - 'a';
                    if (targetRow == row && targetCol == col)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

std::vector<std::string> board::generateMovesForPiece(int row, int col, char piece) const
{
    std::vector<std::string> moves;

    const std::vector<std::pair<int, int>> rookDirections = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    const std::vector<std::pair<int, int>> bishopDirections = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    const std::vector<std::pair<int, int>> queenDirections = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    const std::vector<std::pair<int, int>> knightMoves = {{2, 1}, {2, -1}, {1, 2}, {1, -2}, {-2, 1}, {-2, -1}, {-1, 2}, {-1, -2}};
    const std::vector<std::pair<int, int>> kingMoves = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

    static std::pair<int, int> enPassantTarget = {-1, -1};

    if (piece == 'P')
    {
        if (row < 7 && boardState[row + 1][col] == '\0')
        {
            // Promotion
            if (row + 1 == 7)
            {
                moves.push_back(convertToAlgebraic(row, col, row + 1, col) + "q");
                moves.push_back(convertToAlgebraic(row, col, row + 1, col) + "r");
                moves.push_back(convertToAlgebraic(row, col, row + 1, col) + "b");
                moves.push_back(convertToAlgebraic(row, col, row + 1, col) + "n");
            }
            else
            {
                moves.push_back(convertToAlgebraic(row, col, row + 1, col));
            }

            if (row == 1 && boardState[row + 2][col] == '\0')
            {
                moves.push_back(convertToAlgebraic(row, col, row + 2, col));
            }
        }
        if (col > 0 && row < 7 && std::islower(boardState[row + 1][col - 1]))
        {
            // Promotion capture
            if (row + 1 == 7)
            {
                moves.push_back(convertToAlgebraic(row, col, row + 1, col - 1) + "q");
                moves.push_back(convertToAlgebraic(row, col, row + 1, col - 1) + "r");
                moves.push_back(convertToAlgebraic(row, col, row + 1, col - 1) + "b");
                moves.push_back(convertToAlgebraic(row, col, row + 1, col - 1) + "n");
            }
            else
            {
                moves.push_back(convertToAlgebraic(row, col, row + 1, col - 1));
            }
        }
        if (col < 7 && row < 7 && std::islower(boardState[row + 1][col + 1]))
        {
            // Promotion capture
            if (row + 1 == 7)
            {
                moves.push_back(convertToAlgebraic(row, col, row + 1, col + 1) + "q");
                moves.push_back(convertToAlgebraic(row, col, row + 1, col + 1) + "r");
                moves.push_back(convertToAlgebraic(row, col, row + 1, col + 1) + "b");
                moves.push_back(convertToAlgebraic(row, col, row + 1, col + 1) + "n");
            }
            else
            {
                moves.push_back(convertToAlgebraic(row, col, row + 1, col + 1));
            }
        }
        // En passant
        if (row == 4 && enPassantTarget.first == 5 && std::abs(enPassantTarget.second - col) == 1)
        {
            moves.push_back(convertToAlgebraic(row, col, row + 1, enPassantTarget.second));
        }
    }
    else if (piece == 'p')
    {
        // Black pawn
        if (row > 0 && boardState[row - 1][col] == '\0')
        {
            // Promotion
            if (row - 1 == 0)
            {
                moves.push_back(convertToAlgebraic(row, col, row - 1, col) + "q");
                moves.push_back(convertToAlgebraic(row, col, row - 1, col) + "r");
                moves.push_back(convertToAlgebraic(row, col, row - 1, col) + "b");
                moves.push_back(convertToAlgebraic(row, col, row - 1, col) + "n");
            }
            else
            {
                moves.push_back(convertToAlgebraic(row, col, row - 1, col));
            }

            if (row == 6 && boardState[row - 2][col] == '\0')
            {
                moves.push_back(convertToAlgebraic(row, col, row - 2, col));
            }
        }
        if (col > 0 && row > 0 && std::isupper(boardState[row - 1][col - 1]))
        {
            // Promotion capture
            if (row - 1 == 0)
            {
                moves.push_back(convertToAlgebraic(row, col, row - 1, col - 1) + "q");
                moves.push_back(convertToAlgebraic(row, col, row - 1, col - 1) + "r");
                moves.push_back(convertToAlgebraic(row, col, row - 1, col - 1) + "b");
                moves.push_back(convertToAlgebraic(row, col, row - 1, col - 1) + "n");
            }
            else
            {
                moves.push_back(convertToAlgebraic(row, col, row - 1, col - 1));
            }
        }
        if (col < 7 && row > 0 && std::isupper(boardState[row - 1][col + 1]))
        {
            // Promotion capture
            if (row - 1 == 0)
            {
                moves.push_back(convertToAlgebraic(row, col, row - 1, col + 1) + "q");
                moves.push_back(convertToAlgebraic(row, col, row - 1, col + 1) + "r");
                moves.push_back(convertToAlgebraic(row, col, row - 1, col + 1) + "b");
                moves.push_back(convertToAlgebraic(row, col, row - 1, col + 1) + "n");
            }
            else
            {
                moves.push_back(convertToAlgebraic(row, col, row - 1, col + 1));
            }
        }
        // En passant
        if (row == 3 && enPassantTarget.first == 2 && std::abs(enPassantTarget.second - col) == 1)
        {
            moves.push_back(convertToAlgebraic(row, col, row - 1, enPassantTarget.second));
        }
    }

    else if (piece == 'K' || piece == 'k')
    {
        // King
        for (const auto &move : kingMoves)
        {
            int r = row + move.first, c = col + move.second;
            if (r >= 0 && r < 8 && c >= 0 && c < 8)
            {
                if (boardState[r][c] == '\0' || (piece == 'K' && std::islower(boardState[r][c])) || (piece == 'k' && std::isupper(boardState[r][c])))
                {
                    moves.push_back(convertToAlgebraic(row, col, r, c));
                }
            }
        }
        // Castling
        if (piece == 'K')
        {
            // King-side castling
            if (!whiteKingMoved && !whiteRookMoved[1] &&
                boardState[0][5] == '\0' && boardState[0][6] == '\0' &&
                !isKingInCheck('w') && !isSquareAttacked(0, 5, 'b') && !isSquareAttacked(0, 6, 'b'))
            {
                moves.push_back(convertToAlgebraic(row, col, row, col + 2));
            }
            // Queen-side castling
            if (!whiteKingMoved && !whiteRookMoved[0] &&
                boardState[0][1] == '\0' && boardState[0][2] == '\0' && boardState[0][3] == '\0' &&
                !isKingInCheck('w') && !isSquareAttacked(0, 2, 'b') && !isSquareAttacked(0, 3, 'b'))
            {
                moves.push_back(convertToAlgebraic(row, col, row, col - 2));
            }
        }
        else if (piece == 'k')
        {
            // King-side castling
            if (!blackKingMoved && !blackRookMoved[1] &&
                boardState[7][5] == '\0' && boardState[7][6] == '\0' &&
                !isKingInCheck('b') && !isSquareAttacked(7, 5, 'w') && !isSquareAttacked(7, 6, 'w'))
            {
                moves.push_back(convertToAlgebraic(row, col, row, col + 2));
            }
            // Queen-side castling
            if (!blackKingMoved && !blackRookMoved[0] &&
                boardState[7][1] == '\0' && boardState[7][2] == '\0' && boardState[7][3] == '\0' &&
                !isKingInCheck('b') && !isSquareAttacked(7, 2, 'w') && !isSquareAttacked(7, 3, 'w'))
            {
                moves.push_back(convertToAlgebraic(row, col, row, col - 2));
            }
        }
    }
    else if (piece == 'R' || piece == 'r')
    {
        for (const auto &dir : rookDirections)
        {
            int r = row + dir.first, c = col + dir.second;
            while (r >= 0 && r < 8 && c >= 0 && c < 8)
            {
                if (boardState[r][c] == '\0')
                {
                    moves.push_back(convertToAlgebraic(row, col, r, c));
                }
                else if ((piece == 'R' && std::islower(boardState[r][c])) || (piece == 'r' && std::isupper(boardState[r][c])))
                {
                    moves.push_back(convertToAlgebraic(row, col, r, c));
                    break;
                }
                else
                {
                    break;
                }
                r += dir.first;
                c += dir.second;
            }
        }
    }
    else if (piece == 'B' || piece == 'b')
    {
        for (const auto &dir : bishopDirections)
        {
            int r = row + dir.first, c = col + dir.second;
            while (r >= 0 && r < 8 && c >= 0 && c < 8)
            {
                if (boardState[r][c] == '\0')
                {
                    moves.push_back(convertToAlgebraic(row, col, r, c));
                }
                else if ((piece == 'B' && std::islower(boardState[r][c])) || (piece == 'b' && std::isupper(boardState[r][c])))
                {
                    moves.push_back(convertToAlgebraic(row, col, r, c));
                    break;
                }
                else
                {
                    break;
                }
                r += dir.first;
                c += dir.second;
            }
        }
    }
    else if (piece == 'Q' || piece == 'q')
    {
        for (const auto &dir : queenDirections)
        {
            int r = row + dir.first, c = col + dir.second;
            while (r >= 0 && r < 8 && c >= 0 && c < 8)
            {
                if (boardState[r][c] == '\0')
                {
                    moves.push_back(convertToAlgebraic(row, col, r, c));
                }
                else if ((piece == 'Q' && std::islower(boardState[r][c])) || (piece == 'q' && std::isupper(boardState[r][c])))
                {
                    moves.push_back(convertToAlgebraic(row, col, r, c));
                    break;
                }
                else
                {
                    break;
                }
                r += dir.first;
                c += dir.second;
            }
        }
    }
    else if (piece == 'N' || piece == 'n')
    {
        for (const auto &move : knightMoves)
        {
            int r = row + move.first, c = col + move.second;
            if (r >= 0 && r < 8 && c >= 0 && c < 8)
            {
                if (boardState[r][c] == '\0' || (piece == 'N' && std::islower(boardState[r][c])) || (piece == 'n' && std::isupper(boardState[r][c])))
                {
                    moves.push_back(convertToAlgebraic(row, col, r, c));
                }
            }
        }
    }

    return moves;
}

std::string board::convertToAlgebraic(int fromRow, int fromCol, int toRow, int toCol) const
{
    std::string move;
    move += ('a' + fromCol);
    move += ('1' + fromRow);
    move += ('a' + toCol);
    move += ('1' + toRow);
    return move;
}

void board::printBoard() const
{
    std::cout << "  +-----------------+" << std::endl;
    for (int row = 7; row >= 0; --row)
    {
        std::cout << row + 1 << " |";
        for (int col = 0; col < 8; ++col)
        {
            char piece = boardState[row][col];
            if (piece == '\0')
            {
                std::cout << " .";
            }
            else
            {
                std::cout << " " << piece;
            }
        }
        std::cout << " |" << std::endl;
    }
    std::cout << "  +-----------------+" << std::endl;
    std::cout << "    a b c d e f g h" << std::endl;
}
