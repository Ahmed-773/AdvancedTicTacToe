// database_manager.h
#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "user_auth.h"
#include "game_history.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <QStandardPaths>

class DatabaseManager {
public:
    DatabaseManager(std::string dbFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() + "/tictactoe_data.db");
    ~DatabaseManager();

    // User data management
    bool saveUsers(const std::unordered_map<std::string, UserProfile>& users);
    std::unordered_map<std::string, UserProfile> loadUsers();

    // Game history management
    bool saveGameHistory(const std::vector<GameState>& games);
    std::vector<GameState> loadGameHistory();

    // Individual record management
    bool saveUser(const UserProfile& user);
    bool saveGame(const GameState& game);

private:
    std::string dbFilePath;

    // File I/O helpers
    bool writeToFile(const std::string& filename, const std::string& data);
    std::string readFromFile(const std::string& filename);

    // Serialization helpers
    std::string serializeUsers(const std::unordered_map<std::string, UserProfile>& users);
    std::unordered_map<std::string, UserProfile> deserializeUsers(const std::string& data);

    std::string serializeGames(const std::vector<GameState>& games);
    std::vector<GameState> deserializeGames(const std::string& data);
};

#endif // DATABASE_MANAGER_H
