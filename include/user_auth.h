/*
================================================================================
File: include/user_auth.h (Final Version)
Purpose: Defines the UserProfile data structure and declares the interface
         for the UserAuth class, which handles all user management.
================================================================================
*/
#ifndef USER_AUTH_H
#define USER_AUTH_H

#include <string>
#include <vector>
#include <unordered_map>
#include "game_logic.h" // Needed for the GameResult enum

// Holds all information for a single user.
struct UserProfile {
    std::string userId;
    std::string username;
    std::string passwordHash; // Stores the secure hash, not the password
    int gamesPlayed;
    int gamesWon;
    int gamesLost;
    int gamesTied;
    
    // --- FIELDS FOR DETAILED STATS ---
    long long totalGameTimeSeconds = 0;
    int currentWinStreak = 0;
    int longestWinStreak = 0;
    int aiGamesPlayed = 0;
    int pvpGamesPlayed = 0;

    // Default constructor to initialize stats to zero.
    UserProfile() : gamesPlayed(0), gamesWon(0), gamesLost(0), gamesTied(0) {}
};

// Declares the "what" for user authentication. The "how" is in user_auth.cpp.
class UserAuth {
public:
    UserAuth();
    
    // --- Public API for the GUI ---
    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
    void logoutUser();
    bool isLoggedIn() const;
    const UserProfile* getCurrentUser() const;
    bool updateUserStats(GameResult result, int gameTime, bool vsAI);

    
    // --- Data Hydration ---
    // Used by the persistence layer to load all user data from the database.
    void setUsers(const std::unordered_map<std::string, UserProfile>& usersMap);
    const std::unordered_map<std::string, UserProfile>& getUsers() const;

private:
    // --- Private Members ---
    // An unordered_map provides fast user lookup by userId.
    std::unordered_map<std::string, UserProfile> users;
    // A pointer to the profile of the currently logged-in user.
    UserProfile* currentUser;
    
    // --- Private Helper Functions ---
    // Internal implementation details hidden from the rest of the program.
    std::string generateUserId();
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hashedPassword);
};

#endif // USER_AUTH_H