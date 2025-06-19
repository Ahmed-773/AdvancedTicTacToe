// In game_history.cpp, please replace the existing file content with this new version.

#include "game_history.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "database_manager.h" // Keep this include

GameHistory::GameHistory() {}

// MODIFIED: This function now ONLY adds the game to the in-memory list.
// It no longer calls the database manager.
std::string GameHistory::saveGame(const std::string& player1Id, const std::string& player2Id,
                         bool isAIOpponent, const std::vector<Move>& moves,
                         GameResult result) {
    GameState newGame;
    newGame.gameId = generateGameId();
    newGame.player1Id = player1Id;
    newGame.player2Id = player2Id;
    newGame.isAIOpponent = isAIOpponent;
    newGame.moveHistory = moves;
    newGame.result = result;
    newGame.timestamp = getCurrentTimestamp();

    gameHistory.push_back(newGame);
    return newGame.gameId;
}

// This function now correctly loads the history from the file into the class member.
void GameHistory::loadFromDatabase(DatabaseManager& dbManager) {
    gameHistory = dbManager.loadGameHistory();
}

// NEW FUNCTION: This gives other classes a way to get the full list for saving.
const std::vector<GameState>& GameHistory::getAllGames() const {
    return gameHistory;
}

std::vector<GameState> GameHistory::getUserGames(const std::string& userId) {
    std::vector<GameState> userGames;

    for (const auto& game : gameHistory) {
        if (game.player1Id == userId || game.player2Id == userId) {
            userGames.push_back(game);
        }
    }

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
    return GameState();
}

// This replayGame function remains useful for other potential features, so we keep it.
GameLogic GameHistory::replayGame(const std::string& gameId, int moveIndex) {
    GameLogic replayedGame;
    GameState gameState = getGameById(gameId);

    if (gameState.gameId.empty()) {
        return replayedGame;
    }

    replayedGame.resetBoard();

    int movesToReplay = (moveIndex == -1) ?
                       gameState.moveHistory.size() :
                       std::min(moveIndex, static_cast<int>(gameState.moveHistory.size()));

    for (int i = 0; i < movesToReplay; i++) {
        const Move& move = gameState.moveHistory[i];
        replayedGame.makeMove(move.row, move.col);
    }

    return replayedGame;
}

std::string GameHistory::generateGameId() {
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
    // Use std::put_time with a pointer to a tm struct from std::localtime
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}