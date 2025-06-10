// ai_engine.cpp
#include "ai_engine.h"

AIEngine::AIEngine(int difficulty) : difficulty(difficulty) {}

void AIEngine::setDifficulty(int level) {
    difficulty = level;
}

Player AIEngine::getAIPlayer(const GameLogic& gameState) {
    return gameState.getCurrentPlayer();
}

Move AIEngine::getBestMove(const GameLogic& gameState) {
    GameLogic tempState = gameState;
    Player aiPlayer = getAIPlayer(gameState);
    int bestScore = std::numeric_limits<int>::min();
    Move bestMove(-1, -1);

    // Get all available moves
    std::vector<Move> availableMoves = tempState.getAvailableMoves();

    for (const auto& move : availableMoves) {
        // Try this move
        GameLogic boardCopy = tempState;
        boardCopy.makeMove(move.row, move.col);

        // Calculate score with minimax
        int score = minimax(boardCopy, difficulty, false);

        // Update best move if found
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

int AIEngine::minimax(GameLogic gameState, int depth, bool isMaximizing, int alpha, int beta) {

}

int AIEngine::evaluateBoard(const GameLogic& gameState, Player aiPlayer) {

}
// #include "ai_engine.h"
