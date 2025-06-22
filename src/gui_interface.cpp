// gui_interface.cpp - Complete Implementation
#include "gui_interface.h"
#include <QApplication>
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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardItemModel>
#include <QIcon>
#include <QTimer>

static const int TURN_TIME_LIMIT_S = 15;

GUIInterface::GUIInterface(const std::string& dbPath, QWidget *parent)
    : QMainWindow(parent),
      dbManager(dbPath),
      currentTheme(DARK),
      animationsEnabled(true),
      animationSpeed(300),
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
    loadSettings();
    applyTheme(currentTheme);
    try {
        auto loadedUsers = dbManager.loadUsers();
        userAuth.setUsers(loadedUsers);
        gameHistory.loadFromDatabase(dbManager);
    } catch (const std::exception& e) {
        qWarning() << "Could not load initial data: " << e.what();
    }

    aiEngine.setDifficulty(3);
    switchToLoginView();
}

GUIInterface::~GUIInterface() {
    saveSettings();
}

// --- SETUP FUNCTIONS ---

void GUIInterface::setupUI() {
    setWindowTitle("Advanced Tic Tac Toe - Pro Edition");
    setMinimumSize(1100, 750);
    resize(1200, 800);

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

        QButtonGroup* navGroup = new QButtonGroup(this);
    navGroup->setExclusive(true);
    QPushButton* navButtons[] = {gameNavButton, historyNavButton, statsNavButton, settingsNavButton};
    for (auto* button : {gameNavButton, historyNavButton, statsNavButton, settingsNavButton}) {
        button->setObjectName("navButton");
        button->setCheckable(true);
        navGroup->addButton(button);
        navLayout->addWidget(button);
    }

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
    navLayout->addWidget(logoutButton, 0, Qt::AlignBottom);
}

void GUIInterface::setupAuthentication() {
    loginWidget = new QWidget();
    loginWidget->setObjectName("loginWidget");
    QHBoxLayout *loginMainLayout = new QHBoxLayout(loginWidget);
    loginMainLayout->setContentsMargins(0, 0, 0, 0);
    // Welcome frame
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
    // Login form
    loginFrame = new QFrame();
    loginFrame->setObjectName("loginFrame");
    loginFrame->setMaximumWidth(400);
    QVBoxLayout *loginLayout = new QVBoxLayout(loginFrame);
    loginLayout->setContentsMargins(40, 40, 40, 40);
    loginLayout->setSpacing(20);
    QLabel *loginTitle = new QLabel("Sign In");
    loginTitle->setObjectName("loginTitle");
    loginTitle->setAlignment(Qt::AlignCenter);
        // Form inputs
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
    for (auto* button : {loginButton, registerButton, guestButton}) {
        button->setMinimumHeight(45);
    }
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
    // Enable Enter key for login
    connect(passwordInput, &QLineEdit::returnPressed, this, &GUIInterface::onLoginButtonClicked);
    addDropShadow(loginFrame);
    addDropShadow(welcomeFrame);
    mainStack->addWidget(loginWidget);
}

