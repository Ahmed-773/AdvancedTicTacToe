/*
================================================================================
File: include/ai_engine.h
Purpose: Declares the AIEngine class. This stable version uses the minimax
         algorithm and passes the game state by reference to prevent crashes.
================================================================================
*/
#ifndef AI_ENGINE_H
#define AI_ENGINE_H

#include "game_logic.h"
#include <vector>

class AIEngine {
public:
    AIEngine();

    // The main function called by the GUI to get the AI's next move.
    Move getBestMove(GameLogic& game);

    // Allows the GUI to change the AI's difficulty.
    void setDifficulty(int level);

private:
    // Defines the different difficulty levels for the AI.
    enum Difficulty {
        EASY,
        HARD
    };
    Difficulty currentDifficulty;

    // --- Minimax Algorithm Helpers ---
    Move findBestMove(GameLogic& game);

    // The minimax function now takes the game state by reference.
    int minimax(GameLogic& game, bool isMaximizing);
};

#endif // AI_ENGINE_H
