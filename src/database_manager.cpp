#include "database_manager.h"
#include <sstream>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

DatabaseManager::DatabaseManager(std::string dbFilePath) : db_file_path_(dbFilePath) {
    // Create directory if it doesn't exist
   std::filesystem::path dir = std::filesystem::path(db_file_path_).parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
}

DatabaseManager::~DatabaseManager() {
    // Clean up resources if needed
}

bool DatabaseManager::saveUsers(const std::unordered_map<std::string, UserProfile>& users) {
    std::string serialized = serializeUsers(users);
    return writeToFile(db_file_path_ + ".users", serialized);
}

std::unordered_map<std::string, UserProfile> DatabaseManager::loadUsers() {
    std::string data = readFromFile(db_file_path_ + ".users");
    return deserializeUsers(data);
}

// Load all users, update or add this one, then save all
bool DatabaseManager::saveUser(const UserProfile& user) {
    auto users = loadUsers();
    users[user.userId] = user;
    return saveUsers(users);
}

// part 2
bool DatabaseManager::saveGameHistory(const std::vector<GameState>& games) {
    std::string serialized = serializeGames(games);
    return writeToFile(db_file_path_ + ".games", serialized);
}

std::vector<GameState> DatabaseManager::loadGameHistory() {
    std::string data = readFromFile(db_file_path_ + ".games");
    return deserializeGames(data);
}

//part 3
bool DatabaseManager::writeToFile(const std::string& filename, const std::string& data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(data.c_str(), data.size());
    return !file.fail();
}

std::string DatabaseManager::readFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string DatabaseManager::serializeUsers(const std::unordered_map<std::string, UserProfile>& users) {
    std::stringstream ss;
    for (const auto& pair : users) {
        const UserProfile& user = pair.second;
        ss << user.userId << "|" << user.username << "|" << user.passwordHash
           << "|" << user.gamesPlayed << "|" << user.gamesWon << "|" << user.gamesLost
           << "|" << user.gamesTied << "|" << user.totalGameTimeSeconds 
           << "|" << user.currentWinStreak << "|" << user.longestWinStreak
           << "|" << user.aiGamesPlayed << "|" << user.pvpGamesPlayed << "\n";
    }
    return ss.str();
}

//part 4
std::unordered_map<std::string, UserProfile> DatabaseManager::deserializeUsers(const std::string& data) {
    std::unordered_map<std::string, UserProfile> users;
    std::stringstream ss(data);
    std::string line;
    while (std::getline(ss, line)) {
        std::stringstream lineStream(line);
        std::string segment;
        UserProfile u;
        int field = 0;
        while(std::getline(lineStream, segment, '|')) {
            try {
                switch(field) {
                    case 0: u.userId = segment; break;
                    case 1: u.username = segment; break;
                    case 2: u.passwordHash = segment; break;
                    case 3: u.gamesPlayed = std::stoi(segment); break;
                    case 4: u.gamesWon = std::stoi(segment); break;
                    case 5: u.gamesLost = std::stoi(segment); break;
                    case 6: u.gamesTied = std::stoi(segment); break;
                    case 7: u.totalGameTimeSeconds = std::stoll(segment); break;
                    case 8: u.currentWinStreak = std::stoi(segment); break;
                    case 9: u.longestWinStreak = std::stoi(segment); break;
                    case 10: u.aiGamesPlayed = std::stoi(segment); break;
                    case 11: u.pvpGamesPlayed = std::stoi(segment); break;
                }
            } catch (const std::invalid_argument& e) {
                // Handle error or corrupted data
            }
            field++;
        }
        if (!u.userId.empty()) {
            users[u.userId] = u;
        }
    }
    return users;
}

std::string DatabaseManager::serializeGames(const std::vector<GameState>& games) {
    std::stringstream ss;
    for (const auto& game : games) {
        ss << game.gameId << "|"
           << game.player1Id << "|"
           << game.player2Id << "|"
           << (game.isAIOpponent ? "1" : "0") << "|"
           << static_cast<int>(game.result) << "|"
           << game.timestamp << "|";
        
        for (size_t i = 0; i < game.moveHistory.size(); ++i) {
            if (i > 0) ss << ";";
            ss << game.moveHistory[i].row << "," << game.moveHistory[i].col;
        }
        ss << "\n";
    }
    return ss.str();
}

std::vector<GameState> DatabaseManager::deserializeGames(const std::string& data) {
    std::vector<GameState> games;
    std::stringstream ss(data);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.empty()) {
            continue; 
        }

        std::stringstream lineStream(line);
        std::string field;
        GameState game;


        try {

            if (!std::getline(lineStream, field, '|')) continue;
            game.gameId = field;

            if (!std::getline(lineStream, field, '|')) continue;
            game.player1Id = field;

            if (!std::getline(lineStream, field, '|')) continue;
            game.player2Id = field;

            if (!std::getline(lineStream, field, '|')) continue;
            game.isAIOpponent = (field == "1");

            if (!std::getline(lineStream, field, '|')) continue;
            game.result = static_cast<GameResult>(std::stoi(field));

            if (!std::getline(lineStream, field, '|')) continue;
            game.timestamp = field;

            if (std::getline(lineStream, field)) {
                std::stringstream movesStream(field);
                std::string movePair;

                while (std::getline(movesStream, movePair, ';')) {
                    if (movePair.empty()) continue; 
                    size_t commaPos = movePair.find(',');
                    if (commaPos != std::string::npos) {
                        std::string rowStr = movePair.substr(0, commaPos);
                        std::string colStr = movePair.substr(commaPos + 1);
                        
                        game.moveHistory.emplace_back(std::stoi(rowStr), std::stoi(colStr));
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse game history line: " << line << " | Error: " << e.what() << std::endl;
            continue;
        }

        games.push_back(game);
    }
    return games;
}