void GUIInterface::setupGameBoard() {
     gameWidget = new QWidget();
    QHBoxLayout *gameMainLayout = new QHBoxLayout(gameWidget);
    gameMainLayout->setContentsMargins(20, 20, 20, 20);
    gameMainLayout->setSpacing(20);

    QFrame *leftPanel = new QFrame();
    leftPanel->setFixedWidth(300);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(15);
    
    QFrame *centerPanel = new QFrame();
    QVBoxLayout *centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setAlignment(Qt::AlignCenter);
    
    statusLabel = new QLabel("Welcome! Start a new game.");
    statusLabel->setObjectName("statusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);
    
    timerLabel = new QLabel("Time: 00:00");
    timerLabel->setAlignment(Qt::AlignCenter);

    boardFrame = new QFrame();
    boardFrame->setFixedSize(450, 450);
    QGridLayout *boardLayout = new QGridLayout(boardFrame);
    boardLayout->setSpacing(10);
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardButtons[i][j] = new QPushButton("");
            boardButtons[i][j]->setObjectName("boardButton");
            boardButtons[i][j]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            boardButtons[i][j]->setProperty("row", i);
            boardButtons[i][j]->setProperty("col", j);
            connect(boardButtons[i][j], &QPushButton::clicked, this, &GUIInterface::onCellClicked);
            boardLayout->addWidget(boardButtons[i][j], i, j);
        }
    }
    
    setupReplayControls();
    
    centerLayout->addWidget(statusLabel);
    centerLayout->addWidget(timerLabel);
    centerLayout->addWidget(boardFrame, 1);
    centerLayout->addWidget(replayControlsFrame);

    setupScoreDisplay(leftLayout);
    setupGameModeControls(leftLayout);
    setupGameControls(leftLayout);
    leftLayout->addStretch();

    gameMainLayout->addWidget(leftPanel);
    gameMainLayout->addWidget(centerPanel, 1);
    
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
    playerXScoreLabel = new QLabel("Player X: 0 wins");
    playerOScoreLabel = new QLabel("Player O: 0 wins");
    streakLabel = new QLabel("Current Streak: 0");
    winRateLabel = new QLabel("Win Rate: 0%"); 
    for (auto* label : {playerXScoreLabel, playerOScoreLabel, streakLabel, winRateLabel}) {
        label->setObjectName("scoreText");
    }
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
    // AI Mode
    QWidget *aiModeWidget = new QWidget();
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
    // PvP Mode
    QWidget *pvpModeWidget = new QWidget();
    QVBoxLayout *pvpLayout = new QVBoxLayout(pvpModeWidget);
    QLabel *pvpLabel = new QLabel("👥 Player vs Player\nTake turns on same device");
    pvpLabel->setAlignment(Qt::AlignCenter);
    pvpLabel->setWordWrap(true);
    pvpLayout->addWidget(pvpLabel);
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
    for (auto* button : {newGameButton, undoButton, hintButton}) {
        button->setObjectName("controlButton");
    }
    undoButton->setEnabled(false);
    hintButton->setEnabled(false);
    controlsLayout->addWidget(controlsTitle);
    controlsLayout->addWidget(newGameButton);
    controlsLayout->addWidget(undoButton);
    controlsLayout->addWidget(hintButton);
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
    
    for (auto* button : {replayStartButton, replayPrevButton, replayNextButton, replayAutoButton}) {
        button->setObjectName("replayButton");
        button->setFixedSize(40, 40);
    }
    
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
    
    // Stats content
    QScrollArea *statsScrollArea = new QScrollArea();
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
    overallFrame->setObjectName("statsCard");
    overallFrame->setFrameStyle(QFrame::StyledPanel);
    
    QGridLayout *overallLayout = new QGridLayout(overallFrame);
    overallLayout->setSpacing(15);
    overallLayout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *overallTitle = new QLabel("📊 Overall Statistics");
    overallTitle->setObjectName("sectionTitle");
    overallLayout->addWidget(overallTitle, 0, 0, 1, 2);
    
    totalGamesLabel = new QLabel("Total Games: 0");
    winRateStatsLabel = new QLabel("Win Rate: 0%");
    averageGameTimeLabel = new QLabel("Avg Game Time: 00:00");
    longestStreakLabel = new QLabel("Best Streak: 0");
    favoriteOpponentLabel = new QLabel("Favorite Mode: N/A");
    
    for (auto* label : {totalGamesLabel, winRateStatsLabel, averageGameTimeLabel, longestStreakLabel, favoriteOpponentLabel}) {
        label->setObjectName("statsText");
    }
    
    overallLayout->addWidget(totalGamesLabel, 1, 0);
    overallLayout->addWidget(winRateStatsLabel, 1, 1);
    overallLayout->addWidget(averageGameTimeLabel, 2, 0);
    overallLayout->addWidget(longestStreakLabel, 2, 1);
    overallLayout->addWidget(favoriteOpponentLabel, 3, 0, 1, 2);
    
    statsLayout->addWidget(overallFrame);
    statsScrollArea->setWidget(statsFrame);
    
    statsMainLayout->addWidget(statsHeader);
    statsMainLayout->addWidget(statsScrollArea);
    
    connect(refreshStatsButton, &QPushButton::clicked, this, &GUIInterface::updateGameStats);
    
    addDropShadow(statsHeader);
addDropShadow(overallFrame);
    
    mainStack->addWidget(statsWidget);
}

