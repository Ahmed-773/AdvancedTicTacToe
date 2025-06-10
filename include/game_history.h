// game_history.h
#ifndef GAME_HISTORY_H
#define GAME_HISTORY_H

#include "game_logic.h"
#include <vector>
#include <string>
#include <chrono>
#include <ctime>

class DatabaseManager;

class GameHistory {
public:
    GameHistory();
    
    // Save game to history
    std::string saveGame(const std::string& player1Id, const std::string& player2Id, 
                 bool isAIOpponent, const std::vector<Move>& moves, 
                 GameResult result,DatabaseManager& dbManager);
    
    // Retrieve game records
    std::vector<GameState> getUserGames(const std::string& userId);
    GameState getGameById(const std::string& gameId);

    void loadFromDatabase(DatabaseManager& dbManager);
    // Replay functionality
    GameLogic replayGame(const std::string& gameId, int moveIndex = -1);
    
private:
    std::vector<GameState> gameHistory;
    
    std::string generateGameId();
    std::string getCurrentTimestamp();
};

#endif // GAME_HISTORY_H