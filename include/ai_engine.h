// ai_engine.h
#ifndef AI_ENGINE_H
#define AI_ENGINE_H

#include "game_logic.h"
#include <limits>

class AIEngine {
public:
    AIEngine(int difficulty = 3);
    Move getBestMove(const GameLogic& gameState);
    void setDifficulty(int level);

private:
    int difficulty;  // AI difficulty level (search depth)

    int minimax(GameLogic gameState, int depth, bool isMaximizing,
                int alpha = std::numeric_limits<int>::min(),
                int beta = std::numeric_limits<int>::max());

    int evaluateBoard(const GameLogic& gameState, Player aiPlayer);
    Player getAIPlayer(const GameLogic& gameState);
    
private:
};

#endif // AI_ENGINE_H