void GUIInterface::setupSettingsView() {
    settingsWidget = new QWidget();
    settingsWidget->setObjectName("settingsWidget");
    
    QVBoxLayout *settingsMainLayout = new QVBoxLayout(settingsWidget);
    settingsMainLayout->setContentsMargins(20, 20, 20, 20);
    settingsMainLayout->setSpacing(15);
    
    // Header
    QFrame *settingsHeader = new QFrame();
    settingsHeader->setObjectName("settingsHeader");
    settingsHeader->setFrameStyle(QFrame::StyledPanel);
    
    QHBoxLayout *headerLayout = new QHBoxLayout(settingsHeader);
    headerLayout->setContentsMargins(20, 15, 20, 15);
    
    QLabel *settingsTitle = new QLabel("⚙️ Settings");
    settingsTitle->setObjectName("pageTitle");
    
    QPushButton *resetSettingsButton = new QPushButton("🔄 Reset to Defaults");
    resetSettingsButton->setObjectName("secondaryButton");
    
    headerLayout->addWidget(settingsTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(resetSettingsButton);
    
    // Settings content
    QScrollArea *settingsScrollArea = new QScrollArea();
    settingsScrollArea->setObjectName("settingsScrollArea");
    settingsScrollArea->setWidgetResizable(true);
    settingsScrollArea->setFrameStyle(QFrame::NoFrame);
    
    QFrame *settingsContentFrame = new QFrame();
    settingsContentFrame->setObjectName("settingsContentFrame");
    
    QVBoxLayout *contentLayout = new QVBoxLayout(settingsContentFrame);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(20);
    
    // Appearance settings
    QGroupBox *appearanceGroup = new QGroupBox("🎨 Appearance");
    appearanceGroup->setObjectName("settingsGroup");
    QVBoxLayout *appearanceLayout = new QVBoxLayout(appearanceGroup);
    
    // Theme selection
    QLabel *themeLabel = new QLabel("Theme:");
    themeLabel->setObjectName("settingsLabel");
    
    themeGroup = new QButtonGroup(this);
    QHBoxLayout *themeButtonLayout = new QHBoxLayout();
    
    QRadioButton *darkThemeRadio = new QRadioButton("🌙 Dark");
    QRadioButton *lightThemeRadio = new QRadioButton("☀️ Light");
    QRadioButton *neonThemeRadio = new QRadioButton("⚡ Neon");
    
    darkThemeRadio->setObjectName("themeRadio");
    lightThemeRadio->setObjectName("themeRadio");
    neonThemeRadio->setObjectName("themeRadio");
    
    themeGroup->addButton(darkThemeRadio, DARK);
    themeGroup->addButton(lightThemeRadio, LIGHT);
    themeGroup->addButton(neonThemeRadio, NEON);
    
    darkThemeRadio->setChecked(true);
    
    themeButtonLayout->addWidget(darkThemeRadio);
    themeButtonLayout->addWidget(lightThemeRadio);
    themeButtonLayout->addWidget(neonThemeRadio);
    themeButtonLayout->addStretch();
    
    // Animation settings
    animationsEnabledCheck = new QCheckBox("Enable Animations");
    animationsEnabledCheck->setObjectName("settingsCheckBox");
    animationsEnabledCheck->setChecked(true);
    
    QLabel *animationSpeedLabel = new QLabel("Animation Speed:");
    animationSpeedLabel->setObjectName("settingsLabel");
    
    animationSpeedSlider = new QSlider(Qt::Horizontal);
    animationSpeedSlider->setObjectName("settingsSlider");
    animationSpeedSlider->setRange(1, 10);
    animationSpeedSlider->setValue(5);
    
    QLabel *speedValueLabel = new QLabel("5");
    speedValueLabel->setObjectName("speedValue");
    
    QHBoxLayout *speedLayout = new QHBoxLayout();
    speedLayout->addWidget(animationSpeedSlider);
    speedLayout->addWidget(speedValueLabel);
    
    appearanceLayout->addWidget(themeLabel);
    appearanceLayout->addLayout(themeButtonLayout);
    appearanceLayout->addSpacing(10);
    appearanceLayout->addWidget(animationsEnabledCheck);
    appearanceLayout->addWidget(animationSpeedLabel);
    appearanceLayout->addLayout(speedLayout);
    
    // Game settings
    QGroupBox *gameGroup = new QGroupBox("🎮 Game Settings");
    gameGroup->setObjectName("settingsGroup");
    QVBoxLayout *gameLayout = new QVBoxLayout(gameGroup);
    
    QLabel *autoSaveLabel = new QLabel("Auto-save interval (minutes):");
    autoSaveLabel->setObjectName("settingsLabel");
    
    autoSaveSpinBox = new QSpinBox();
    autoSaveSpinBox->setObjectName("settingsSpinBox");
    autoSaveSpinBox->setRange(1, 60);
    autoSaveSpinBox->setValue(5);
    autoSaveSpinBox->setSuffix(" min");
    
    QCheckBox *soundEnabledCheck = new QCheckBox("Enable Sound Effects");
    soundEnabledCheck->setObjectName("settingsCheckBox");
    soundEnabledCheck->setChecked(false);
    soundEnabledCheck->setEnabled(false); // Disabled as sound was removed
    
    QCheckBox *showHintsCheck = new QCheckBox("Show Move Hints");
    showHintsCheck->setObjectName("settingsCheckBox");
    showHintsCheck->setChecked(true);
    
    gameLayout->addWidget(autoSaveLabel);
    gameLayout->addWidget(autoSaveSpinBox);
    gameLayout->addWidget(soundEnabledCheck);
    gameLayout->addWidget(showHintsCheck);
    
    contentLayout->addWidget(appearanceGroup);
    contentLayout->addWidget(gameGroup);
    contentLayout->addStretch();
    
    settingsScrollArea->setWidget(settingsContentFrame);
    
    settingsMainLayout->addWidget(settingsHeader);
    settingsMainLayout->addWidget(settingsScrollArea);
    
    // Connect signals
    connect(themeGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &GUIInterface::onThemeChanged);
    connect(animationSpeedSlider, &QSlider::valueChanged, speedValueLabel, QOverload<int>::of(&QLabel::setNum));
    connect(animationSpeedSlider, &QSlider::valueChanged, this, &GUIInterface::onAnimationSpeedChanged);
    connect(resetSettingsButton, &QPushButton::clicked, this, [this]() {
        currentTheme = DARK;
        animationsEnabled = true;
        animationSpeed = 500;
        themeGroup->button(DARK)->setChecked(true);
        animationsEnabledCheck->setChecked(true);
        animationSpeedSlider->setValue(5);
        autoSaveSpinBox->setValue(5);
        applySettings();
    });
    
    addDropShadow(settingsHeader);
    addDropShadow(appearanceGroup);
    addDropShadow(gameGroup);
    
    mainStack->addWidget(settingsWidget);
}

// Theme and styling methods
void GUIInterface::applyTheme(Theme theme) {
    currentTheme = theme;
    QString styleSheet;
    
    switch (theme) {
        case DARK:
            styleSheet = R"(
                QMainWindow { background-color: #1e1e1e; color: #ffffff; }
                QWidget { background-color: #1e1e1e; color: #ffffff; }
                
                #navigationFrame { background-color: #2d2d2d; border-right: 2px solid #404040; }
                #appTitle { font-size: 18px; font-weight: bold; color: #00d4ff; margin-bottom: 10px; }
                #navButton { 
                    background-color: #404040; border: none; border-radius: 8px; padding: 12px; 
                    text-align: left; font-size: 14px; margin: 2px 0;
                }
                #navButton:hover { background-color: #505050; }
                #navButton:checked { background-color: #00d4ff; color: #000000; }
                #logoutButton { 
                    background-color: #d32f2f; border: none; border-radius: 8px; padding: 12px;
                    color: white; font-size: 14px;
                }
                #logoutButton:hover { background-color: #f44336; }
                
                QFrame { background-color: #2d2d2d; border-radius: 12px; }
                #welcomeFrame { background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #1e1e1e, stop: 1 #2d2d2d); }
                #loginFrame { background-color: #333333; }
                #welcomeTitle { font-size: 32px; font-weight: bold; color: #00d4ff; margin-bottom: 20px; }
                #featuresList { font-size: 16px; line-height: 1.8; color: #cccccc; }
                #loginTitle { font-size: 24px; font-weight: bold; color: #ffffff; margin-bottom: 20px; }
                
                #formInput { 
                    background-color: #404040; border: 2px solid #606060; border-radius: 8px; 
                    padding: 12px; font-size: 14px; color: #ffffff;
                }
                #formInput:focus { border-color: #00d4ff; }
                
                #primaryButton { 
                    background-color: #00d4ff; color: #000000; border: none; border-radius: 8px; 
                    font-weight: bold; font-size: 14px;
                }
                #primaryButton:hover { background-color: #00b8e6; }
                #secondaryButton { 
                    background-color: #404040; color: #ffffff; border: 2px solid #606060; 
                    border-radius: 8px; font-size: 14px;
                }
                #secondaryButton:hover { background-color: #505050; }
                #tertiaryButton { 
                    background-color: transparent; color: #00d4ff; border: 2px solid #00d4ff; 
                    border-radius: 8px; font-size: 14px;
                }
                #tertiaryButton:hover { background-color: #00d4ff; color: #000000; }
                
                #gameStatus { 
                    background-color: #333333; border-radius: 12px; padding: 15px; 
                    font-size: 16px; font-weight: bold; text-align: center;
                }
                #timerLabel { font-size: 18px; font-weight: bold; color: #00d4ff; }
                
                #boardButton { 
                    background-color: #404040; border: 2px solid #606060; border-radius: 12px; 
                    font-size: 48px; font-weight: bold; color: #ffffff;
                }
                #boardButton:hover { background-color: #505050; border-color: #00d4ff; }
                #boardButton:pressed { background-color: #606060; }
                
                #scoreFrame, #controlsFrame, #replayControls { 
                    background-color: #333333; border: 1px solid #404040; 
                }
                #sectionTitle { font-size: 16px; font-weight: bold; color: #00d4ff; }
                #scoreText { font-size: 14px; color: #cccccc; }
                
                #controlButton { 
                    background-color: #404040; border: 2px solid #606060; border-radius: 8px; 
                    padding: 10px; font-size: 14px; color: #ffffff;
                }
                #controlButton:hover { background-color: #505050; border-color: #00d4ff; }
                #controlButton:disabled { background-color: #2d2d2d; color: #666666; border-color: #404040; }
                
                QTableWidget { 
                    background-color: #2d2d2d; alternate-background-color: #333333; 
                    gridline-color: #404040; selection-background-color: #00d4ff;
                }
                QHeaderView::section { 
                    background-color: #404040; color: #ffffff; border: 1px solid #606060; 
                    padding: 8px; font-weight: bold;
                }
                
                QProgressBar { 
                    background-color: #404040; border: 2px solid #606060; border-radius: 8px; 
                    text-align: center; color: #ffffff;
                }
                QProgressBar::chunk { background-color: #00d4ff; border-radius: 6px; }
                
                QComboBox, QSlider, QSpinBox { 
                    background-color: #404040; border: 2px solid #606060; border-radius: 6px; 
                    padding: 6px; color: #ffffff;
                }
                QComboBox:hover, QSlider:hover, QSpinBox:hover { border-color: #00d4ff; }
                
                QGroupBox { 
                    font-weight: bold; border: 2px solid #404040; border-radius: 8px; 
                    margin: 8px 0; padding-top: 10px;
                }
                QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 8px; }
                
                QRadioButton, QCheckBox { spacing: 8px; }
                QRadioButton::indicator, QCheckBox::indicator { 
                    width: 16px; height: 16px; border: 2px solid #606060; border-radius: 8px;
                }
                QRadioButton::indicator:checked, QCheckBox::indicator:checked { 
                    background-color: #00d4ff; border-color: #00d4ff;
                }
            )";
            break;
            
        case LIGHT:
            styleSheet = R"(
                QMainWindow { background-color: #f5f5f5; color: #333333; }
                QWidget { background-color: #f5f5f5; color: #333333; }
                
                #navigationFrame { background-color: #ffffff; border-right: 2px solid #e0e0e0; }
                #appTitle { font-size: 18px; font-weight: bold; color: #1976d2; margin-bottom: 10px; }
                #navButton { 
                    background-color: #e8e8e8; border: none; border-radius: 8px; padding: 12px; 
                    text-align: left; font-size: 14px; margin: 2px 0; color: #333333;
                }
                #navButton:hover { background-color: #d0d0d0; }
                #navButton:checked { background-color: #1976d2; color: #ffffff; }
                #logoutButton { 
                    background-color: #d32f2f; border: none; border-radius: 8px; padding: 12px;
                    color: white; font-size: 14px;
                }
                #logoutButton:hover { background-color: #f44336; }
                
                QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e0e0e0; }
                #welcomeFrame { background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #f5f5f5, stop: 1 #ffffff); }
                #loginFrame { background-color: #ffffff; }
                #welcomeTitle { font-size: 32px; font-weight: bold; color: #1976d2; margin-bottom: 20px; }
                #featuresList { font-size: 16px; line-height: 1.8; color: #666666; }
                #loginTitle { font-size: 24px; font-weight: bold; color: #333333; margin-bottom: 20px; }
                
                #formInput { 
                    background-color: #ffffff; border: 2px solid #e0e0e0; border-radius: 8px; 
                    padding: 12px; font-size: 14px; color: #333333;
                }
                #formInput:focus { border-color: #1976d2; }
                
                #primaryButton { 
                    background-color: #1976d2; color: #ffffff; border: none; border-radius: 8px; 
                    font-weight: bold; font-size: 14px;
                }
                #primaryButton:hover { background-color: #1565c0; }
                #secondaryButton { 
                    background-color: #ffffff; color: #333333; border: 2px solid #e0e0e0; 
                    border-radius: 8px; font-size: 14px;
                }
                #secondaryButton:hover { background-color: #f5f5f5; }
                #tertiaryButton { 
                    background-color: transparent; color: #1976d2; border: 2px solid #1976d2; 
                    border-radius: 8px; font-size: 14px;
                }
                #tertiaryButton:hover { background-color: #1976d2; color: #ffffff; }
                
                #gameStatus { 
                    background-color: #ffffff; border: 1px solid #e0e0e0; border-radius: 12px; 
                    padding: 15px; font-size: 16px; font-weight: bold; text-align: center;
                }
                #timerLabel { font-size: 18px; font-weight: bold; color: #1976d2; }
                
                #boardButton { 
                    background-color: #ffffff; border: 2px solid #e0e0e0; border-radius: 12px; 
                    font-size: 48px; font-weight: bold; color: #333333;
                }
                #boardButton:hover { background-color: #f5f5f5; border-color: #1976d2; }
                #boardButton:pressed { background-color: #e0e0e0; }
                
                #scoreFrame, #controlsFrame, #replayControls { 
                    background-color: #ffffff; border: 1px solid #e0e0e0; 
                }
                #sectionTitle { font-size: 16px; font-weight: bold; color: #1976d2; }
                #scoreText { font-size: 14px; color: #666666; }
                
                #controlButton { 
                    background-color: #ffffff; border: 2px solid #e0e0e0; border-radius: 8px; 
                    padding: 10px; font-size: 14px; color: #333333;
                }
                #controlButton:hover { background-color: #f5f5f5; border-color: #1976d2; }
                #controlButton:disabled { background-color: #f5f5f5; color: #cccccc; border-color: #e0e0e0; }
                
                QTableWidget { 
                    background-color: #ffffff; alternate-background-color: #f5f5f5; 
                    gridline-color: #e0e0e0; selection-background-color: #1976d2;
                }
                QHeaderView::section { 
                    background-color: #e0e0e0; color: #333333; border: 1px solid #cccccc; 
                    padding: 8px; font-weight: bold;
                }
                
                QProgressBar { 
                    background-color: #ffffff; border: 2px solid #e0e0e0; border-radius: 8px; 
                    text-align: center; color: #333333;
                }
                QProgressBar::chunk { background-color: #1976d2; border-radius: 6px; }
                
                QComboBox, QSlider, QSpinBox { 
                    background-color: #ffffff; border: 2px solid #e0e0e0; border-radius: 6px; 
                    padding: 6px; color: #333333;
                }
                QComboBox:hover, QSlider:hover, QSpinBox:hover { border-color: #1976d2; }
                
                QGroupBox { 
                    font-weight: bold; border: 2px solid #e0e0e0; border-radius: 8px; 
                    margin: 8px 0; padding-top: 10px;
                }
                QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 8px; }
                
                QRadioButton, QCheckBox { spacing: 8px; }
                QRadioButton::indicator, QCheckBox::indicator { 
                    width: 16px; height: 16px; border: 2px solid #e0e0e0; border-radius: 8px;
                }
                QRadioButton::indicator:checked, QCheckBox::indicator:checked { 
                    background-color: #1976d2; border-color: #1976d2;
                }
            )";
            break;
            
        case NEON:
            styleSheet = R"(
                QMainWindow { background-color: #0a001a; color: #ff00ff; }
                QWidget { background-color: #0a001a; color: #ff00ff; }
                
                #navigationFrame { background-color: #1a0033; border-right: 2px solid #ff00ff; }
                #appTitle { font-size: 18px; font-weight: bold; color: #00ffff; margin-bottom: 10px; }
                #navButton { 
                    background-color: #330066; border: 2px solid #ff00ff; border-radius: 8px; 
                    padding: 12px; text-align: left; font-size: 14px; margin: 2px 0; color: #ff00ff;
                }
                #navButton:hover { background-color: #4d0099; border-color: #00ffff; }
                #navButton:checked { background-color: #ff00ff; color: #000000; }
                #logoutButton { 
                    background-color: #ff0066; border: 2px solid #ff00ff; border-radius: 8px; 
                    padding: 12px; color: #ffffff; font-size: 14px;
                }
                #logoutButton:hover { background-color: #ff3399; }
                
                QFrame { background-color: #1a0033; border: 2px solid #ff00ff; border-radius: 12px; }
                #welcomeFrame { 
                    background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #0a001a, stop: 1 #1a0033); 
                    border-color: #00ffff;
                }
                #loginFrame { background-color: #1a0033; border-color: #ff00ff; }
                #welcomeTitle { font-size: 32px; font-weight: bold; color: #00ffff; margin-bottom: 20px; }
                #featuresList { font-size: 16px; line-height: 1.8; color: #ff66ff; }
                #loginTitle { font-size: 24px; font-weight: bold; color: #00ffff; margin-bottom: 20px; }
                
                #formInput { 
                    background-color: #330066; border: 2px solid #ff00ff; border-radius: 8px; 
                    padding: 12px; font-size: 14px; color: #ffffff;
                }
                #formInput:focus { border-color: #00ffff; }
                
                #primaryButton { 
                    background-color: #ff00ff; color: #000000; border: 2px solid #ff00ff; 
                    border-radius: 8px; font-weight: bold; font-size: 14px;
                }
                #primaryButton:hover { background-color: #ff66ff; }
                #secondaryButton { 
                    background-color: #330066; color: #ff00ff; border: 2px solid #ff00ff; 
                    border-radius: 8px; font-size: 14px;
                }
                #secondaryButton:hover { background-color: #4d0099; border-color: #00ffff; }
                #tertiaryButton { 
                    background-color: transparent; color: #00ffff; border: 2px solid #00ffff; 
                    border-radius: 8px; font-size: 14px;
                }
                #tertiaryButton:hover { background-color: #00ffff; color: #000000; }
                
                #gameStatus { 
                    background-color: #330066; border: 2px solid #ff00ff; border-radius: 12px; 
                    padding: 15px; font-size: 16px; font-weight: bold; text-align: center; color: #00ffff;
                }
                #timerLabel { font-size: 18px; font-weight: bold; color: #00ffff; }
                
                #boardButton { 
                    background-color: #330066; border: 3px solid #ff00ff; border-radius: 12px; 
                    font-size: 48px; font-weight: bold; color: #ffffff;
                }
                #boardButton:hover { 
                    background-color: #4d0099; border-color: #00ffff; 
                    box-shadow: 0 0 20px #00ffff;
                }
                #boardButton:pressed { background-color: #660099; }
                
                #scoreFrame, #controlsFrame, #replayControls { 
                    background-color: #1a0033; border: 2px solid #ff00ff; 
                }
                #sectionTitle { font-size: 16px; font-weight: bold; color: #00ffff; }
                #scoreText { font-size: 14px; color: #ff66ff; }
                
                #controlButton { 
                    background-color: #330066; border: 2px solid #ff00ff; border-radius: 8px; 
                    padding: 10px; font-size: 14px; color: #ff00ff;
                }
                #controlButton:hover { 
                    background-color: #4d0099; border-color: #00ffff; 
                    box-shadow: 0 0 10px #00ffff;
                }
                #controlButton:disabled { 
                    background-color: #1a0033; color: #666666; border-color: #330066; 
                }
                
                QTableWidget { 
                    background-color: #1a0033; alternate-background-color: #330066; 
                    gridline-color: #ff00ff; selection-background-color: #ff00ff; color: #ffffff;
                }
                QHeaderView::section { 
                    background-color: #330066; color: #00ffff; border: 2px solid #ff00ff; 
                    padding: 8px; font-weight: bold;
                }
                
                QProgressBar { 
                    background-color: #330066; border: 2px solid #ff00ff; border-radius: 8px; 
                    text-align: center; color: #ffffff;
                }
                QProgressBar::chunk { 
                    background-color: #00ffff; border-radius: 6px; 
                    box-shadow: 0 0 10px #00ffff;
                }
                
                QComboBox, QSlider, QSpinBox { 
                    background-color: #330066; border: 2px solid #ff00ff; border-radius: 6px; 
                    padding: 6px; color: #ffffff;
                }
                QComboBox:hover, QSlider:hover, QSpinBox:hover { 
                    border-color: #00ffff; box-shadow: 0 0 5px #00ffff; 
                }
                QGroupBox { 
                    font-weight: bold; border: 2px solid #ff00ff; border-radius: 8px; 
                    margin: 8px 0; padding-top: 10px; color: #00ffff;
                }
                QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 8px; }
                
                QRadioButton, QCheckBox { spacing: 8px; color: #ff66ff; }
                QRadioButton::indicator, QCheckBox::indicator { 
                    width: 16px; height: 16px; border: 2px solid #ff00ff; border-radius: 8px;
                }
                QRadioButton::indicator:checked, QCheckBox::indicator:checked { 
                    background-color: #00ffff; border-color: #00ffff;
                }
            )";
            break;
    } 
    setStyleSheet(styleSheet);
    updateButtonStyles();
}
void GUIInterface::updateButtonStyles() {
    // Update board buttons with player colors
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (boardButtons[i][j]->text() == "X") {
                boardButtons[i][j]->setStyleSheet(
                    boardButtons[i][j]->styleSheet() + 
                    QString("color: %1;").arg(getPlayerColor(Player::X).name())
                );
            } else if (boardButtons[i][j]->text() == "O") {
                boardButtons[i][j]->setStyleSheet(
                    boardButtons[i][j]->styleSheet() + 
                    QString("color: %1;").arg(getPlayerColor(Player::O).name())
                );
            }
        }
    }
}
void GUIInterface::animateButton(QWidget* widget) {
    if (!animationsEnabled) return;
    QPropertyAnimation *animation = new QPropertyAnimation(widget, "geometry");
    animation->setDuration(animationSpeed / 2);
    QRect geom = widget->geometry();
    animation->setStartValue(QRect(geom.x(), geom.y() + 5, geom.width(), geom.height()));
    animation->setEndValue(geom);
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void GUIInterface::addDropShadow(QWidget* widget) {
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15.0);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 160));
    widget->setGraphicsEffect(shadow);
}

