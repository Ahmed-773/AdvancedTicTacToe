/*
================================================================================
File: src/ai_engine.cpp
Purpose: Implements a stable and memory-efficient minimax algorithm.
         This fixes the stack overflow crash by passing the game state
         by reference (&) instead of by value.
================================================================================
*/
#include "ai_engine.h"
#include <vector>
#include <algorithm> // For std::max and std::min
#include <random>    // For the "Easy" difficulty
#include <limits>    // For std::numeric_limits

AIEngine::AIEngine() : currentDifficulty(HARD) {}

void AIEngine::setDifficulty(int level) {
    if (level == 0) {
        currentDifficulty = EASY;
    } else {
        // For Tic-Tac-Toe, Medium and Hard will both play perfectly.
        // A more advanced game might have different logic for Medium.
        currentDifficulty = HARD;
    }
}

Move AIEngine::getBestMove(GameLogic& game) {
    if (currentDifficulty == EASY) {
        // Easy difficulty: Just pick a random available spot.
        std::vector<Move> availableMoves = game.getAvailableMoves();
        if (availableMoves.empty()) {
            return {-1, -1}; // No move possible
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
    int bestVal = std::numeric_limits<int>::min();
    Move bestMove = {-1, -1};

    std::vector<Move> availableMoves = game.getAvailableMoves();

    // The AI is player 'O', which is the maximizer.
    for (const auto& move : availableMoves) {
        // Make the move on the actual board passed by reference.
        game.makeMove(move.row, move.col);
        
        // Calculate the score for this move by looking ahead.
        // After the AI moves, it's the minimizer's (player's) turn.
        int moveVal = minimax(game, false); 
        
        // IMPORTANT: Undo the move to restore the board for the next iteration.
        game.undoLastMove();

        // If the value of the current move is better than the best value found so far,
        // then update the best move.
        if (moveVal > bestVal) {
            bestMove = move;
            bestVal = moveVal;
        }
    }
    return bestMove;
}

// The recursive minimax function.
// It takes the game state BY REFERENCE (&) to avoid making thousands of copies.
int AIEngine::minimax(GameLogic& game, bool isMaximizing) {
    GameResult result = game.checkGameResult();

    // Check for a terminal state (win, loss, draw) and return a score.
    // A higher score is better for the AI ('O'), a lower score is better for the Player ('X').
    if (result == GameResult::O_WINS) return 10;
    if (result == GameResult::X_WINS) return -10;
    if (result == GameResult::DRAW) return 0;

    // If it's the maximizer's (AI's) turn, find the move with the highest score.
    if (isMaximizing) {
        int best = std::numeric_limits<int>::min();
        std::vector<Move> availableMoves = game.getAvailableMoves();
        for (const auto& move : availableMoves) {
            game.makeMove(move.row, move.col);
            best = std::max(best, minimax(game, !isMaximizing));
            game.undoLastMove();
        }
        return best;
    } 
    // If it's the minimizer's (Player's) turn, find the move with the lowest score.
    else {
        int best = std::numeric_limits<int>::max();
        std::vector<Move> availableMoves = game.getAvailableMoves();
        for (const auto& move : availableMoves) {
            game.makeMove(move.row, move.col);
            best = std::min(best, minimax(game, !isMaximizing));
            game.undoLastMove();
        }
        return best;
    }
}