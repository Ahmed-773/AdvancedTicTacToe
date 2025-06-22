// File: gui_interface.h - Fixed Version
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
#include <QProgressBar>
#include <QSlider>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QParallelAnimationGroup>
#include <QFrame>
#include <QScrollArea>
#include <QTabWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QTextEdit>
#include <QSplitter>
#include <QToolButton>
#include <QButtonGroup>
#include <QStandardPaths>
#include <QDir>
#include <string>

// Forward declarations to avoid circular dependencies
class DatabaseManager;
class UserAuth;
class GameLogic;
class AIEngine;
class GameHistory;

// Define enums and structs that are used in the interface
enum class Player { NONE, X, O };
enum class GameResult { IN_PROGRESS, X_WINS, O_WINS, DRAW };

struct Move {
    int row;
    int col;
    Move(int r = -1, int c = -1) : row(r), col(c) {}
};

struct GameState {
    std::string gameId;
    std::string userId;
    std::string opponentId;
    bool isAIOpponent;
    std::vector<Move> moveHistory;
    GameResult result;
    std::string timestamp;
    
    GameState() : isAIOpponent(false), result(GameResult::IN_PROGRESS) {}
};

struct UserProfile {
    std::string userId;
    std::string username;
    std::string passwordHash;
    int gamesPlayed;
    int gamesWon;
    int gamesLost;
    int gamesTied;
    std::string createdAt;
    
    UserProfile() : gamesPlayed(0), gamesWon(0), gamesLost(0), gamesTied(0) {}
};

class GUIInterface : public QMainWindow {
    Q_OBJECT

public:
    explicit GUIInterface(const std::string& dbPath, QWidget *parent = nullptr);
    ~GUIInterface();

private slots:
    void onLoginButtonClicked();
    void onRegisterButtonClicked();
    void onLogoutButtonClicked();
    void onGuestPlayClicked();
    void onCellClicked();
    void onNewGameButtonClicked();
    void onUndoMoveClicked();
    void onHintClicked();
    void onGameModeChanged();
    void onThemeChanged(int id);
    void onAnimationSpeedChanged(int value);
    void onViewHistoryClicked();
    void onViewStatsClicked();
    void onGameHistoryItemClicked(int row, int column);
    void onBackToGameClicked();
    void onReplayNextClicked();
    void onReplayPrevClicked();
    void onReplayStartClicked();
    void onReplayAutoPlay();
    void onGameTimerUpdate();

private:
    // Forward declared classes - will be included in cpp file
    std::unique_ptr<GameLogic> gameLogic;
    std::unique_ptr<AIEngine> aiEngine;
    std::unique_ptr<UserAuth> userAuth;
    std::unique_ptr<GameHistory> gameHistory;
    std::unique_ptr<DatabaseManager> dbManager;
    
    enum Theme { DARK, LIGHT, NEON };
    Theme currentTheme;
    bool animationsEnabled;
    int animationSpeed;
    bool isGameInProgress;
    bool isReplayMode;
    int gameTimeSeconds;
    QTimer* gameTimer;
    QTimer* aiThinkTimer;
    QStackedWidget *mainStack;
    QWidget *loginWidget, *gameWidget, *historyWidget, *statsWidget, *settingsWidget;
    QFrame *navigationFrame, *loginFrame, *welcomeFrame, *boardFrame, *scoreFrame, *controlsFrame, *historyHeader, *tableFrame, *historyDetailsFrame, *statsHeader, *statsFrame, *settingsContentFrame;
    QLineEdit *usernameInput, *passwordInput;
    QPushButton *boardButtons[3][3];
    QLabel *statusLabel, *timerLabel, *loginStatusLabel, *playerXScoreLabel, *playerOScoreLabel, *streakLabel, *winRateLabel, *totalGamesLabel, *winRateStatsLabel, *averageGameTimeLabel, *longestStreakLabel, *favoriteOpponentLabel, *replayPositionLabel;
    QProgressBar *aiThinkingBar, *loginProgressBar;
    QTabWidget *gameModeTab, *settingsTab;
    QWidget *pvpModeWidget, *aiModeWidget, *appearanceTab, *gameplayTab;
    QComboBox *difficultyCombo;
    QSlider *aiSpeedSlider, *animationSpeedSlider, *replaySpeedSlider;
    QPushButton *loginButton, *registerButton, *guestButton, *newGameButton, *undoButton, *hintButton, *pauseButton, *backToGameButton, *exportHistoryButton;
    QTableWidget *gameHistoryTable;
    QSplitter *historySplitter;
    QTextEdit *gameDetailsText;
    QButtonGroup *themeGroup;
    QCheckBox *animationsEnabledCheck, *showHintsCheck;
    QSpinBox *autoSaveSpinBox;
    QFrame *replayControlsFrame;
    QPushButton *replayStartButton, *replayPrevButton, *replayNextButton, *replayAutoButton;
    QPushButton *gameNavButton, *historyNavButton, *statsNavButton, *settingsNavButton;
    QTimer *replayAutoTimer;
    std::vector<Move> replayHistory;
    int replayMoveIndex;
    bool replayAutoMode;
    QScrollArea *statsScrollArea;

    void setupUI();
    void setupNavigation();
    void setupAuthentication();
    void setupGameBoard();
    void setupScoreDisplay(QVBoxLayout *layout);
    void setupGameModeControls(QVBoxLayout *layout);
    void setupGameControls(QVBoxLayout *layout);
    void setupHistoryView();
    void setupStatsView();
    void setupSettingsView();
    void setupReplayControls();
    void setupReplayControls(bool visible); // Overloaded version
    void applyTheme(Theme theme);
    void updateButtonStyles();
    void animateButton(QWidget* widget);
    void addDropShadow(QWidget* widget);
    void addGlowEffect(QWidget* widget, const QColor& color);
    void updateBoard(bool isReplay = false);
    void updateScoreDisplay();
    void updateGameStats();
    void updateTimer();
    void handleGameOver(GameResult result);
    void makeAIMove();
    void showHint();
    void highlightWinningCells(const std::vector<Move>& cells);
    void resetBoardHighlights();
    void switchToLoginView();
    void switchToGameView();
    void switchToHistoryView();
    void switchToStatsView();
    void switchToSettingsView();
    void updateNavigationButtons();
    void loadUserGames();
    void displayGameForReplay(const GameState& game);
    void updateReplayControls();
    void exportGameHistory();
    void loadSettings();
    void saveSettings();
    void applySettings();
    void animateCellPlacement(int row, int col, Player player);
    void animateGameOver(GameResult result); // Missing declaration
    void fadeInWidget(QWidget* widget);
    QString formatTime(int seconds);
    QString formatGameResult(GameResult result);
    QString getPlayerName(Player player);
    QColor getPlayerColor(Player player);
    void showNotification(const QString& message, const QString& type = "info");
    void setLoading(QPushButton* button, bool loading);
};

#endif // GUI_INTERFACE_H