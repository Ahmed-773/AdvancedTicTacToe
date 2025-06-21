// gui_interface.h - Optimized Version
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
#include "database_manager.h"
#include <QStandardPaths>
#include <QDir>
#include <string>

// Forward declarations
class QStackedWidget;
class QLineEdit;
class QLabel;
class QPushButton;
class QTableWidget;
class QComboBox;
class QRadioButton;
class QTimer;
class QPropertyAnimation;

// Include all necessary headers for our own classes
#include "user_auth.h"
#include "game_logic.h"
#include "ai_engine.h"
#include "database_manager.h"
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
    void onGuestPlayClicked();

    // Game control
    void onCellClicked();
    void onNewGameButtonClicked();
    void onUndoMoveClicked();
    void onHintClicked();

    // Game modes and settings
    void onGameModeChanged();
    void onThemeChanged();
    void onSoundToggled();
    void onAnimationSpeedChanged();

    // Game history and Replay
    void onViewHistoryClicked();
    void onViewStatsClicked();
    void onGameHistoryItemClicked(int row, int column);
    void onBackToGameClicked();
    void onReplayNextClicked();
    void onReplayPrevClicked();
    void onReplayStartClicked();
    void onReplayAutoPlay();

    // Timers and animations
    void onGameTimerUpdate();
    void onCellAnimationFinished();

private:
    // Core backend components
    GameLogic gameLogic;
    AIEngine aiEngine;
    UserAuth userAuth;
    GameHistory gameHistory;
    DatabaseManager dbManager;

    // Themes and settings
    enum Theme { DARK, LIGHT, NEON, CLASSIC };
    Theme currentTheme;
    bool soundEnabled;
    bool animationsEnabled;
    int animationSpeed;

    // Game state
    bool isGameInProgress;
    bool isReplayMode;
    int gameTimeSeconds;
    QTimer* gameTimer;
    QTimer* aiThinkTimer;

    // --- UI Structure ---
    QStackedWidget *mainStack;

    // --- Enhanced Login Screen ---
    QWidget *loginWidget;
    QFrame *loginFrame;
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *guestButton;
    QLabel *loginStatusLabel;
    QProgressBar *loginProgressBar;

    // --- Enhanced Game Screen ---
    QWidget *gameWidget;
    QFrame *gameFrame;
    QFrame *boardFrame;
    QPushButton *boardButtons[3][3];
    QLabel *statusLabel;
    QLabel *timerLabel;
    QProgressBar *aiThinkingBar;
    
    // Game mode controls
    QTabWidget *gameModeTab;
    QWidget *pvpModeWidget;
    QWidget *aiModeWidget;
    QWidget *challengeModeWidget;
    QComboBox *difficultyCombo;
    QSlider *aiSpeedSlider;
    
    // Score and stats
    QFrame *scoreFrame;
    QLabel *playerXScoreLabel;
    QLabel *playerOScoreLabel;
    QLabel *streakLabel;
    QLabel *winRateLabel;
    
    // Game controls
    QFrame *controlsFrame;
    QPushButton *newGameButton;
    QPushButton *undoButton;
    QPushButton *hintButton;
    QPushButton *pauseButton;

    // --- Enhanced History Screen ---
    QWidget *historyWidget;
    QSplitter *historySplitter;
    QTableWidget *gameHistoryTable;
    QFrame *historyDetailsFrame;
    QTextEdit *gameDetailsText;
    QPushButton *backToGameButton;
    QPushButton *exportHistoryButton;

    // --- Settings Screen ---
    QWidget *settingsWidget;
    QTabWidget *settingsTab;
    QWidget *appearanceTab;
    QWidget *gameplayTab;
    QWidget *soundTab;
    QButtonGroup *themeGroup;
    QCheckBox *soundEnabledCheck;
    QCheckBox *animationsEnabledCheck;
    QSlider *animationSpeedSlider;
    QSpinBox *autoSaveSpinBox;

    // --- Statistics Screen ---
    QWidget *statsWidget;
    QScrollArea *statsScrollArea;
    QFrame *statsFrame;
    QLabel *totalGamesLabel;
    QLabel *winRateStatsLabel;
    QLabel *averageGameTimeLabel;
    QLabel *longestStreakLabel;
    QLabel *favoriteOpponentLabel;

    // --- Replay Controls ---
    QFrame *replayControlsFrame;
    QPushButton *replayStartButton;
    QPushButton *replayPrevButton;
    QPushButton *replayNextButton;
    QPushButton *replayAutoButton;
    QSlider *replaySpeedSlider;
    QLabel *replayPositionLabel;
    QTimer *replayAutoTimer;
    std::vector<Move> replayHistory;
    int replayMoveIndex;
    bool replayAutoMode;

    // --- Navigation ---
    QFrame *navigationFrame;
    QPushButton *gameNavButton;
    QPushButton *historyNavButton;
    QPushButton *statsNavButton;
    QPushButton *settingsNavButton;

    // --- Animations ---
    QPropertyAnimation *boardAnimation;
    QParallelAnimationGroup *cellAnimations;
    QGraphicsOpacityEffect *fadeEffect;

    // --- Private Helper Methods ---
    void setupUI();
    void setupAuthentication();
    void setupGameBoard();
    void setupHistoryView();
    void setupStatsView();
    void setupSettingsView();
    void setupNavigation();
    void setupReplayControls();
    void setupAnimations();
    
    // Styling and themes
    void applyTheme(Theme theme);
    void applyDarkTheme();
    void applyLightTheme();
    void applyNeonTheme();
    void applyClassicTheme();
    void updateButtonStyles();
    void animateButton(QPushButton* button);
    void addDropShadow(QWidget* widget);
    void addGlowEffect(QWidget* widget, const QColor& color);
    
    // Game logic helpers
    void updateBoard(bool isReplay = false);
    void updateScoreDisplay();
    void updateGameStats();
    void updateTimer();
    void handleGameOver(GameResult result);
    void makeAIMove();
    void showHint();
    void highlightWinningCells(const std::vector<Move>& cells);
    void resetBoardHighlights();
    
    // Navigation helpers
    void switchToLoginView();
    void switchToGameView();
    void switchToHistoryView();
    void switchToStatsView();
    void switchToSettingsView();
    void updateNavigationButtons();
    
    // History and replay helpers
    void loadUserGames();
    void displayGameForReplay(const GameState& game);
    void updateReplayControls();
    void exportGameHistory();
    
    // Settings helpers
    void loadSettings();
    void saveSettings();
    void applySettings();
    
    // Animation helpers
    void animateCellPlacement(int row, int col, Player player);
    void animateGameOver(GameResult result);
    void animateScoreUpdate();
    void fadeInWidget(QWidget* widget);
    void fadeOutWidget(QWidget* widget);
    void slideInWidget(QWidget* widget);
    
    // Utility methods
    QString formatTime(int seconds);
    QString formatGameResult(GameResult result);
    QString getPlayerName(Player player);
    QColor getPlayerColor(Player player);
    void showNotification(const QString& message, const QString& type = "info");
    void playSound(const QString& soundName);
};

#endif // GUI_INTERFACE_H