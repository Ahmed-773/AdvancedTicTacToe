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
    GameResult result = gameState.checkGameResult();
    Player aiPlayer = getAIPlayer(gameState);
    Player opponent = (aiPlayer == Player::X) ? Player::O : Player::X;
    
    // Terminal state evaluation
    if (result == GameResult::X_WINS) {
        return (aiPlayer == Player::X) ? 10 + depth : -10 - depth;
    }
    if (result == GameResult::O_WINS) {
        return (aiPlayer == Player::O) ? 10 + depth : -10 - depth;
    }
    if (result == GameResult::DRAW || depth == 0) {
        return 0;
    }
    
    if (isMaximizing) {
        int maxEval = std::numeric_limits<int>::min();
        for (const auto& move : gameState.getAvailableMoves()) {
            GameLogic boardCopy = gameState;
            boardCopy.makeMove(move.row, move.col);
            int eval = minimax(boardCopy, depth - 1, false, alpha, beta);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                break;  // Alpha-beta pruning
            }
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (const auto& move : gameState.getAvailableMoves()) {
            GameLogic boardCopy = gameState;
            boardCopy.makeMove(move.row, move.col);
            int eval = minimax(boardCopy, depth - 1, true, alpha, beta);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break;  // Alpha-beta pruning
            }
        }
        return minEval;
    }
}

int AIEngine::evaluateBoard(const GameLogic& gameState, Player aiPlayer) {
    GameResult result = gameState.checkGameResult();
    
    // Terminal state evaluation
    if (result == GameResult::X_WINS) {
        return (aiPlayer == Player::X) ? 10 : -10;
    if (result == GameResult::O_WINS) {
        return (aiPlayer == Player::O) ? 10 : -10;
    }
    
    return 0;  // Draw or in progress
}