void GUIInterface::fadeInWidget(QWidget* widget) {
    if (!animationsEnabled) return;
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);
    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(animationSpeed);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void GUIInterface::onThemeChanged(int id) {
    applyTheme(static_cast<Theme>(id));
}

void GUIInterface::onAnimationSpeedChanged(int value) {
    animationSpeed = value * 100;
}

void GUIInterface::onGameModeChanged() {
    undoButton->setEnabled(false);
    hintButton->setEnabled(gameModeTab->currentIndex() == 0);
}

void GUIInterface::onLoginButtonClicked() {
    QString username = usernameInput->text().trimmed();
    QString password = passwordInput->text();
    loginProgressBar->setVisible(true);
    QTimer::singleShot(1000, this, [=]() {
        if (userAuth.login(username.toStdString(), password.toStdString())) {
            loginStatusLabel->setText("Login successful!");
            switchToGameView();
        } else {
            loginStatusLabel->setText("Invalid credentials. Try again.");
        }
        loginProgressBar->setVisible(false);
    });
}

void GUIInterface::onRegisterButtonClicked() {
    QString username = usernameInput->text().trimmed();
    QString password = passwordInput->text();
    if (userAuth.registerUser(username.toStdString(), password.toStdString())) {
        loginStatusLabel->setText("Registration successful! You can now sign in.");
    } else {
        loginStatusLabel->setText("Username already exists.");
    }
}

