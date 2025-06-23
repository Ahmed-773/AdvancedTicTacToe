/*
================================================================================
File: include/ai_engine.h
Purpose: Declares the AIEngine class. This stable version uses the minimax
         algorithm and passes the game state by reference to prevent crashes.
================================================================================
*/
#ifndef AI_ENGINE_H
#define AI_ENGINE_H

#include "game_logic.h" // AIEngine needs to know about the game's rules and state.
#include <vector>

class AIEngine {
public:
    AIEngine();

    // The main function called by the GUI to get the AI's next move.
    // It takes a non-const reference because our internal minimax needs to
    // make and undo moves on the board to test possibilities.
    Move getBestMove(GameLogic& game);

    // Allows the GUI to change the AI's difficulty.
    void setDifficulty(int level);

private:
    // Defines the different difficulty levels for the AI.
    enum Difficulty {
        EASY,   // Makes random valid moves.
        HARD    // Uses the full minimax algorithm to play perfectly.
    };
    Difficulty currentDifficulty;

    // --- Minimax Algorithm Helpers ---
    // These are the core functions for the AI's decision-making process.
    Move findBestMove(GameLogic& game);

    // The minimax function now takes the game state by reference to be efficient.
    int minimax(GameLogic& game, bool isMaximizing);
};

#endif // AI_ENGINE_H
