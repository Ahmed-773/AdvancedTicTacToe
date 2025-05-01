// game_logic.cpp
#include "game_logic.h"

GameLogic::GameLogic() {
    resetBoard();
}

void GameLogic::resetBoard() {
    // Initialize empty board
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = Player::NONE;
        }
    }
    currentPlayer = Player::X;  // X always starts
    moveHistory.clear();
}

bool GameLogic::makeMove(int row, int col) {
    if (!isValidMove(row, col)) {
        return false;
    }

    board[row][col] = currentPlayer;
    recordMove(row, col);

    // Switch players
    currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
    return true;
}

bool GameLogic::isValidMove(int row, int col) const {
    // Check if within bounds
    if (row < 0 || row >= 3 || col < 0 || col >= 3) {
        return false;
    }

    // Check if cell is empty
    return board[row][col] == Player::NONE;
}

GameResult GameLogic::checkGameResult() const {
    // Check if X wins
    if (checkWin(Player::X)) {
        return GameResult::X_WINS;
    }

    // Check if O wins
    if (checkWin(Player::O)) {
        return GameResult::O_WINS;
    }

    // Check for draw
    if (isBoardFull()) {
        return GameResult::DRAW;
    }

    // Game still in progress
    return GameResult::IN_PROGRESS;
}

bool GameLogic::checkWin(Player player) const {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
            return true;
        }
    }

    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player) {
            return true;
        }
    }

    // Check diagonals
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
        return true;
    }

    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
        return true;
    }

    return false;
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
    return board[row][col];
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
        currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
    }
}
