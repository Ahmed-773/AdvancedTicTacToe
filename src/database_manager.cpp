
#include "database_manager.h"
#include <sstream>
#include <iostream>
#include <filesystem>

DatabaseManager::DatabaseManager(std::string dbFilePath) : dbFilePath(dbFilePath) {
    // Create directory if it doesn't exist
    std::filesystem::path dir = std::filesystem::path(dbFilePath).parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
}

DatabaseManager::~DatabaseManager() {
    // Clean up resources if needed
}

bool DatabaseManager::saveUsers(const std::unordered_map<std::string, UserProfile>& users) {
    std::string serialized = serializeUsers(users);
    return writeToFile(dbFilePath + ".users", serialized);
}

std::unordered_map<std::string, UserProfile> DatabaseManager::loadUsers() {
    std::string data = readFromFile(dbFilePath + ".users");
    return deserializeUsers(data);
}


bool DatabaseManager::saveUser(const UserProfile& user) {
    // Load all users, update or add this one, then save all
    auto users = loadUsers();
    users[user.userId] = user;
    return saveUsers(users);
}



