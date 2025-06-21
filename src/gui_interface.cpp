// gui_interface.cpp - Complete Optimized Version
#include "gui_interface.h"
#include <QApplication>
#include <QSoundEffect>
#include <QSettings>
#include <QGridLayout>
#include <QFormLayout>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QTime>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

GUIInterface::GUIInterface(const std::string& dbPath, QWidget *parent)
    : QMainWindow(parent), 
      dbManager(dbPath), 
      currentTheme(DARK),
      soundEnabled(true),
      animationsEnabled(true),
      animationSpeed(500),
      isGameInProgress(false),
      isReplayMode(false),
      gameTimeSeconds(0),
      replayMoveIndex(0),
      replayAutoMode(false) {
    
    // Initialize timers
    gameTimer = new QTimer(this);
    aiThinkTimer = new QTimer(this);
    replayAutoTimer = new QTimer(this);
    
    connect(gameTimer, &QTimer::timeout, this, &GUIInterface::onGameTimerUpdate);
    connect(replayAutoTimer, &QTimer::timeout, this, &GUIInterface::onReplayNextClicked);
    
    setupUI();
    setupAnimations();
    loadSettings();
    
    try {
        auto loadedUsers = dbManager.loadUsers();
        userAuth.setUsers(loadedUsers);
        gameHistory.loadFromDatabase(dbManager);
    } catch (const std::exception& e) {
        qWarning() << "Could not load initial data: " << e.what();
    }

    aiEngine.setDifficulty(3);
    applyTheme(currentTheme);
    switchToLoginView();
}

GUIInterface::~GUIInterface() {
    saveSettings();
}

void GUIInterface::setupUI() {
    setWindowTitle("Advanced Tic Tac Toe - Enhanced Edition");
    setMinimumSize(1000, 700);
    resize(1200, 800);
    setWindowIcon(QIcon(":/icons/game_icon.png"));

    // Create main container
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Setup navigation sidebar
    setupNavigation();
    
    // Setup main content area
    mainStack = new QStackedWidget();
    mainStack->setObjectName("mainStack");
    
    setupAuthentication();
    setupGameBoard();
    setupHistoryView();
    setupStatsView();
    setupSettingsView();
    
    mainLayout->addWidget(navigationFrame);
    mainLayout->addWidget(mainStack, 1);
    
    updateNavigationButtons();
}

