// user_auth.cpp
#include "user_auth.h"
#include <chrono>
#include <random>
#include <functional>
#include <iomanip>
#include <sstream>

UserAuth::UserAuth() : currentUser(nullptr) {}

bool UserAuth::registerUser(const std::string& username, const std::string& password) {
    // Check if username already exists
    for (const auto& pair : users) {
        if (pair.second.username == username) {
            return false;  // Username already taken
        }
    }
    
    // Create new user profile
    UserProfile newUser;
    newUser.userId = generateUserId();
    newUser.username = username;
    newUser.passwordHash = hashPassword(password);
    
    // Add to users map
    users[newUser.userId] = newUser;
    currentUser = &users[newUser.userId];
    return true;
}

bool UserAuth::loginUser(const std::string& username, const std::string& password) {
    // Find user by username
    for (auto& pair : users) {
        if (pair.second.username == username) {
            // Verify password
            if (verifyPassword(password, pair.second.passwordHash)) {
                currentUser = &(pair.second);
                currentSessionToken = generateSessionToken();
                return true;
            }
            return false;  // Incorrect password
        }
    }
    return false;  // User not found
}

void UserAuth::logoutUser() {
    currentUser = nullptr;
    currentSessionToken.clear();
}

bool UserAuth::isLoggedIn() const {
    return currentUser != nullptr;
}

const UserProfile* UserAuth::getCurrentUser() const {
    return currentUser;
}
void UserAuth::setUsers(const std::unordered_map<std::string, UserProfile>& usersMap) {
    users = usersMap;
}

bool UserAuth::updateUserStats(GameResult result) {
    if (!currentUser) {
        return false;
    }
    
    currentUser->gamesPlayed++;
    
    if (result == GameResult::X_WINS && currentUser->username == "PlayerX") {
        currentUser->gamesWon++;
    } else if (result == GameResult::O_WINS && currentUser->username == "PlayerO") {
        currentUser->gamesWon++;
    } else if (result == GameResult::DRAW) {
        currentUser->gamesTied++;
    } else {
        currentUser->gamesLost++;
    }
    
    return true;
}

std::string UserAuth::generateSessionToken() {
    // Generate a random session token
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << std::hex << timestamp;
    
    for (int i = 0; i < 16; i++) {
        ss << std::hex << dis(gen);
    }
    
    return ss.str();
}

bool UserAuth::validateSession(const std::string& token) {
    return token == currentSessionToken && currentUser != nullptr;
}

std::string UserAuth::hashPassword(const std::string& password) {
    // This is a simplified password hash for demonstration
    // In a real application, use a proper cryptographic library for secure hashing
    std::hash<std::string> hasher;
    size_t hash = hasher(password);
    
    std::stringstream ss;
    ss << std::hex << hash;
    return ss.str();
}

bool UserAuth::verifyPassword(const std::string& password, const std::string& hashedPassword) {
    return hashPassword(password) == hashedPassword;
}

std::string UserAuth::generateUserId() {
    // Simple UUID generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    for (int i = 0; i < 8; i++) {
        ss << std::hex << dis(gen);
    }
    ss << "-";
    for (int i = 0; i < 4; i++) {
        ss << std::hex << dis(gen);
    }
    
    return ss.str();
}
