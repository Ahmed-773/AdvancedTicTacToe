#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "user_auth.h"
#include "game_history.h"
#include <string>
#include <vector>
#include <fstream>
#include <QStandardPaths>

class DatabaseManager {
public:
    DatabaseManager(std::string dbFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() + "/tictactoe_data.db");
    ~DatabaseManager();

    // User data management
    bool saveUsers(const std::unordered_map<std::string, UserProfile>& users);
    std::unordered_map<std::string, UserProfile> loadUsers();

    // Individual record management
    bool saveUser(const UserProfile& user);
    

private:
    std::string dbFilePath;

};

#endif // DATABASE_MANAGER_H