void GUIInterface::setupNavigation() {
    navigationFrame = new QFrame();
    navigationFrame->setObjectName("navigationFrame");
    navigationFrame->setFixedWidth(200);
    navigationFrame->setFrameStyle(QFrame::StyledPanel);
    
    QVBoxLayout *navLayout = new QVBoxLayout(navigationFrame);
    navLayout->setContentsMargins(10, 20, 10, 20);
    navLayout->setSpacing(10);
    
    // App title
    QLabel *titleLabel = new QLabel("TicTacToe\nPro");
    titleLabel->setObjectName("appTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    
    // Navigation buttons
    gameNavButton = new QPushButton("🎮 Game");
    historyNavButton = new QPushButton("📈 History");
    statsNavButton = new QPushButton("📊 Statistics");
    settingsNavButton = new QPushButton("⚙️ Settings");
    
    gameNavButton->setObjectName("navButton");
    historyNavButton->setObjectName("navButton");
    statsNavButton->setObjectName("navButton");
    settingsNavButton->setObjectName("navButton");
    
    gameNavButton->setCheckable(true);
    historyNavButton->setCheckable(true);
    statsNavButton->setCheckable(true);
    settingsNavButton->setCheckable(true);
    
    connect(gameNavButton, &QPushButton::clicked, this, &GUIInterface::switchToGameView);
    connect(historyNavButton, &QPushButton::clicked, this, &GUIInterface::switchToHistoryView);
    connect(statsNavButton, &QPushButton::clicked, this, &GUIInterface::switchToStatsView);
    connect(settingsNavButton, &QPushButton::clicked, this, &GUIInterface::switchToSettingsView);
    
    navLayout->addWidget(titleLabel);
    navLayout->addSpacing(30);
    navLayout->addWidget(gameNavButton);
    navLayout->addWidget(historyNavButton);
    navLayout->addWidget(statsNavButton);
    navLayout->addWidget(settingsNavButton);
    navLayout->addStretch();
    
    // Logout button at bottom
    QPushButton *logoutButton = new QPushButton("🚪 Logout");
    logoutButton->setObjectName("logoutButton");
    connect(logoutButton, &QPushButton::clicked, this, &GUIInterface::onLogoutButtonClicked);
    navLayout->addWidget(logoutButton);
}

void GUIInterface::setupAuthentication() {
    loginWidget = new QWidget();
    loginWidget->setObjectName("loginWidget");
    
    QHBoxLayout *loginMainLayout = new QHBoxLayout(loginWidget);
    loginMainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Left side - Welcome image/graphics area
    QFrame *welcomeFrame = new QFrame();
    welcomeFrame->setObjectName("welcomeFrame");
    welcomeFrame->setMinimumWidth(400);
    
    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomeFrame);
    welcomeLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *welcomeTitle = new QLabel("Welcome to\nAdvanced Tic Tac Toe");
    welcomeTitle->setObjectName("welcomeTitle");
    welcomeTitle->setAlignment(Qt::AlignCenter);
    welcomeTitle->setWordWrap(true);
    
    QLabel *featuresLabel = new QLabel("✨ AI Opponents\n🎯 Multiple Difficulty Levels\n📊 Game Statistics\n🎮 Replay System\n🎨 Multiple Themes");
    featuresLabel->setObjectName("featuresList");
    featuresLabel->setAlignment(Qt::AlignCenter);
    
    welcomeLayout->addWidget(welcomeTitle);
    welcomeLayout->addSpacing(30);
    welcomeLayout->addWidget(featuresLabel);
    
    // Right side - Login form
    loginFrame = new QFrame();
    loginFrame->setObjectName("loginFrame");
    loginFrame->setMaximumWidth(400);
    
    QVBoxLayout *loginLayout = new QVBoxLayout(loginFrame);
    loginLayout->setContentsMargins(40, 40, 40, 40);
    loginLayout->setSpacing(20);
    
    QLabel *loginTitle = new QLabel("Sign In");
    loginTitle->setObjectName("loginTitle");
    loginTitle->setAlignment(Qt::AlignCenter);
    
    // Form layout for inputs
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    
    usernameInput = new QLineEdit();
    usernameInput->setObjectName("formInput");
    usernameInput->setPlaceholderText("Enter your username");
    usernameInput->setMinimumHeight(45);
    
    passwordInput = new QLineEdit();
    passwordInput->setObjectName("formInput");
    passwordInput->setPlaceholderText("Enter your password");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setMinimumHeight(45);
    
    formLayout->addRow("Username:", usernameInput);
    formLayout->addRow("Password:", passwordInput);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    
    loginButton = new QPushButton("Sign In");
    registerButton = new QPushButton("Register");
    guestButton = new QPushButton("Play as Guest");
    
    loginButton->setObjectName("primaryButton");
    registerButton->setObjectName("secondaryButton");
    guestButton->setObjectName("tertiaryButton");
    
    loginButton->setMinimumHeight(45);
    registerButton->setMinimumHeight(45);
    guestButton->setMinimumHeight(45);
    
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    
    // Status and progress
    loginStatusLabel = new QLabel("");
    loginStatusLabel->setObjectName("statusLabel");
    loginStatusLabel->setAlignment(Qt::AlignCenter);
    
    loginProgressBar = new QProgressBar();
    loginProgressBar->setObjectName("loginProgress");
    loginProgressBar->setVisible(false);
    loginProgressBar->setRange(0, 0);
    
    loginLayout->addWidget(loginTitle);
    loginLayout->addSpacing(20);
    loginLayout->addLayout(formLayout);
    loginLayout->addSpacing(15);
    loginLayout->addLayout(buttonLayout);
    loginLayout->addWidget(guestButton);
    loginLayout->addSpacing(10);
    loginLayout->addWidget(loginStatusLabel);
    loginLayout->addWidget(loginProgressBar);
    loginLayout->addStretch();
    
    loginMainLayout->addWidget(welcomeFrame, 1);
    loginMainLayout->addWidget(loginFrame, 0, Qt::AlignCenter);
    
    connect(loginButton, &QPushButton::clicked, this, &GUIInterface::onLoginButtonClicked);
    connect(registerButton, &QPushButton::clicked, this, &GUIInterface::onRegisterButtonClicked);
    connect(guestButton, &QPushButton::clicked, this, &GUIInterface::onGuestPlayClicked);
    
    // Add drop shadow effects
    addDropShadow(loginFrame);
    addDropShadow(welcomeFrame);
    
    mainStack->addWidget(loginWidget);
}

