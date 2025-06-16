/*
================================================================================
File: src/user_auth.cpp
Purpose: Implements the UserAuth class using secure OpenSSL for password hashing.
         This version replaces the non-secure placeholder hash.
================================================================================
*/
#include "user_auth.h"
#include <chrono>
#include <random>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h> // Include the required OpenSSL header for SHA-256

// Constructor: Initializes the currentUser pointer to nullptr, meaning no one is logged in.
UserAuth::UserAuth() : currentUser(nullptr) {}

// --- PUBLIC METHODS ---

bool UserAuth::registerUser(const std::string& username, const std::string& password) {
    // Check if the username already exists to prevent duplicates.
    for (const auto& pair : users) {
        if (pair.second.username == username) {
            return false; // Username is already taken.
        }
    }

    UserProfile newUser;
    newUser.userId = generateUserId();
    newUser.username = username;
    // Here, we call our secure hashing function. The plaintext password is never stored.
    newUser.passwordHash = hashPassword(password);

    users[newUser.userId] = newUser;
    // Automatically log in the user upon successful registration.
    currentUser = &users.at(newUser.userId);
    return true;
}

bool UserAuth::loginUser(const std::string& username, const std::string& password) {
    // Find the user by their username.
    for (auto& pair : users) {
        if (pair.second.username == username) {
            // If the user is found, verify the provided password against the stored hash.
            if (verifyPassword(password, pair.second.passwordHash)) {
                currentUser = &pair.second; // Set the current user.
                return true; // Login successful.
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

void UserAuth::setUsers(const std::unordered_map<std::string, UserProfile>& usersMap) {
    users = usersMap;
}

bool UserAuth::updateUserStats(GameResult result) {
    if (!currentUser) {
        return false; // Can't update stats if no one is logged in.
    }

    currentUser->gamesPlayed++;

    // This logic correctly updates stats based on the game result for the logged-in user.
    if (result == GameResult::X_WINS || result == GameResult::O_WINS) {
        currentUser->gamesWon++;
    } else if (result == GameResult::DRAW) {
        currentUser->gamesTied++;
    } else {
        // Any other state (like IN_PROGRESS) would not typically be logged, but if
        // it were, it could be considered a loss or an incomplete game.
        currentUser->gamesLost++;
    }
    return true;
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

/*
================================================================================
This is the secure password hashing function using OpenSSL's SHA-256.
================================================================================
*/
std::string UserAuth::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

/*
================================================================================
This function verifies a password attempt against a stored hash.
It does this by hashing the new attempt and comparing the results.
You can never "un-hash" the stored password.
================================================================================
*/
bool UserAuth::verifyPassword(const std::string& password, const std::string& hashedPassword) {
    // Hash the incoming password and check if it matches the stored hash.
    return hashPassword(password) == hashedPassword;
}