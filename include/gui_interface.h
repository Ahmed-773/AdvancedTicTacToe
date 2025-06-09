// gui_interface.h
#ifndef GUI_INTERFACE_H
#define GUI_INTERFACE_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QRadioButton>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include<string>

// --- ADDED ALL NECESSARY INCLUDES FOR MEMBER VARIABLES ---
#include "game_logic.h"
#include "ai_engine.h"
#include "user_auth.h"
#include "game_history.h"
#include "database_manager.h"

class GUIInterface : public QMainWindow {
    Q_OBJECT
    
public:
    explicit GUIInterface(const std::string& dbPath,QWidget *parent = nullptr);
    ~GUIInterface();
    
private slots:
    // Authentication
    void onLoginButtonClicked();
    void onRegisterButtonClicked();
    void onLogoutButtonClicked();
    
    // Game control
    void onCellClicked();
    void onNewGameButtonClicked();
    void onGameModeChanged();
    void onDifficultyChanged(int difficulty);
    
    // Game history
    void onViewHistoryClicked();
    void onGameHistoryItemClicked(int row, int column);
    void onBackToGameClicked();
    
private:
    // Core components
    GameLogic gameLogic;
    AIEngine aiEngine;
    UserAuth userAuth;
    GameHistory gameHistory;



    DatabaseManager dbManager;
    
    // GUI widgets
    QStackedWidget *mainStack;
    
    // Authentication widgets
    QWidget *loginWidget;
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
    QPushButton *loginButton;
    QPushButton *registerButton;
    
    // Game widgets
    QWidget *gameWidget;
    QGridLayout *boardLayout;
    QPushButton *boardButtons[3][3];
    QLabel *statusLabel;
    QPushButton *newGameButton;
    QPushButton *viewHistoryButton;
    QPushButton *logoutButton;
    QRadioButton *pvpModeRadio;
    QRadioButton *aiModeRadio;
    QComboBox *difficultyCombo;
    
    // History widgets
    QWidget *historyWidget;
    QTableWidget *gameHistoryTable;
    QPushButton *backToGameButton;
    
    // Private helper methods
    void applyStylesheet();
    void setupUI();
    void setupAuthentication();
    void setupGameBoard();
    void setupHistoryView();
    
    void updateBoard();
    void handleGameOver(GameResult result);
    void switchToLoginView();
    void switchToGameView();
    void switchToHistoryView();
    
    bool isAIMode() const;
    void makeAIMove();
    
    void loadUserGames();
    void displayGame(const GameState& game);
};

#endif // GUI_INTERFACE_H
