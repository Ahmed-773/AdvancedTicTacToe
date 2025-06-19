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
#include "database_manager.h"
#include <QStandardPaths>
#include <QDir>
#include<string>
// --- ALL NECESSARY INCLUDES FOR MEMBER VARIABLES ---
#include "user_auth.h"
#include "game_logic.h"
#include "ai_engine.h"
#include "game_history.h"

// Forward declarations for Qt classes to speed up compilation
class QStackedWidget;
class QLineEdit;
class QLabel;
class QPushButton;
class QTableWidget;
class QComboBox;
class QRadioButton;

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

    // UI Structure
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
    QLabel* playerXScoreLabel; // NEW: Score display for Player X
    QLabel* playerOScoreLabel; // NEW: Score display for Player O / AI

    QWidget *historyWidget;
    QTableWidget *gameHistoryTable;

    // Replay state variables
    std::vector<Move> replayHistory;
    int replayMoveIndex;
    QPushButton* replayNextButton;
    QPushButton* replayPrevButton;
    QPushButton* replayStartButton;


    // --- Private Helper Methods ---
    void applyStylesheet();
    void setupUI();
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