void GUIInterface::onGuestPlayClicked() {
    userAuth.loginAsGuest();
    loginStatusLabel->setText("Logged in as Guest.");
    switchToGameView();
}

void GUIInterface::switchToLoginView() {
    mainStack->setCurrentWidget(loginWidget);
    gameTimer->stop();
    replayAutoTimer->stop();
    isGameInProgress = false;
    isReplayMode = false;
    updateNavigationButtons();
}

void GUIInterface::switchToGameView() {
    mainStack->setCurrentWidget(gameWidget);
    updateNavigationButtons();
}

void GUIInterface::switchToHistoryView() {
    mainStack->setCurrentWidget(historyWidget);
    loadUserGames();
    updateNavigationButtons();
}

void GUIInterface::switchToStatsView() {
    mainStack->setCurrentWidget(statsWidget);
    updateGameStats();
    updateNavigationButtons();
}

void GUIInterface::switchToSettingsView() {
    mainStack->setCurrentWidget(settingsWidget);
    updateNavigationButtons();
}

void GUIInterface::updateNavigationButtons() {
    gameNavButton->setChecked(mainStack->currentWidget() == gameWidget);
    historyNavButton->setChecked(mainStack->currentWidget() == historyWidget);
    statsNavButton->setChecked(mainStack->currentWidget() == statsWidget);
    settingsNavButton->setChecked(mainStack->currentWidget() == settingsWidget);
}