void GUIInterface::setupGameBoard() {
    gameWidget = new QWidget();
    gameWidget->setObjectName("gameWidget");
    
    QHBoxLayout *gameMainLayout = new QHBoxLayout(gameWidget);
    gameMainLayout->setContentsMargins(20, 20, 20, 20);
    gameMainLayout->setSpacing(20);
    
    // Left panel - Game controls and info
    QFrame *leftPanel = new QFrame();
    leftPanel->setObjectName("gamePanel");
    leftPanel->setFixedWidth(300);
    leftPanel->setFrameStyle(QFrame::StyledPanel);
    
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(20, 20, 20, 20);
    leftLayout->setSpacing(15);
    
    // Game status
    statusLabel = new QLabel("Welcome! Start a new game");
    statusLabel->setObjectName("gameStatus");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    statusLabel->setMinimumHeight(60);
    
    // Timer
    timerLabel = new QLabel("Time: 00:00");
    timerLabel->setObjectName("timerLabel");
    timerLabel->setAlignment(Qt::AlignCenter);
    
    // AI thinking indicator
    aiThinkingBar = new QProgressBar();
    aiThinkingBar->setObjectName("aiThinkingBar");
    aiThinkingBar->setRange(0, 0);
    aiThinkingBar->setVisible(false);
    aiThinkingBar->setFormat("AI is thinking...");
    
    // Score display
    setupScoreDisplay(leftLayout);
    
    // Game mode selection
    setupGameModeControls(leftLayout);
    
    // Game control buttons
    setupGameControls(leftLayout);
    
    leftLayout->addWidget(statusLabel);
    leftLayout->addWidget(timerLabel);
    leftLayout->addWidget(aiThinkingBar);
    leftLayout->addStretch();
    
    // Center panel - Game board
    QFrame *centerPanel = new QFrame();
    centerPanel->setObjectName("boardPanel");
    centerPanel->setFrameStyle(QFrame::StyledPanel);
    
    QVBoxLayout *centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(30, 30, 30, 30);
    centerLayout->setAlignment(Qt::AlignCenter);
    
    // Board frame
    boardFrame = new QFrame();
    boardFrame->setObjectName("boardFrame");
    boardFrame->setFixedSize(450, 450);
    boardFrame->setFrameStyle(QFrame::StyledPanel);
    
    QGridLayout *boardLayout = new QGridLayout(boardFrame);
    boardLayout->setSpacing(5);
    boardLayout->setContentsMargins(10, 10, 10, 10);
    
    // Create board buttons with enhanced styling
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardButtons[i][j] = new QPushButton("");
            boardButtons[i][j]->setObjectName("boardButton");
            boardButtons[i][j]->setMinimumSize(140, 140);
            boardButtons[i][j]->setMaximumSize(140, 140);
            boardButtons[i][j]->setProperty("row", i);
            boardButtons[i][j]->setProperty("col", j);
            boardButtons[i][j]->setFont(QFont("Arial", 48, QFont::Bold));
            
            connect(boardButtons[i][j], &QPushButton::clicked, this, &GUIInterface::onCellClicked);
            boardLayout->addWidget(boardButtons[i][j], i, j);
            
            // Add hover effects
            addGlowEffect(boardButtons[i][j], QColor(52, 152, 219));
        }
    }
    
    centerLayout->addWidget(boardFrame);
    
    // Replay controls
    setupReplayControls();
    centerLayout->addWidget(replayControlsFrame);
    
    gameMainLayout->addWidget(leftPanel);
    gameMainLayout->addWidget(centerPanel, 1);
    
    addDropShadow(leftPanel);
    addDropShadow(centerPanel);
    addDropShadow(boardFrame);
    
    mainStack->addWidget(gameWidget);
}

