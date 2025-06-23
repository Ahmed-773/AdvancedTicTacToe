/*
================================================================================
File: src/ai_engine.cpp
Purpose: Implements a stable minimax algorithm and adds a debug message
         to confirm this file is being correctly compiled and run.
================================================================================
*/
#include "ai_engine.h"
#include <vector>
#include <algorithm>
#include <random>
#include <limits>
#include <iostream> // For the std::cout debug message

AIEngine::AIEngine() : currentDifficulty(HARD) {}

void AIEngine::setDifficulty(int level) {
    if (level == 0) {
        currentDifficulty = EASY;
    } else {
        currentDifficulty = HARD;
    }
}

Move AIEngine::getBestMove(GameLogic& game) {
    // --- DEBUG TEST ---
    // This message should appear in your Application Output right before the AI moves.
    // If it doesn't appear and the game crashes, the old file is still being used.
    std::cout << "--- AI IS THINKING (NEW CODE) ---" << std::endl;

    if (currentDifficulty == EASY) {
        std::vector<Move> availableMoves = game.getAvailableMoves();
        if (availableMoves.empty()) {
            return {-1, -1};
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, availableMoves.size() - 1);
        return availableMoves[distrib(gen)];
    }
    
    return findBestMove(game);
}

Move AIEngine::findBestMove(GameLogic& game) {
    int bestVal = std::numeric_limits<int>::min();
    Move bestMove = {-1, -1};
    std::vector<Move> availableMoves = game.getAvailableMoves();

    for (const auto& move : availableMoves) {
        game.makeMove(move.row, move.col);
        int moveVal = minimax(game, false);
        game.undoLastMove();

        if (moveVal > bestVal) {
            bestMove = move;
            bestVal = moveVal;
        }
    }
    return bestMove;
}

// The recursive minimax function now correctly uses pass-by-reference.
int AIEngine::minimax(GameLogic& game, bool isMaximizing) {
    GameResult result = game.checkGameResult();

    if (result == GameResult::O_WINS) return 10;
    if (result == GameResult::X_WINS) return -10;
    if (result == GameResult::DRAW) return 0;

    if (isMaximizing) {
        int best = std::numeric_limits<int>::min();
        for (const auto& move : game.getAvailableMoves()) {
            game.makeMove(move.row, move.col);
            best = std::max(best, minimax(game, !isMaximizing));
            game.undoLastMove();
        }
        return best;
    } else {
        int best = std::numeric_limits<int>::max();
        for (const auto& move : game.getAvailableMoves()) {
            game.makeMove(move.row, move.col);
            best = std::min(best, minimax(game, !isMaximizing));
            game.undoLastMove();
        }
        return best;
    }
}
