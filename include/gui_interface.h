// // gui_interface.h
// #ifndef GUI_INTERFACE_H
// #define GUI_INTERFACE_H

// #include <QMainWindow>
// #include <QGridLayout>
// #include <QPushButton>
// #include <QLabel>
// #include <QLineEdit>
// #include <QStackedWidget>
// #include <QTableWidget>
// #include <QComboBox>
// #include <QRadioButton>
// #include <QMessageBox>
// #include "database_manager.h"
// #include <QStandardPaths>
// #include <QDir>
// #include<string>

// // Forward declarations for Qt classes to speed up compilation
// class QStackedWidget;
// class QLineEdit;
// class QLabel;
// class QPushButton;
// class QTableWidget;
// class QComboBox;
// class QRadioButton;

// // Include all necessary headers for our own classes
// #include "user_auth.h"
// #include "game_logic.h"
// #include "ai_engine.h"
// #include "database_manager.h"
// #include "game_history.h"

// class GUIInterface : public QMainWindow {
//     Q_OBJECT

// public:
//     explicit GUIInterface(const std::string& dbPath, QWidget *parent = nullptr);
//     ~GUIInterface();

// private slots:
//     // Authentication
//     void onLoginButtonClicked();
//     void onRegisterButtonClicked();
//     void onLogoutButtonClicked();

//     // Game control
//     void onCellClicked();
//     void onNewGameButtonClicked();

//     // Game history and Replay
//     void onViewHistoryClicked();
//     void onGameHistoryItemClicked(int row, int column);
//     void onBackToGameClicked();
//     void onReplayNextClicked();
//     void onReplayPrevClicked();
//     void onReplayStartClicked();

// private:
//     // Core backend components
//     GameLogic gameLogic;
//     AIEngine aiEngine;
//     UserAuth userAuth;
//     GameHistory gameHistory;
//     DatabaseManager dbManager;

//     // --- UI Structure ---
//     QStackedWidget *mainStack;

//     // --- Widgets for Each Screen ---
//     QWidget *loginWidget;
//     QLineEdit *usernameInput;
//     QLineEdit *passwordInput;

//     QWidget *gameWidget;
//     QPushButton *boardButtons[3][3];
//     QLabel *statusLabel;
//     QRadioButton *pvpModeRadio;
//     QRadioButton *aiModeRadio;
//     QComboBox *difficultyCombo;
//     QLabel* playerXScoreLabel;
//     QLabel* playerOScoreLabel;

//     QWidget *historyWidget;
//     QTableWidget *gameHistoryTable;
//     QPushButton *backToGameButton;

//     // --- Replay state variables ---
//     std::vector<Move> replayHistory;
//     int replayMoveIndex;
//     QWidget* replayControlsWidget; // A widget to hold the replay buttons
//     QPushButton* replayNextButton;
//     QPushButton* replayPrevButton;
//     QPushButton* replayStartButton;


//     // --- Private Helper Methods ---
//     void applyStylesheet();
//     void setupUI();
//     void setupAuthentication();
//     void setupGameBoard();
//     void setupHistoryView();
//     void updateBoard(bool isReplay = false);
//     void updateScoreDisplay();
//     void handleGameOver(GameResult result);
//     void switchToLoginView();
//     void switchToGameView();
//     void switchToHistoryView();
//     void setupReplayControls(bool show);
//     void makeAIMove();
//     void loadUserGames();
//     void displayGameForReplay(const GameState& game);
// };

// #endif // GUI_INTERFACE_H
//===========================================================================================================================
//===========================================================================================================================
//===========================================================================================================================
// File: gui_interface.h
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
#include <QTimer>
#include <QProgressBar>
#include "database_manager.h"
#include <QStandardPaths>
#include <QDir>
#include <string>

#include "user_auth.h"
#include "game_logic.h"
#include "ai_engine.h"
#include "game_history.h"

class GUIInterface : public QMainWindow {
    Q_OBJECT

public:
    explicit GUIInterface(const std::string& dbPath, QWidget *parent = nullptr);
    ~GUIInterface();

private slots:
    // Authentication
    void onLoginButtonClicked();
    void onRegisterButtonClicked();
    void onLogoutButtonClicked();

    // Game control
    void onCellClicked();
    void onNewGameButtonClicked();
    void onUndoClicked();

    // Game history and Replay
    void onViewHistoryClicked();
    void onGameHistoryItemClicked(int row, int column);
    void onBackToGameClicked();
    void onReplayNextClicked();
    void onReplayPrevClicked();
    void onReplayStartClicked();

private:
    // Core backend components
    GameLogic gameLogic;
    AIEngine aiEngine;
    UserAuth userAuth;
    GameHistory gameHistory;
    DatabaseManager dbManager;

    // --- UI Structure ---
    QStackedWidget *mainStack;

    // --- Widgets for Each Screen ---
    QWidget *loginWidget;
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;

    QWidget *gameWidget;
    QPushButton *boardButtons[3][3];
    QLabel *statusLabel;
    QRadioButton *pvpModeRadio;
    QRadioButton *aiModeRadio;
    QComboBox *difficultyCombo;
    QLabel *playerXScoreLabel;
    QLabel *playerOScoreLabel;

    QWidget *historyWidget;
    QTableWidget *gameHistoryTable;
    QPushButton *backToGameButton;

    // Replay and Undo state
    std::vector<Move> replayHistory;
    int replayMoveIndex;
    QWidget* replayControlsWidget;
    QPushButton* replayNextButton;
    QPushButton* replayPrevButton;
    QPushButton* replayStartButton;
    QPushButton* undoButton;  // Undo last move

    // Turn timer
    QTimer *turnTimer;
    QProgressBar *timerBar;

    // --- Private Helper Methods ---
    void applyStylesheet();
    void setupUI();
    void setupAuthentication();
    void setupGameBoard();
    void setupHistoryView();
    void updateBoard(bool isReplay = false);
    void updateScoreDisplay();
    void handleGameOver(GameResult result);
    void switchToLoginView();
    void switchToGameView();
    void switchToHistoryView();
    void setupReplayControls(bool show);
    void makeAIMove();
    void loadUserGames();
    void displayGameForReplay(const GameState& game);
};

#endif // GUI_INTERFACE_H