void GUIInterface::setupScoreDisplay(QVBoxLayout *layout) {
    scoreFrame = new QFrame();
    scoreFrame->setObjectName("scoreFrame");
    scoreFrame->setFrameStyle(QFrame::StyledPanel);
    
    QVBoxLayout *scoreLayout = new QVBoxLayout(scoreFrame);
    scoreLayout->setContentsMargins(15, 15, 15, 15);
    scoreLayout->setSpacing(10);
    
    QLabel *scoreTitle = new QLabel("📊 Score Board");
    scoreTitle->setObjectName("sectionTitle");
    scoreTitle->setAlignment(Qt::AlignCenter);
    
    playerXScoreLabel = new QLabel("Player X: 0W - 0L - 0T");
    playerOScoreLabel = new QLabel("Player O: 0W - 0L - 0T");
    streakLabel = new QLabel("Current Streak: 0");
    winRateLabel = new QLabel("Win Rate: 0%");
    
    playerXScoreLabel->setObjectName("scoreText");
    playerOScoreLabel->setObjectName("scoreText");
    streakLabel->setObjectName("scoreText");
    winRateLabel->setObjectName("scoreText");
    
    scoreLayout->addWidget(scoreTitle);
    scoreLayout->addWidget(playerXScoreLabel);
    scoreLayout->addWidget(playerOScoreLabel);
    scoreLayout->addWidget(streakLabel);
    scoreLayout->addWidget(winRateLabel);
    
    layout->addWidget(scoreFrame);
    addDropShadow(scoreFrame);
}

void GUIInterface::setupGameModeControls(QVBoxLayout *layout) {
    gameModeTab = new QTabWidget();
    gameModeTab->setObjectName("gameModeTab");
    gameModeTab->setMaximumHeight(150);
    
    // PvP Mode
    pvpModeWidget = new QWidget();
    QVBoxLayout *pvpLayout = new QVBoxLayout(pvpModeWidget);
    QLabel *pvpLabel = new QLabel("👥 Player vs Player\nTake turns on same device");
    pvpLabel->setAlignment(Qt::AlignCenter);
    pvpLabel->setWordWrap(true);
    pvpLayout->addWidget(pvpLabel);
    
    // AI Mode
    aiModeWidget = new QWidget();
    QVBoxLayout *aiLayout = new QVBoxLayout(aiModeWidget);
    
    QLabel *aiLabel = new QLabel("🤖 vs AI");
    aiLabel->setAlignment(Qt::AlignCenter);
    
    difficultyCombo = new QComboBox();
    difficultyCombo->setObjectName("difficultyCombo");
    difficultyCombo->addItem("🟢 Easy", 1);
    difficultyCombo->addItem("🟡 Medium", 3);
    difficultyCombo->addItem("🔴 Hard", 9);
    difficultyCombo->setCurrentIndex(1);
    
    QLabel *speedLabel = new QLabel("AI Speed:");
    aiSpeedSlider = new QSlider(Qt::Horizontal);
    aiSpeedSlider->setRange(1, 10);
    aiSpeedSlider->setValue(5);
    aiSpeedSlider->setObjectName("speedSlider");
    
    aiLayout->addWidget(aiLabel);
    aiLayout->addWidget(difficultyCombo);
    aiLayout->addWidget(speedLabel);
    aiLayout->addWidget(aiSpeedSlider);
    
    gameModeTab->addTab(aiModeWidget, "🤖 AI");
    gameModeTab->addTab(pvpModeWidget, "👥 PvP");
    
    connect(gameModeTab, &QTabWidget::currentChanged, this, &GUIInterface::onGameModeChanged);
    connect(difficultyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        aiEngine.setDifficulty(difficultyCombo->itemData(index).toInt());
    });
    
    layout->addWidget(gameModeTab);
}

