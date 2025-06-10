// game_history.cpp
#include "game_history.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include"database_manager.h"
GameHistory::GameHistory() {}

std::string GameHistory::saveGame(const std::string& player1Id, const std::string& player2Id,
                         bool isAIOpponent, const std::vector<Move>& moves, 
                         GameResult result,DatabaseManager& dbManager) {
    GameState newGame;
    newGame.gameId = generateGameId();
    newGame.player1Id = player1Id;
    newGame.player2Id = player2Id;
    newGame.isAIOpponent = isAIOpponent;
    newGame.moveHistory = moves;
    newGame.result = result;
    newGame.timestamp = getCurrentTimestamp();
    
    gameHistory.push_back(newGame);
    dbManager.saveGame(newGame);
    return newGame.gameId;
}
void GameHistory::loadFromDatabase(DatabaseManager& dbManager) {
    gameHistory = dbManager.loadGameHistory();
}

std::vector<GameState> GameHistory::getUserGames(const std::string& userId) {
    std::vector<GameState> userGames;
    
    for (const auto& game : gameHistory) {
        if (game.player1Id == userId || game.player2Id == userId) {
            userGames.push_back(game);
        }
    }
    
    // Sort by timestamp (newest first)
    std::sort(userGames.begin(), userGames.end(), 
              [](const GameState& a, const GameState& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return userGames;
}

GameState GameHistory::getGameById(const std::string& gameId) {
    for (const auto& game : gameHistory) {
        if (game.gameId == gameId) {
            return game;
        }
    }
    
    // Return empty game state if not found
    return GameState();
}

GameLogic GameHistory::replayGame(const std::string& gameId, int moveIndex) {
    GameLogic replayedGame;
    GameState gameState = getGameById(gameId);
    
    if (gameState.gameId.empty()) {
        return replayedGame;  // Game not found
    }
    
    // Reset game board
    replayedGame.resetBoard();
    
    // If moveIndex is -1, replay all moves
    int movesToReplay = (moveIndex == -1) ? 
                       gameState.moveHistory.size() : 
                       std::min(moveIndex, static_cast<int>(gameState.moveHistory.size()));
    
    // Replay moves up to the specified index
    for (int i = 0; i < movesToReplay; i++) {
        const Move& move = gameState.moveHistory[i];
        replayedGame.makeMove(move.row, move.col);
    }
    
    return replayedGame;
}

std::string GameHistory::generateGameId() {
    // Simple UUID generation for game ID
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << "game-";
    for (int i = 0; i < 8; i++) {
        ss << std::hex << dis(gen);
    }
    
    return ss.str();
}

std::string GameHistory::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
