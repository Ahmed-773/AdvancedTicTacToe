/*
================================================================================
File: src/ai_engine.cpp
Purpose: Implements a stable and memory-efficient minimax algorithm.
         This fixes the stack overflow crash.
================================================================================
*/
#include "ai_engine.h"
#include <vector>
#include <algorithm> // For std::max and std::min
#include <random>    // For the "Easy" difficulty

AIEngine::AIEngine() : currentDifficulty(HARD) {}

void AIEngine::setDifficulty(int level) {
    if (level == 0) {
        currentDifficulty = EASY;
    } else {
        // For this game, Medium and Hard will both play perfectly.
        currentDifficulty = HARD;
    }
}

Move AIEngine::getBestMove(GameLogic& game) {
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
    
    // Use the optimal algorithm for Hard difficulty.
    return findBestMove(game);
}

// This is the main entry point for the minimax calculation.
Move AIEngine::findBestMove(GameLogic& game) {
    int bestVal = -1000;
    Move bestMove = {-1, -1};

    std::vector<Move> availableMoves = game.getAvailableMoves();

    for (const auto& move : availableMoves) {
        // Make the move on the actual board passed by reference.
        game.makeMove(move.row, move.col);
        
        // Calculate the score for this move. The AI is 'O' (the maximizer),
        // so after its move, it will be the player's ('X', the minimizer) turn.
        int moveVal = minimax(game, false); 
        
        // IMPORTANT: Undo the move to restore the board to its original state for the next iteration.
        game.undoLastMove();

        if (moveVal > bestVal) {
            bestMove = move;
            bestVal = moveVal;
        }
    }
    return bestMove;
}

// The recursive minimax function.
// It takes the game state BY REFERENCE (&) to avoid crashing.
int AIEngine::minimax(GameLogic& game, bool isMaximizing) {
    GameResult result = game.checkGameResult();

    // Check for a terminal state (win, loss, draw) and return a score.
    if (result == GameResult::O_WINS) return 10;  // AI 'O' wins
    if (result == GameResult::X_WINS) return -10; // Player 'X' wins
    if (result == GameResult::DRAW) return 0;   // Draw

    // If it's the maximizer's (AI's) turn
    if (isMaximizing) {
        int best = -1000;
        std::vector<Move> availableMoves = game.getAvailableMoves();
        for (const auto& move : availableMoves) {
            game.makeMove(move.row, move.col);
            best = std::max(best, minimax(game, !isMaximizing));
            game.undoLastMove();
        }
        return best;
    } 
    // If it's the minimizer's (Player's) turn
    else {
        int best = 1000;
        std::vector<Move> availableMoves = game.getAvailableMoves();
        for (const auto& move : availableMoves) {
            game.makeMove(move.row, move.col);
            best = std::min(best, minimax(game, !isMaximizing));
            game.undoLastMove();
        }
        return best;
    }
}