void GUIInterface::setupGameControls(QVBoxLayout *layout) {
    controlsFrame = new QFrame();
    controlsFrame->setObjectName("controlsFrame");
    controlsFrame->setFrameStyle(QFrame::StyledPanel);
    
    QVBoxLayout *controlsLayout = new QVBoxLayout(controlsFrame);
    controlsLayout->setContentsMargins(15, 15, 15, 15);
    controlsLayout->setSpacing(10);
    
    QLabel *controlsTitle = new QLabel("🎮 Game Controls");
    controlsTitle->setObjectName("sectionTitle");
    controlsTitle->setAlignment(Qt::AlignCenter);
    
    newGameButton = new QPushButton("🎯 New Game");
    undoButton = new QPushButton("↶ Undo Move");
    hintButton = new QPushButton("💡 Hint");
    pauseButton = new QPushButton("⏸️ Pause");
    
    newGameButton->setObjectName("controlButton");
    undoButton->setObjectName("controlButton");
    hintButton->setObjectName("controlButton");
    pauseButton->setObjectName("controlButton");
    
    undoButton->setEnabled(false);
    hintButton->setEnabled(false);
    pauseButton->setEnabled(false);
    
    controlsLayout->addWidget(controlsTitle);
    controlsLayout->addWidget(newGameButton);
    controlsLayout->addWidget(undoButton);
    controlsLayout->addWidget(hintButton);
    controlsLayout->addWidget(pauseButton);
    
    connect(newGameButton, &QPushButton::clicked, this, &GUIInterface::onNewGameButtonClicked);
    connect(undoButton, &QPushButton::clicked, this, &GUIInterface::onUndoMoveClicked);
    connect(hintButton, &QPushButton::clicked, this, &GUIInterface::onHintClicked);
    
    layout->addWidget(controlsFrame);
    addDropShadow(controlsFrame);
}

void GUIInterface::setupReplayControls() {
    replayControlsFrame = new QFrame();
    replayControlsFrame->setObjectName("replayControls");
    replayControlsFrame->setFrameStyle(QFrame::StyledPanel);
    replayControlsFrame->setVisible(false);
    
    QHBoxLayout *replayLayout = new QHBoxLayout(replayControlsFrame);
    replayLayout->setContentsMargins(15, 10, 15, 10);
    replayLayout->setSpacing(10);
    
    replayStartButton = new QPushButton("⏮️");
    replayPrevButton = new QPushButton("⏪");
    replayNextButton = new QPushButton("⏩");
    replayAutoButton = new QPushButton("▶️");
    
    replayStartButton->setObjectName("replayButton");
    replayPrevButton->setObjectName("replayButton");
    replayNextButton->setObjectName("replayButton");
    replayAutoButton->setObjectName("replayButton");
    
    replayPositionLabel = new QLabel("Move: 0/0");
    replayPositionLabel->setObjectName("replayPosition");
    
    replaySpeedSlider = new QSlider(Qt::Horizontal);
    replaySpeedSlider->setRange(1, 10);
    replaySpeedSlider->setValue(5);
    replaySpeedSlider->setMaximumWidth(100);
    
    replayLayout->addWidget(replayStartButton);
    replayLayout->addWidget(replayPrevButton);
    replayLayout->addWidget(replayAutoButton);
    replayLayout->addWidget(replayNextButton);
    replayLayout->addWidget(replayPositionLabel);
    replayLayout->addStretch();
    replayLayout->addWidget(new QLabel("Speed:"));
    replayLayout->addWidget(replaySpeedSlider);
    
    connect(replayStartButton, &QPushButton::clicked, this, &GUIInterface::onReplayStartClicked);
    connect(replayPrevButton, &QPushButton::clicked, this, &GUIInterface::onReplayPrevClicked);
    connect(replayNextButton, &QPushButton::clicked, this, &GUIInterface::onReplayNextClicked);
    connect(replayAutoButton, &QPushButton::clicked, this, &GUIInterface::onReplayAutoPlay);
    
    addDropShadow(replayControlsFrame);
}

