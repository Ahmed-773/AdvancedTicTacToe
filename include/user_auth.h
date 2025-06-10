// user_auth.h
#ifndef USER_AUTH_H
#define USER_AUTH_H

#include <string>
#include <vector>
#include <unordered_map>
#include "game_logic.h"

struct UserProfile {
    std::string userId;
    std::string username;
    std::string passwordHash;
    int gamesPlayed;
    int gamesWon;
    int gamesLost;
    int gamesTied;
    
    UserProfile() : gamesPlayed(0), gamesWon(0), gamesLost(0), gamesTied(0) {}
};

class UserAuth {
public:
    UserAuth();
    
    // User management functions
    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
    void logoutUser();
    bool isLoggedIn() const;

    void setUsers(const std::unordered_map<std::string, UserProfile>& usersMap);

    // User profile functions
    const UserProfile* getCurrentUser() const;
    bool updateUserStats(GameResult result);
    
    std::string generateSessionToken();
    bool validateSession(const std::string& token);
    
private:
    std::unordered_map<std::string, UserProfile> users;
    UserProfile* currentUser;
    std::string currentSessionToken;
    
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hashedPassword);
    std::string generateUserId();
};

#endif // USER_AUTH_H
