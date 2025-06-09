
#include "database_manager.h"
#include <sstream>
#include <iostream>
#include <filesystem>
#include <fstream>
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
// part 2
bool DatabaseManager::SaveGameHistory(const std::vector<GameState>& games) {
    std::string serialized = SerializeGames(games);
    return WriteToFile(db_file_path_ + ".games", serialized);
}

std::vector<GameState> DatabaseManager::LoadGameHistory() {
    std::string data = ReadFromFile(db_file_path_ + ".games");
    return DeserializeGames(data);
}

bool DatabaseManager::SaveGame(const GameState& game) {
    auto games = LoadGameHistory();
    games.push_back(game);
    return SaveGameHistory(games);
}
//part 3
bool DatabaseManager::WriteToFile(const std::string& filename, const std::string& data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(data.c_str(), data.size());
    return !file.fail();
}

std::string DatabaseManager::ReadFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string DatabaseManager::SerializeUsers(const std::unordered_map<std::string, UserProfile>& users) {
    std::stringstream ss;

    for (const auto& [id, user] : users) {
        ss << user.user_id << "|"
            << user.username << "|"
            << user.password_hash << "|"
            << user.games_played << "|"
            << user.games_won << "|"
            << user.games_lost << "|"
            << user.games_tied << "\n";
    }

    return ss.str();
}
//part 4

std::unordered_map<std::string, UserProfile> DatabaseManager::DeserializeUsers(const std::string& data) {
    std::unordered_map<std::string, UserProfile> users;
    std::stringstream ss(data);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.empty()) continue;

        std::stringstream line_stream(line);
        std::string field;
        UserProfile user;

        if (!std::getline(line_stream, field, '|')) continue;
        user.user_id = field;

        if (!std::getline(line_stream, field, '|')) continue;
        user.username = field;

        if (!std::getline(line_stream, field, '|')) continue;
        user.password_hash = field;

        if (!std::getline(line_stream, field, '|')) continue;
        user.games_played = std::stoi(field);

        if (!std::getline(line_stream, field, '|')) continue;
        user.games_won = std::stoi(field);

        if (!std::getline(line_stream, field, '|')) continue;
        user.games_lost = std::stoi(field);

        if (!std::getline(line_stream, field, '|')) continue;
        user.games_tied = std::stoi(field);

        users[user.user_id] = user;
    }

    return users;
}

std::string DatabaseManager::SerializeGames(const std::vector<GameState>& games) {
    std::stringstream ss;

    for (const auto& game : games) {
        ss << game.game_id << "|"
            << game.player1_id << "|"
            << game.player2_id << "|"
            << (game.is_ai_opponent ? "1" : "0") << "|"
            << static_cast<int>(game.result) << "|"
            << game.timestamp << "|";

        for (size_t i = 0; i < game.move_history.size(); ++i) {
            if (i > 0) ss << ";";
            ss << game.move_history[i].row << "," << game.move_history[i].col;
        }

        ss << "\n";
    }

    return ss.str();
}

std::vector<GameState> DatabaseManager::DeserializeGames(const std::string& data) {
    std::vector<GameState> games;
    std::stringstream ss(data);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.empty()) continue;

        std::stringstream line_stream(line);
        std::string field;
        GameState game;

        if (!std::getline(line_stream, field, '|')) continue;
        game.game_id = field;

        if (!std::getline(line_stream, field, '|')) continue;
        game.player1_id = field;

        if (!std::getline(line_stream, field, '|')) continue;
        game.player2_id = field;

        if (!std::getline(line_stream, field, '|')) continue;
        game.is_ai_opponent = (field == "1");

        if (!std::getline(line_stream, field, '|')) continue;
        game.result = static_cast<GameResult>(std::stoi(field));

        if (!std::getline(line_stream, field, '|')) continue;
        game.timestamp = field;

        if (!std::getline(line_stream, field)) continue;
        std::stringstream moves_stream(field);
        std::string move_str;

        while (std::getline(moves_stream, move_str, ';')) {
            std::stringstream move_stream(move_str);
            std::string row_str, col_str;

            if (!std::getline(move_stream, row_str, ',')) continue;
            if (!std::getline(move_stream, col_str)) continue;

            game.move_history.emplace_back(std::stoi(row_str), std::stoi(col_str));
        }

        games.push_back(game);
    }

    return games;
}



