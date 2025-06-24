/*
================================================================================
File: src/user_auth.cpp (Corrected)
Purpose: Implements the UserAuth class. Includes the missing getUsers() function.
================================================================================
*/
#include "user_auth.h"
#include <chrono>
#include <random>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h> // Uses OpenSSL for hashing

// Constructor
UserAuth::UserAuth() : currentUser(nullptr) {}

// --- PUBLIC METHODS ---

bool UserAuth::registerUser(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) {
        return false;
    }
    for (const auto& pair : users) {
        if (pair.second.username == username) {
            return false; // Username is already taken.
        }
    }

    UserProfile newUser;
    newUser.userId = generateUserId();
    newUser.username = username;
    newUser.passwordHash = hashPassword(password);

    users[newUser.userId] = newUser;
    currentUser = &users.at(newUser.userId);
    return true;
}

bool UserAuth::loginUser(const std::string& username, const std::string& password) {
    for (auto& pair : users) {
        if (pair.second.username == username) {
            if (verifyPassword(password, pair.second.passwordHash)) {
                currentUser = &pair.second;
                return true;
            }
            return false; // Incorrect password.
        }
    }
    return false; // User not found.
}

void UserAuth::logoutUser() {
    currentUser = nullptr;
}

bool UserAuth::isLoggedIn() const {
    return currentUser != nullptr;
}

const UserProfile* UserAuth::getCurrentUser() const {
    return currentUser;
}

void UserAuth::updateUserStats(GameResult result, int gameTime, bool vsAI) {
    if (!currentUser) return;

    currentUser->gamesPlayed++;
    currentUser->totalGameTimeSeconds += gameTime;

    if (vsAI) {
        currentUser->aiGamesPlayed++;
    } else {
        currentUser->pvpGamesPlayed++;
    }

    if (result == GameResult::X_WINS) {
        currentUser->gamesWon++;
        currentUser->currentWinStreak++;
        if (currentUser->currentWinStreak > currentUser->longestWinStreak) {
            currentUser->longestWinStreak = currentUser->currentWinStreak;
        }
    } else if (result == GameResult::O_WINS) {
        currentUser->gamesLost++;
        currentUser->currentWinStreak = 0; // Reset streak on a loss
    } else { // Draw
        currentUser->gamesTied++;
        currentUser->currentWinStreak = 0; // Reset streak on a draw
    }
}

void UserAuth::setUsers(const std::unordered_map<std::string, UserProfile>& usersMap) {
    users = usersMap;
}

// SOLUTION: This is the missing function implementation required by the linker.
const std::unordered_map<std::string, UserProfile>& UserAuth::getUsers() const {
    return users;
}

// --- PRIVATE METHODS ---

std::string UserAuth::generateUserId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    for (int i = 0; i < 16; i++) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

std::string UserAuth::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

bool UserAuth::verifyPassword(const std::string& password, const std::string& hashedPassword) {
    return hashPassword(password) == hashedPassword;
}