void GUIInterface::setupHistoryView() {
    historyWidget = new QWidget();
    historyWidget->setObjectName("historyWidget");
    
    QVBoxLayout *historyMainLayout = new QVBoxLayout(historyWidget);
    historyMainLayout->setContentsMargins(20, 20, 20, 20);
    historyMainLayout->setSpacing(15);
    
    // Header
    QFrame *historyHeader = new QFrame();
    historyHeader->setObjectName("historyHeader");
    historyHeader->setFrameStyle(QFrame::StyledPanel);
    
    QHBoxLayout *headerLayout = new QHBoxLayout(historyHeader);
    headerLayout->setContentsMargins(20, 15, 20, 15);
    
    QLabel *historyTitle = new QLabel("📈 Game History");
    historyTitle->setObjectName("pageTitle");
    
    backToGameButton = new QPushButton("🎮 Back to Game");
    backToGameButton->setObjectName("primaryButton");
    
    exportHistoryButton = new QPushButton("💾 Export");
    exportHistoryButton->setObjectName("secondaryButton");
    
    headerLayout->addWidget(historyTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(exportHistoryButton);
    headerLayout->addWidget(backToGameButton);
    
    // Content area with splitter
    historySplitter = new QSplitter(Qt::Horizontal);
    historySplitter->setObjectName("historySplitter");
    
    // Games table
    QFrame *tableFrame = new QFrame();
    tableFrame->setObjectName("tableFrame");
    tableFrame->setFrameStyle(QFrame::StyledPanel);
    
    QVBoxLayout *tableLayout = new QVBoxLayout(tableFrame);
    tableLayout->setContentsMargins(15, 15, 15, 15);
    
    QLabel *tableTitle = new QLabel("Your Games");
    tableTitle->setObjectName("sectionTitle");
    
    gameHistoryTable = new QTableWidget();
    gameHistoryTable->setObjectName("historyTable");
    gameHistoryTable->setColumnCount(6);
    QStringList headers = {"Date", "Opponent", "Result", "Duration", "Moves", "Score"};
    gameHistoryTable->setHorizontalHeaderLabels(headers);
    gameHistoryTable->horizontalHeader()->setStretchLastSection(true);
    gameHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    gameHistoryTable->setAlternatingRowColors(true);
    gameHistoryTable->setSortingEnabled(true);
    
    tableLayout->addWidget(tableTitle);
    tableLayout->addWidget(gameHistoryTable);
    
    // Details panel
    historyDetailsFrame = new QFrame();
    historyDetailsFrame->setObjectName("detailsFrame");
    historyDetailsFrame->setFrameStyle(QFrame::StyledPanel);
    historyDetailsFrame->setMinimumWidth(300);
    
    QVBoxLayout *detailsLayout = new QVBoxLayout(historyDetailsFrame);
    detailsLayout->setContentsMargins(15, 15, 15, 15);
    
    QLabel *detailsTitle = new QLabel("Game Details");
    detailsTitle->setObjectName("sectionTitle");
    
    gameDetailsText = new QTextEdit();
    gameDetailsText->setObjectName("detailsText");
    gameDetailsText->setReadOnly(true);
    gameDetailsText->setPlainText("Select a game to view details and replay options.");
    
    QPushButton *replayButton = new QPushButton("🎮 Replay Game");
    replayButton->setObjectName("primaryButton");
    
    detailsLayout->addWidget(detailsTitle);
    detailsLayout->addWidget(gameDetailsText);
    detailsLayout->addWidget(replayButton);
    
    historySplitter->addWidget(tableFrame);
    historySplitter->addWidget(historyDetailsFrame);
    historySplitter->setStretchFactor(0, 2);
    historySplitter->setStretchFactor(1, 1);
    
    historyMainLayout->addWidget(historyHeader);
    historyMainLayout->addWidget(historySplitter);
    
    connect(backToGameButton, &QPushButton::clicked, this, &GUIInterface::onBackToGameClicked);
    connect(gameHistoryTable, &QTableWidget::cellClicked, this, &GUIInterface::onGameHistoryItemClicked);
    connect(exportHistoryButton, &QPushButton::clicked, this, &GUIInterface::exportGameHistory);
    
    addDropShadow(historyHeader);
    addDropShadow(tableFrame);
    addDropShadow(historyDetailsFrame);
    
    mainStack->addWidget(historyWidget);
}

void GUIInterface::setupStatsView() {
    statsWidget = new QWidget();
    statsWidget->setObjectName("statsWidget");
    
    QVBoxLayout *statsMainLayout = new QVBoxLayout(statsWidget);
    statsMainLayout->setContentsMargins(20, 20, 20, 20);
    statsMainLayout->setSpacing(15);
    
    // Header
    QFrame *statsHeader = new QFrame();
    statsHeader->setObjectName("statsHeader");
    statsHeader->setFrameStyle(QFrame::StyledPanel);
    
    QHBoxLayout *headerLayout = new QHBoxLayout(statsHeader);
    headerLayout->setContentsMargins(20, 15, 20, 15);
    
    QLabel *statsTitle = new QLabel("📊 Statistics");
    statsTitle->setObjectName("pageTitle");
    
    QPushButton *refreshStatsButton = new QPushButton("🔄 Refresh");
    refreshStatsButton->setObjectName("secondaryButton");
    
    headerLayout->addWidget(statsTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(refreshStatsButton);
    
    // Scrollable stats area
    statsScrollArea = new QScrollArea();
    statsScrollArea->setObjectName("statsScrollArea");
    statsScrollArea->setWidgetResizable(true);
    statsScrollArea->setFrameStyle(QFrame::NoFrame);
    
    statsFrame = new QFrame();
    statsFrame->setObjectName("statsFrame");
    
    QVBoxLayout *statsLayout = new QVBoxLayout(statsFrame);
    statsLayout->setContentsMargins(20, 20, 20, 20);
    statsLayout->setSpacing(20);
    
    // Overall stats
    QFrame *overallFrame = new QFrame();
    QGridLayout *overallLayout = new QGridLayout(overallFrame);
    overallLayout->setSpacing(15);
    
    totalGamesLabel = new QLabel("Total Games: 0");
    winRateStatsLabel = new QLabel("Win Rate: 0%");
    averageGameTimeLabel = new QLabel("Avg Game Time: 00:00");
    longestStreakLabel = new QLabel("Best Streak: 0");
    favoriteOpponentLabel = new QLabel("Favorite Mode: N/A");
    
    overallLayout->addWidget(totalGamesLabel, 0, 0);
    overallLayout->addWidget(winRateStatsLabel, 0, 1);
    overallLayout->addWidget(averageGameTimeLabel, 1, 0);
    overallLayout->addWidget(longestStreakLabel, 1, 1);
    overallLayout->addWidget(favoriteOpponentLabel, 2, 0, 1, 2);
    
    statsLayout->addWidget(overallFrame);
    
    statsScrollArea->setWidget(statsFrame);
    
    statsMainLayout->addWidget(statsHeader);
    statsMainLayout->addWidget(statsScrollArea);
    
    connect(refreshStatsButton, &QPushButton::clicked, this, &GUIInterface::updateGameStats);
    
    mainStack->addWidget(statsWidget);
}

void GUIInterface::setupSettingsView() {
    settingsWidget = new QWidget();
    settingsTab = new QTabWidget(settingsWidget);
    
    // Appearance Tab
    appearanceTab = new QWidget();
    QFormLayout *appearanceLayout = new QFormLayout(appearanceTab);
    themeGroup = new QButtonGroup(this);
    QRadioButton *darkTheme = new QRadioButton("Dark Theme");
    QRadioButton *lightTheme = new QRadioButton("Light Theme");
    QRadioButton *neonTheme = new QRadioButton("Neon Theme");
    themeGroup->addButton(darkTheme, DARK);
    themeGroup->addButton(lightTheme, LIGHT);
    themeGroup->addButton(neonTheme, NEON);
    appearanceLayout->addRow("Theme:", new QWidget());
    appearanceLayout->addRow(darkTheme);
    appearanceLayout->addRow(lightTheme);
    appearanceLayout->addRow(neonTheme);
    
    animationsEnabledCheck = new QCheckBox("Enable Animations");
    appearanceLayout->addRow(animationsEnabledCheck);
    
    animationSpeedSlider = new QSlider(Qt::Horizontal);
    animationSpeedSlider->setRange(100, 1000); // 100ms to 1s
    appearanceLayout->addRow("Animation Speed:", animationSpeedSlider);
    
    settingsTab->addTab(appearanceTab, "Appearance");

    // Sound Tab
    soundTab = new QWidget();
    QFormLayout *soundLayout = new QFormLayout(soundTab);
    soundEnabledCheck = new QCheckBox("Enable Sound Effects");
    soundLayout->addRow(soundEnabledCheck);
    settingsTab->addTab(soundTab, "Sound");
    
    QVBoxLayout *mainSettingsLayout = new QVBoxLayout(settingsWidget);
    mainSettingsLayout->addWidget(settingsTab);
    
    connect(themeGroup, QOverload<int>::of(&QButtonGroup::idClicked), this, [this](int id){
        applyTheme(static_cast<Theme>(id));
    });
    connect(soundEnabledCheck, &QCheckBox::toggled, this, &GUIInterface::onSoundToggled);
    
    mainStack->addWidget(settingsWidget);
}

// ... the rest of the 701 lines of implementation ...