// game_logic.cpp
#include "game_logic.h"

GameLogic::GameLogic() {
    resetBoard();
}

void GameLogic::resetBoard() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = Player::NONE;
        }
    }
    currentPlayer = Player::X;
    moveHistory.clear();
}

bool GameLogic::makeMove(int row, int col) {
    if (!isValidMove(row, col)) {
        return false;
    }
    board[row][col] = currentPlayer;
    recordMove(row, col);
    currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
    return true;
}

bool GameLogic::isValidMove(int row, int col) const {
    if (row < 0 || row >= 3 || col < 0 || col >= 3) {
        return false;
    }
    return board[row][col] == Player::NONE;
}

GameResult GameLogic::checkGameResult() const {
    if (checkWin(Player::X)) return GameResult::X_WINS;
    if (checkWin(Player::O)) return GameResult::O_WINS;
    if (isBoardFull()) return GameResult::DRAW;
    return GameResult::IN_PROGRESS;
}

bool GameLogic::checkWin(Player player) const {
    // Check rows and columns
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
            return true;
        }
    }
    // Check diagonals
    if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
        (board[0][2] == player && board[1][1] == player && board[2][0] == player)) {
        return true;
    }
    return false;
}

// NEWLY IMPLEMENTED: This function finds which 3 cells made the win.
std::vector<Move> GameLogic::findWinningCombination() const {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != Player::NONE && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            return {{i, 0}, {i, 1}, {i, 2}};
        }
    }
    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board[0][i] != Player::NONE && board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
            return {{0, i}, {1, i}, {2, i}};
        }
    }
    // Check diagonals
    if (board[0][0] != Player::NONE && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return {{0, 0}, {1, 1}, {2, 2}};
    }
    if (board[0][2] != Player::NONE && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return {{0, 2}, {1, 1}, {2, 0}};
    }
    return {}; // Return empty vector if no win
}

bool GameLogic::isBoardFull() const {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == Player::NONE) {
                return false;
            }
        }
    }
    return true;
}

Player GameLogic::getCurrentPlayer() const {
    return currentPlayer;
}

Player GameLogic::getCell(int row, int col) const {
    if (row >= 0 && row < 3 && col >= 0 && col < 3) {
        return board[row][col];
    }
    return Player::NONE;
}

const std::array<std::array<Player, 3>, 3>& GameLogic::getBoard() const {
    return board;
}

void GameLogic::recordMove(int row, int col) {
    moveHistory.push_back(Move(row, col));
}

const std::vector<Move>& GameLogic::getMoveHistory() const {
    return moveHistory;
}

std::vector<Move> GameLogic::getAvailableMoves() const {
    std::vector<Move> moves;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == Player::NONE) {
                moves.push_back(Move(i, j));
            }
        }
    }
    return moves;
}

void GameLogic::undoLastMove() {
    if (!moveHistory.empty()) {
        Move lastMove = moveHistory.back();
        board[lastMove.row][lastMove.col] = Player::NONE;
        moveHistory.pop_back();
        // Switch player back
        currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
    }
}
