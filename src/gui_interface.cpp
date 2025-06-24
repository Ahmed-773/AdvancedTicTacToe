// gui_interface.cpp (UI Refresh Fix)
#include "gui_interface.h"
#include <QApplication>
#include <QSettings>
#include <QButtonGroup>
#include <QFormLayout>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDesktopServices>
#include <QFileInfo>
#include <QDebug>
#include <QHeaderView> 
#include <QTableWidgetItem>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup> // For more complex animations
#include <QPixmap>


// =====================================================================================
// --- Navigation and View Switching (SOLUTION AREA) ---
// =====================================================================================

void GUIInterface::switchToLoginView() { 
    mainStack->setCurrentWidget(loginWidget); 
    navigationFrame->hide(); 
}

void GUIInterface::switchToGameView() { 
    mainStack->setCurrentWidget(gameWidget); 
    navigationFrame->show(); 
    gameNavButton->setChecked(true); 
}

void GUIInterface::switchToHistoryView() { 
    // SOLUTION: Add this line to refresh the history table every time the view is shown.
    loadUserGames(); 
    
    mainStack->setCurrentWidget(historyWidget); 
    navigationFrame->show(); 
    historyNavButton->setChecked(true); 
}

void GUIInterface::switchToStatsView() { 
    // SOLUTION: Add this line to refresh the stats labels every time the view is shown.
    updateGameStats(); 

    mainStack->setCurrentWidget(statsWidget); 
    navigationFrame->show(); 
    statsNavButton->setChecked(true); 
}

void GUIInterface::switchToSettingsView() { 
    mainStack->setCurrentWidget(settingsWidget); 
    navigationFrame->show(); 
    settingsNavButton->setChecked(true); 
}

// =====================================================================================
// --- The rest of the file is included below for completeness ---
// =====================================================================================


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
    gameTimer = new QTimer(this);
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
        qWarning() << "Could not load initial data from database: " << e.what();
    }
    aiEngine.setDifficulty(difficultyCombo->currentIndex());
    switchToLoginView();
}

GUIInterface::~GUIInterface() {
    saveSettings();
}

void GUIInterface::setupUI() {
    setWindowTitle("Advanced Tic Tac Toe");
    setMinimumSize(1100, 750);
    resize(1200, 800);
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setupNavigation();
    mainStack = new QStackedWidget();
    mainStack->setObjectName("mainStack");
    setupAuthentication();
    setupGameBoard();
    setupHistoryView();
    setupStatsView();
    setupSettingsView();
    mainLayout->addWidget(navigationFrame);
    mainLayout->addWidget(mainStack, 1);
}

void GUIInterface::setupNavigation() {
    navigationFrame = new QFrame();
    navigationFrame->setObjectName("navigationFrame");
    navigationFrame->setFixedWidth(200);
    QVBoxLayout *navLayout = new QVBoxLayout(navigationFrame);
    navLayout->setContentsMargins(0, 20, 0, 20);
    navLayout->setSpacing(5);
    
    // This line must come first.
    // QLabel *titleLabel = new QLabel("TicTacToe\nPro");
    // titleLabel->setObjectName("appTitle");
    // titleLabel->setAlignment(Qt::AlignCenter);

    // Create buttons with new icons
    gameNavButton = new QPushButton( " Play Game");
    gameNavButton->setIcon(QIcon(":/play.png")); // Assumes play.png is in your resources
    gameNavButton = new QPushButton( " Pause Game");
    gameNavButton->setIcon(QIcon(":/Play Game.png")); // Assumes Play Game.png is in your resources
    historyNavButton = new QPushButton(" Game History");
    historyNavButton->setIcon(QIcon(":/list.png")); // Assumes list.png
    statsNavButton = new QPushButton(" Statistics");
    statsNavButton->setIcon(QIcon(":/bar-chart.png")); // Assumes bar-chart.png
    settingsNavButton = new QPushButton(" Settings");
    settingsNavButton->setIcon(QIcon(":/settings.png")); // Assumes settings.png
    
    QButtonGroup* navGroup = new QButtonGroup(this);
    navGroup->setExclusive(true);
    QPushButton* navButtons[] = {gameNavButton, historyNavButton, statsNavButton, settingsNavButton};
    for(auto* button : navButtons) {
        button->setProperty("class", "navButton");
        button->setCheckable(true);
        button->setIconSize(QSize(20, 20)); // Set a nice size for the icons
        navGroup->addButton(button);
        navLayout->addWidget(button);
    }
    
    // navLayout->addWidget(titleLabel, 0, Qt::AlignTop);
    // navLayout->addSpacing(30);
    connect(gameNavButton, &QPushButton::clicked, this, &GUIInterface::switchToGameView);
    connect(historyNavButton, &QPushButton::clicked, this, &GUIInterface::onViewHistoryClicked);
    connect(statsNavButton, &QPushButton::clicked, this, &GUIInterface::onViewStatsClicked);
    connect(settingsNavButton, &QPushButton::clicked, this, &GUIInterface::switchToSettingsView);
    navLayout->addStretch();
    QPushButton *logoutButton = new QPushButton(" Logout");
    logoutButton->setProperty("class", "logoutButton");
    logoutButton->setIconSize(QSize(20,20));
    connect(logoutButton, &QPushButton::clicked, this, &GUIInterface::onLogoutButtonClicked);
    navLayout->addWidget(logoutButton);
}

void GUIInterface::setupAuthentication() {
    loginWidget = new QWidget();
    loginWidget->setObjectName("loginWidget");
    QHBoxLayout *loginMainLayout = new QHBoxLayout(loginWidget);
    loginMainLayout->setContentsMargins(0, 0, 0, 0);
    // --- Welcome Panel (Left Side) - REDESIGNED ---
    welcomeFrame = new QFrame();
    welcomeFrame->setObjectName("welcomeFrame");
    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomeFrame);
    welcomeLayout->setAlignment(Qt::AlignCenter);
    welcomeLayout->setSpacing(10); // Adjust spacing
    
    // Add Logo
    QLabel* logoLabel = new QLabel();
    QPixmap logoPixmap(":/logo.png"); // Assumes logo.png is in your resources
    logoLabel->setPixmap(logoPixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignCenter);

    // Add Main Title
    QLabel *mainTitleLabel = new QLabel("Advanced Tic Tac Toe");
    mainTitleLabel->setObjectName("mainTitleLabel"); // New object name for styling
    mainTitleLabel->setAlignment(Qt::AlignCenter);
    // Add Subtitle
    QLabel *subTitleLabel = new QLabel("Pro Edition");
    subTitleLabel->setObjectName("subTitleLabel"); // New object name for styling
    subTitleLabel->setAlignment(Qt::AlignCenter);
    
    // Add new widgets to the layout
    welcomeLayout->addStretch(); // Pushes content to the center
    welcomeLayout->addWidget(logoLabel);
    welcomeLayout->addWidget(mainTitleLabel);
    welcomeLayout->addWidget(subTitleLabel);
    welcomeLayout->addStretch(); // Pushes content to the center

    loginFrame = new QFrame();
    loginFrame->setObjectName("loginFormContainer");
    loginFrame->setMaximumWidth(400);
    QVBoxLayout *loginLayout = new QVBoxLayout(loginFrame);
    loginLayout->setContentsMargins(40, 40, 40, 40);
    loginLayout->setSpacing(20);
    QLabel *loginTitle = new QLabel("Sign In or Register");
    loginTitle->setObjectName("loginTitle");
    loginTitle->setAlignment(Qt::AlignCenter);
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setHorizontalSpacing(15);

    usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("Enter your username");
    passwordInput = new QLineEdit();
    passwordInput->setPlaceholderText("Enter your password");
    passwordInput->setEchoMode(QLineEdit::Password);
    // --- NEW: Create labels for the icons ---
    QLabel* userIconLabel = new QLabel();
    userIconLabel->setPixmap(QPixmap(":/users.png").scaled(20, 20, Qt::KeepAspectRatio));
    QLabel* passIconLabel = new QLabel();
    passIconLabel->setPixmap(QPixmap(":/lock.png").scaled(20, 20, Qt::KeepAspectRatio));
    
    formLayout->addRow(userIconLabel, usernameInput);
    formLayout->addRow(passIconLabel, passwordInput);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    loginButton = new QPushButton("Sign In");
    registerButton = new QPushButton("Register");
    buttonLayout->addWidget(registerButton);
    buttonLayout->addWidget(loginButton);
    guestButton = new QPushButton("Play as Guest");
    loginStatusLabel = new QLabel("");
    loginStatusLabel->setObjectName("errorLabel");
    loginLayout->addWidget(loginTitle);
    loginLayout->addLayout(formLayout);
    loginLayout->addLayout(buttonLayout);
    loginLayout->addWidget(guestButton);
    loginLayout->addStretch();
    loginLayout->addWidget(loginStatusLabel);
    loginMainLayout->addWidget(welcomeFrame, 1);
    loginMainLayout->addWidget(loginFrame);
    connect(loginButton, &QPushButton::clicked, this, &GUIInterface::onLoginButtonClicked);
    connect(registerButton, &QPushButton::clicked, this, &GUIInterface::onRegisterButtonClicked);
    connect(guestButton, &QPushButton::clicked, this, &GUIInterface::onGuestPlayClicked);
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
    timerLabel = new QLabel("Time: 00:00");
    timerLabel->setObjectName("scoreLabel");
    boardFrame = new QFrame();
    boardFrame->setFixedSize(450, 450);
    QGridLayout *boardLayout = new QGridLayout(boardFrame);
    boardLayout->setSpacing(10);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardButtons[i][j] = new QPushButton("");
            boardButtons[i][j]->setProperty("class", "game-cell");
            boardButtons[i][j]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            connect(boardButtons[i][j], &QPushButton::clicked, this, &GUIInterface::onCellClicked);
            boardButtons[i][j]->setProperty("row", i);
            boardButtons[i][j]->setProperty("col", j);
            boardLayout->addWidget(boardButtons[i][j], i, j);
        }
    }
    setupReplayControls();
    centerLayout->addWidget(statusLabel);
    centerLayout->addWidget(timerLabel);
    centerLayout->addWidget(boardFrame, 1, Qt::AlignCenter);
    centerLayout->addWidget(replayControlsFrame);
    setupScoreDisplay(leftLayout);
    setupGameModeControls(leftLayout);
    setupGameControls(leftLayout);
    // This button is only for exiting replay mode, so it starts invisible.
    backToGameButton = new QPushButton("Exit Replay");
    connect(backToGameButton, &QPushButton::clicked, this, &GUIInterface::onExitReplayClicked);
    backToGameButton->setVisible(false);
    leftLayout->addWidget(backToGameButton);
    leftLayout->addStretch();
    gameMainLayout->addWidget(leftPanel);
    gameMainLayout->addWidget(centerPanel, 1);
    mainStack->addWidget(gameWidget);
}

void GUIInterface::setupScoreDisplay(QVBoxLayout* layout) {
    scoreFrame = new QFrame();
    scoreFrame->setProperty("class", "groupBox");
    QVBoxLayout *scoreLayout = new QVBoxLayout(scoreFrame);
    scoreLayout->setContentsMargins(15, 15, 15, 15);
    scoreLayout->setSpacing(5);
    QLabel* scoreTitle = new QLabel("SCOREBOARD");
    scoreTitle->setObjectName("sectionTitle");
    playerXScoreLabel = new QLabel("Player (You): 0");
    playerOScoreLabel = new QLabel("Opponent: 0");
    streakLabel = new QLabel("Current Streak: 0");
    winRateLabel = new QLabel("Win Rate: 0%");
    scoreLayout->addWidget(scoreTitle, 0, Qt::AlignCenter);
    scoreLayout->addWidget(playerXScoreLabel);
    scoreLayout->addWidget(playerOScoreLabel);
    scoreLayout->addWidget(streakLabel);
    scoreLayout->addWidget(winRateLabel);
    layout->addWidget(scoreFrame);
    addDropShadow(scoreFrame);
}

void GUIInterface::setupGameModeControls(QVBoxLayout* layout) {
    gameModeTab = new QTabWidget();
    
    aiModeWidget = new QWidget();
    QFormLayout *aiLayout = new QFormLayout(aiModeWidget);
    difficultyCombo = new QComboBox();
    difficultyCombo->addItems({"Easy", "Medium", "Hard"});
    difficultyCombo->setCurrentIndex(1);
    aiLayout->addRow("Difficulty:", difficultyCombo);
    gameModeTab->addTab(aiModeWidget, "vs AI");
    
    pvpModeWidget = new QWidget();
    gameModeTab->addTab(pvpModeWidget, "vs Player");
    
    gameModeTab->setTabIcon(0, QIcon(":/AI.png")); // Icon for "vs AI" tab
    gameModeTab->setTabIcon(1, QIcon(":/people.png")); // Icon for "vs Player" tab

    layout->addWidget(gameModeTab);
    connect(gameModeTab, &QTabWidget::currentChanged, this, &GUIInterface::onGameModeChanged);
    connect(difficultyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        aiEngine.setDifficulty(index);
    });
}

void GUIInterface::setupGameControls(QVBoxLayout* layout) {
    controlsFrame = new QFrame();
    controlsFrame->setProperty("class", "groupBox");
    QVBoxLayout* controlsLayout = new QVBoxLayout(controlsFrame);
    newGameButton = new QPushButton("New Game");
    undoButton = new QPushButton("Undo");
    hintButton = new QPushButton("Hint");
    controlsLayout->addWidget(newGameButton);
    controlsLayout->addWidget(undoButton);
    controlsLayout->addWidget(hintButton);
    layout->addWidget(controlsFrame);
    connect(newGameButton, &QPushButton::clicked, this, &GUIInterface::onNewGameButtonClicked);
    connect(undoButton, &QPushButton::clicked, this, &GUIInterface::onUndoMoveClicked);
    connect(hintButton, &QPushButton::clicked, this, &GUIInterface::onHintClicked);
    addDropShadow(controlsFrame);
}

void GUIInterface::setupHistoryView() {
    historyWidget = new QWidget();
    QVBoxLayout *historyLayout = new QVBoxLayout(historyWidget);
    historyLayout->setContentsMargins(20,20,20,20);
    historySplitter = new QSplitter(Qt::Horizontal);
    QFrame* tableFrame = new QFrame;
    QVBoxLayout* tableLayout = new QVBoxLayout(tableFrame);
    QLabel* tableTitle = new QLabel("Game History");
    tableTitle->setObjectName("titleLabel");
    tableLayout->addWidget(tableTitle);
    gameHistoryTable = new QTableWidget();
    gameHistoryTable->setColumnCount(4);
    gameHistoryTable->setHorizontalHeaderLabels({"Date", "Opponent", "Result", "Moves"});
    gameHistoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    gameHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    gameHistoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableLayout->addWidget(gameHistoryTable);
    historyDetailsFrame = new QFrame;
    QVBoxLayout* detailsLayout = new QVBoxLayout(historyDetailsFrame);
    QLabel* detailsTitle = new QLabel("Game Details");
    detailsTitle->setObjectName("titleLabel");
    gameDetailsText = new QTextEdit();
    gameDetailsText->setReadOnly(true);
    detailsLayout->addWidget(detailsTitle);
    detailsLayout->addWidget(gameDetailsText);
    historySplitter->addWidget(tableFrame);
    historySplitter->addWidget(historyDetailsFrame);
    historySplitter->setSizes({600, 300});
    exportHistoryButton = new QPushButton("Export as CSV");
    historyLayout->addWidget(historySplitter, 1);
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(exportHistoryButton);
    historyLayout->addLayout(btnLayout);
    connect(exportHistoryButton, &QPushButton::clicked, this, &GUIInterface::exportGameHistory);
    connect(gameHistoryTable, &QTableWidget::itemDoubleClicked, this, &GUIInterface::onGameHistoryItemClicked);
    mainStack->addWidget(historyWidget);
}

void GUIInterface::setupStatsView() {
    statsWidget = new QWidget();
    statsScrollArea = new QScrollArea();
    statsScrollArea->setWidgetResizable(true);
    statsFrame = new QFrame();
    statsScrollArea->setWidget(statsFrame);
    
    QVBoxLayout* statsMainLayout = new QVBoxLayout(statsWidget);
    QGridLayout* statsLayout = new QGridLayout(statsFrame);
    
    totalGamesLabel = new QLabel("Total Games: 0");
    winRateStatsLabel = new QLabel("Win Rate: 0%");
    averageGameTimeLabel = new QLabel("Avg. Game Time: 0s");
    longestStreakLabel = new QLabel("Longest Streak: 0");
    favoriteOpponentLabel = new QLabel("Favorite Mode: N/A");

    statsLayout->addWidget(totalGamesLabel, 0, 0);
    statsLayout->addWidget(winRateStatsLabel, 0, 1);
    statsLayout->addWidget(averageGameTimeLabel, 1, 0);
    statsLayout->addWidget(longestStreakLabel, 1, 1);
    statsLayout->addWidget(favoriteOpponentLabel, 2, 0);

    QLabel* statsTitle = new QLabel("Player Statistics");
    statsTitle->setObjectName("titleLabel");
    statsMainLayout->addWidget(statsTitle);
    statsMainLayout->addWidget(statsScrollArea);
    
    mainStack->addWidget(statsWidget);
}

void GUIInterface::setupSettingsView() {
    settingsWidget = new QWidget();
    QVBoxLayout* rootLayout = new QVBoxLayout(settingsWidget);
    rootLayout->setAlignment(Qt::AlignCenter);

    QFormLayout *settingsLayout = new QFormLayout();
    settingsLayout->setContentsMargins(20,20,20,20);
    settingsLayout->setSpacing(15);
    
    QLabel* settingsTitle = new QLabel("Settings");
    settingsTitle->setObjectName("titleLabel");
    
    themeGroup = new QButtonGroup(this);
    QRadioButton* darkRadio = new QRadioButton("Dark");
    QRadioButton* lightRadio = new QRadioButton("Light");
    QRadioButton* neonRadio = new QRadioButton("Neon");
    themeGroup->addButton(darkRadio, DARK);
    themeGroup->addButton(lightRadio, LIGHT);
    themeGroup->addButton(neonRadio, NEON);
    QHBoxLayout* themeLayout = new QHBoxLayout;
    themeLayout->addWidget(darkRadio);
    themeLayout->addWidget(lightRadio);
    themeLayout->addWidget(neonRadio);
    settingsLayout->addRow("Theme:", themeLayout);
    
    animationsEnabledCheck = new QCheckBox("Enable Animations");
    settingsLayout->addRow(animationsEnabledCheck);
    
    animationSpeedSlider = new QSlider(Qt::Horizontal);
    animationSpeedSlider->setRange(100, 1000);
    settingsLayout->addRow("Animation Speed:", animationSpeedSlider);
    
    QGroupBox* settingsBox = new QGroupBox();
    settingsBox->setLayout(settingsLayout);
    settingsBox->setFixedWidth(500);

    rootLayout->addWidget(settingsTitle);
    rootLayout->addWidget(settingsBox);
    rootLayout->addStretch();

    mainStack->addWidget(settingsWidget);
    
    connect(themeGroup, QOverload<int>::of(&QButtonGroup::idClicked), this, &GUIInterface::onThemeChanged);
    connect(animationsEnabledCheck, &QCheckBox::toggled, this, [this](bool checked){ animationsEnabled = checked; });
    connect(animationSpeedSlider, &QSlider::valueChanged, this, &GUIInterface::onAnimationSpeedChanged);
}

void GUIInterface::setupReplayControls() {
    replayControlsFrame = new QFrame();
    QHBoxLayout *replayLayout = new QHBoxLayout(replayControlsFrame);
    
    replayStartButton = new QPushButton("⏮️");
    replayStartButton->setToolTip("Go to Start"); // Add tooltip
    replayPrevButton = new QPushButton("⏪");
    replayPrevButton->setToolTip("Previous Move"); // Add tooltip
    replayAutoButton = new QPushButton("▶️");
    replayAutoButton->setToolTip("Auto-Play / Pause"); // Add tooltip
    replayNextButton = new QPushButton("⏩");
    replayNextButton->setToolTip("Next Move"); // Add tooltip
    
    replayPositionLabel = new QLabel("Move: 0 / 0");
    replayLayout->addWidget(replayStartButton);
    replayLayout->addWidget(replayPrevButton);
    replayLayout->addWidget(replayAutoButton);
    replayLayout->addWidget(replayNextButton);
    replayLayout->addStretch();
    replayLayout->addWidget(replayPositionLabel);
    connect(replayStartButton, &QPushButton::clicked, this, &GUIInterface::onReplayStartClicked);
    connect(replayPrevButton, &QPushButton::clicked, this, &GUIInterface::onReplayPrevClicked);
    connect(replayNextButton, &QPushButton::clicked, this, &GUIInterface::onReplayNextClicked);
    connect(replayAutoButton, &QPushButton::clicked, this, &GUIInterface::onReplayAutoPlay);
    replayControlsFrame->setVisible(false);
}

void GUIInterface::setupReplayControls(bool visible) {
    if(replayControlsFrame) {
        replayControlsFrame->setVisible(visible);
    }
}

void GUIInterface::applyTheme(Theme theme) {
    currentTheme = theme;
    QString styleSheet = R"(
        QWidget { font-family: 'Segoe UI', sans-serif; }
        QFrame#navigationFrame { background-color: #2c3e50; }
        QStackedWidget#mainStack, QWidget#loginWidget {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #34495e, stop:1 #2c3e50);}
        QLabel { color: #ecf0f1; font-size: 14px; }
        QLabel#appTitle { color: #ffffff; font-size: 24px; font-weight: bold; }
        QLabel#welcomeTitle, QLabel#loginTitle, QLabel#titleLabel { font-size: 32px; font-weight: bold; color: #ffffff; }
        QLabel#featuresList { color: #bdc3c7; }
        QLabel#statusLabel { font-size: 22px; font-weight: bold; color: #f1c40f; padding: 10px; }
        QLabel#scoreLabel, QLabel#sectionTitle { font-weight: bold; color: #3498db; }
        QLabel#errorLabel { color: #e74c3c; font-weight: bold; }
        QPushButton { background-color: #3498db; color: white; border: none; padding: 10px; border-radius: 5px; font-weight: bold; }
        QPushButton:hover { background-color: #5dade2; }
        QPushButton:pressed { background-color: #2980b9; }
        QPushButton:disabled { background-color: #95a5a6; }
        QPushButton[class='navButton'] { background-color: transparent; text-align: left; padding-left: 20px; color: #bdc3c7; border-left: 3px solid transparent; border-radius: 0; }
        QPushButton[class='navButton']:hover { background-color: #34495e; color: #ffffff; }
        QPushButton[class='navButton']:checked { background-color: #2c3e50; color: #ffffff; border-left: 3px solid #3498db; }
        QPushButton[class='logoutButton'] { background-color: #c0392b; }
        QPushButton[class='logoutButton']:hover { background-color: #e74c3c; }
        QPushButton[class='game-cell'] { background-color: rgba(0, 0, 0, 0.2); font-size: 48px; font-weight: bold; }
        QPushButton[class='game-cell']:hover { background-color: rgba(0, 0, 0, 0.4); }
        QLineEdit, QComboBox { padding: 8px; border: 1px solid #2c3e50; border-radius: 4px; background-color: #566573; color: white; }
        QLineEdit:focus { border-color: #3498db; }
        QFrame#loginFormContainer, QFrame[class='groupBox'], QGroupBox { background-color: rgba(0,0,0,0.2); border-radius: 15px; }
        QFrame#welcomeFrame { background-color: transparent; }
        QTableWidget { background-color: #566573; color: #ecf0f1; border: none; gridline-color: #34495e; selection-background-color: #3498db; }
        QHeaderView::section { background-color: #2c3e50; color: #ffffff; padding: 8px; border: none; font-weight: bold; }
        QLabel#mainTitleLabel { font-size: 36px; font-weight: bold; color: #ffffff; }
        QLabel#subTitleLabel { font-size: 20px; font-style: italic; color: #bdc3c7; }
    )";
    
    if (theme == LIGHT) { /* Light theme QSS here */ }
    if (theme == NEON) { /* Neon theme QSS here */ }

    qApp->setStyleSheet(styleSheet);
    updateButtonStyles();
}

void GUIInterface::updateButtonStyles() {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            Player cell = gameLogic.getCell(i, j);
            if (cell != Player::NONE) {
                boardButtons[i][j]->setStyleSheet(QString("color: %1;").arg(getPlayerColor(cell).name()));
            } else {
                boardButtons[i][j]->setStyleSheet("");
            }
        }
    }
}

void GUIInterface::onLoginButtonClicked() {
    if (userAuth.loginUser(usernameInput->text().toStdString(), passwordInput->text().toStdString())) {
        showNotification("Login Successful!", "success");
        // Refresh data on login
        updateGameStats();
        loadUserGames();
        switchToGameView();
    } else {
        showNotification("Invalid username or password.", "error");
        // --- UI CALL: Animating the login form to show an error ---
        animateButton(loginFrame);
    }
}

void GUIInterface::onRegisterButtonClicked() {
    if (userAuth.registerUser(usernameInput->text().toStdString(), passwordInput->text().toStdString())) {
        dbManager.saveUsers(userAuth.getUsers());
        showNotification("Registration Successful! Please log in.", "success");
        usernameInput->clear();
        passwordInput->clear();
        usernameInput->setFocus();
    } else {
        showNotification("Username already exists or is invalid.", "error");
    }
}

void GUIInterface::onGuestPlayClicked() {
    userAuth.logoutUser();
    showNotification("Playing as Guest. Progress will not be saved.", "info");
    updateGameStats();
    loadUserGames();
    switchToGameView();
}

void GUIInterface::onLogoutButtonClicked() {
    userAuth.logoutUser();
    showNotification("You have been logged out.", "info");
    switchToLoginView();
}

void GUIInterface::onCellClicked() {
    if (!isGameInProgress || isReplayMode) return;

    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    int row = button->property("row").toInt();
    int col = button->property("col").toInt();

    if (gameLogic.makeMove(row, col)) {
        Player movedPlayer = (gameLogic.getCurrentPlayer() == Player::X) ? Player::O : Player::X;
        updateBoard();
        animateCellPlacement(row, col, movedPlayer);
        
        GameResult result = gameLogic.checkGameResult();
        if (result != GameResult::IN_PROGRESS) {
            handleGameOver(result);
        } else {
            statusLabel->setText(getPlayerName(gameLogic.getCurrentPlayer()) + "'s Turn");
            if (gameModeTab->currentIndex() == 0 && gameLogic.getCurrentPlayer() == Player::O) {
                QTimer::singleShot(animationSpeed + 50, this, &GUIInterface::makeAIMove);
            }
        }
    }
}

void GUIInterface::onNewGameButtonClicked() {
    if (isGameInProgress && QMessageBox::question(this, "New Game", "Abandon current game?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }
    
    isGameInProgress = true;
    isReplayMode = false;
    gameTimeSeconds = 0;
    
    gameLogic.resetBoard();
    updateBoard();
    statusLabel->setText(getPlayerName(gameLogic.getCurrentPlayer()) + "'s Turn");
    updateTimer();
    gameTimer->start(1000);
    
    setupReplayControls(false);
    for(int r=0; r<3; ++r) for(int c=0; c<3; ++c) boardButtons[r][c]->setEnabled(true);
    undoButton->setEnabled(true);
    hintButton->setEnabled(true);
    
    showNotification("New game started!", "info");
}

void GUIInterface::onUndoMoveClicked() {
    if (!isGameInProgress || isReplayMode || gameLogic.getMoveHistory().empty()) return;
    
    gameLogic.undoLastMove();
    if (gameModeTab->currentIndex() == 0 && !gameLogic.getMoveHistory().empty()) {
        gameLogic.undoLastMove();
    }
    updateBoard();
}

void GUIInterface::onHintClicked() {
    if (!isGameInProgress || isReplayMode) return;
    Move hint = aiEngine.getBestMove(gameLogic);
    if(hint.row != -1){
        animateButton(boardButtons[hint.row][hint.col]);
    }
}

void GUIInterface::onGameModeChanged() {
    onNewGameButtonClicked();
}

void GUIInterface::onThemeChanged(int id) {
    applyTheme(static_cast<Theme>(id));
}

void GUIInterface::onAnimationSpeedChanged(int value) {
    animationSpeed = value;
}

void GUIInterface::onGameHistoryItemClicked(QTableWidgetItem *item) {
    if (!item) return;

    int row = item->row();
    QVariant data = gameHistoryTable->item(row, 0)->data(Qt::UserRole);
    if (!data.isValid()) return;

    std::string gameId = data.toString().toStdString();
    GameState game = gameHistory.getGameById(gameId);

    if (!game.gameId.empty()) {
        QString details;
        details += "<b>Game ID:</b> " + QString::fromStdString(game.gameId) + "<br>";
        details += "<b>Date:</b> " + QString::fromStdString(game.timestamp) + "<br>";
        details += "<b>Opponent:</b> " + QString::fromStdString(game.player2Id) + "<br>";
        details += "<b>Result:</b> " + formatGameResult(game.result) + "<br>";
        
        // This call now matches the GameState struct in game_logic.h
        details += "<b>Duration:</b> " + QString::number(game.durationSeconds) + " seconds<br><br>";
        details += "<b>Move List:</b><br>";

        int moveNumber = 1;
        for (const auto& move : game.moveHistory) {
            QString player = (moveNumber % 2 != 0) ? "X" : "O";
            details += QString("%1. %2 to (%3, %4)<br>").arg(moveNumber).arg(player).arg(move.row).arg(move.col);
            moveNumber++;
        }

        gameDetailsText->setHtml(details);
        // Now, switch to the replay mode for the selected game.
        displayGameForReplay(game);
    }
}

void GUIInterface::handleGameOver(GameResult result) {
    isGameInProgress = false;
    gameTimer->stop();
    statusLabel->setText(formatGameResult(result));
    
    if (result == GameResult::X_WINS || result == GameResult::O_WINS) {
        highlightWinningCells(gameLogic.findWinningCombination());
    }
    animateGameOver(result);
    for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) boardButtons[i][j]->setEnabled(false);
    undoButton->setEnabled(false);
    hintButton->setEnabled(false);

    if (userAuth.isLoggedIn()) {
        bool vsAI = (gameModeTab->currentIndex() == 0);
        
        // --- UPDATE: Pass game time and opponent type to the stats update function ---
        userAuth.updateUserStats(result, gameTimeSeconds, vsAI);
        
        dbManager.saveUsers(userAuth.getUsers());
        std::string opponentId = vsAI ? "AI" : "Player2";

        gameHistory.saveGame(userAuth.getCurrentUser()->userId, opponentId, 
                             vsAI, gameLogic.getMoveHistory(), result);

        dbManager.saveGameHistory(gameHistory.getAllGames());
        
        // Refresh the UI with the new stats
        updateScoreDisplay(); 
        updateGameStats();
    }
}

void GUIInterface::makeAIMove() {
    Move aiMove = aiEngine.getBestMove(gameLogic);
    if(gameLogic.makeMove(aiMove.row, aiMove.col)) {
        updateBoard();
        animateCellPlacement(aiMove.row, aiMove.col, Player::O);
        
        GameResult result = gameLogic.checkGameResult();
        if (result != GameResult::IN_PROGRESS) {
            handleGameOver(result);
        } else {
            statusLabel->setText(getPlayerName(gameLogic.getCurrentPlayer()) + "'s Turn");
        }
    }
}

void GUIInterface::animateCellPlacement(int row, int col, Player player) {
    if (!animationsEnabled) {
        updateBoard();
        return;
    }

    QPushButton* button = boardButtons[row][col];
    if (!button) return; // Safety check

    // If an effect already exists, it will be managed by its parent (the button),
    // but we must set the pointer to null to avoid using a stale effect.
    if (button->graphicsEffect()) {
        button->setGraphicsEffect(nullptr);
    }

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(button);
    button->setGraphicsEffect(effect);

    // Parent the animation to 'this' (the GUIInterface) to manage its lifetime
    QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity", this);
    anim->setDuration(animationSpeed);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::InOutQuad);

    // When the animation is finished, clean up the effect.
    // By providing 'button' as the context object, Qt guarantees this lambda
    // will NOT be called if the button is destroyed before the animation finishes.
    connect(anim, &QPropertyAnimation::finished, button, [button]() {
        if (button) {
            button->setGraphicsEffect(nullptr);
        }
    });

    // The animation will delete itself once it's done.
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

// Provides a subtle "shake" or "wobble" animation for a widget.
// Useful for drawing attention or providing feedback on an invalid action.
void GUIInterface::animateButton(QWidget* widget) {
    if (!animationsEnabled || !widget) return;

    QPropertyAnimation* anim = new QPropertyAnimation(widget, "pos", this);
    QPoint originalPos = widget->pos();
    anim->setDuration(300);
    anim->setEasingCurve(QEasingCurve::InOutSine);

    // Create the wobble effect
    anim->setKeyValueAt(0.0, originalPos);
    anim->setKeyValueAt(0.1, originalPos + QPoint(5, 0));
    anim->setKeyValueAt(0.2, originalPos - QPoint(5, 0));
    anim->setKeyValueAt(0.3, originalPos + QPoint(5, 0));
    anim->setKeyValueAt(0.4, originalPos - QPoint(5, 0));
    anim->setKeyValueAt(0.5, originalPos);

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

// Provides a flourish when the game ends to make the result more impactful.
void GUIInterface::animateGameOver(GameResult result) {
    if (!animationsEnabled) return;

    // We can animate the status label to make it pop.
    QPropertyAnimation* anim = new QPropertyAnimation(statusLabel, "geometry", this);
    QRect startGeom = statusLabel->geometry();
    QRect endGeom = startGeom.adjusted(-10, -5, 10, 5); // Make it slightly larger

    anim->setDuration(600);
    anim->setEasingCurve(QEasingCurve::OutElastic); // A bouncy effect

    // Animate from original size, to larger, back to original
    anim->setKeyValueAt(0.0, startGeom);
    anim->setKeyValueAt(0.5, endGeom);
    anim->setKeyValueAt(1.0, startGeom);

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void GUIInterface::onGameTimerUpdate() { if(isGameInProgress) gameTimeSeconds++; updateTimer(); }

void GUIInterface::highlightWinningCells(const std::vector<Move>& cells) { 
    for(const auto& move : cells) {
        boardButtons[move.row][move.col]->setStyleSheet("background-color: #f1c40f;");
    }
}

void GUIInterface::resetBoardHighlights() { 
    for(int r=0; r<3; ++r) for(int c=0; c<3; ++c) {
        boardButtons[r][c]->setStyleSheet(""); 
    }
    updateButtonStyles(); 
}

void GUIInterface::updateBoard(bool isReplay) { 
    resetBoardHighlights(); 
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardButtons[i][j]->setText(getPlayerName(gameLogic.getCell(i, j)));
        }
    }
    updateButtonStyles(); 
}

void GUIInterface::updateScoreDisplay() { 
    if(userAuth.isLoggedIn()) { 
        const UserProfile* u = userAuth.getCurrentUser();
        // Update all labels with the current user's stats
        playerXScoreLabel->setText(QString("Player (You): %1").arg(u->gamesWon)); 
        playerOScoreLabel->setText(QString("Opponent: %1").arg(u->gamesLost));
        
        // This is a placeholder for streak logic, which can be complex
        streakLabel->setText("Current Streak: 0"); 
        
        if (u->gamesPlayed > u->gamesTied) {
            double winRate = (static_cast<double>(u->gamesWon) / (u->gamesPlayed - u->gamesTied)) * 100.0;
            winRateLabel->setText(QString("Win Rate: %1%").arg(winRate, 0, 'f', 1));
        } else {
            winRateLabel->setText("Win Rate: N/A");
        }
    } else {
        // Reset all labels for guest mode
        playerXScoreLabel->setText("Player (You): 0");
        playerOScoreLabel->setText("Opponent: 0");
        streakLabel->setText("Current Streak: 0");
        winRateLabel->setText("Win Rate: 0%");
    }
}

void GUIInterface::updateGameStats() {
    // Reset to default state
    totalGamesLabel->setText("Total Games: 0");
    winRateStatsLabel->setText("Win Rate: N/A");
    averageGameTimeLabel->setText("Avg. Game Time: N/A");
    longestStreakLabel->setText("Longest Streak: 0");
    favoriteOpponentLabel->setText("Favorite Mode: N/A");
    
    if(userAuth.isLoggedIn()){
        const UserProfile* user = userAuth.getCurrentUser();
        totalGamesLabel->setText(QString("Total Games: %1").arg(user->gamesPlayed));
        longestStreakLabel->setText(QString("Longest Streak: %1").arg(user->longestWinStreak));
        
        // Calculate Win Rate
        if (user->gamesPlayed > user->gamesTied) {
            double winRate = (static_cast<double>(user->gamesWon) / (user->gamesPlayed - user->gamesTied)) * 100.0;
            winRateStatsLabel->setText(QString("Win Rate: %1%").arg(winRate, 0, 'f', 1));
        }
        
        // Calculate Average Game Time
        if (user->gamesPlayed > 0) {
            long avgTime = user->totalGameTimeSeconds / user->gamesPlayed;
            averageGameTimeLabel->setText(QString("Avg. Game Time: %1s").arg(avgTime));
        }

        // Determine Favorite Mode
        if (user->aiGamesPlayed > user->pvpGamesPlayed) {
            favoriteOpponentLabel->setText("Favorite Mode: vs AI");
        } else if (user->pvpGamesPlayed > user->aiGamesPlayed) {
            favoriteOpponentLabel->setText("Favorite Mode: vs Player");
        } else {
            favoriteOpponentLabel->setText("Favorite Mode: N/A");
        }
    }
}

void GUIInterface::loadUserGames() {
    gameHistoryTable->setRowCount(0);
    if (!userAuth.isLoggedIn()) return;
    
    std::vector<GameState> userGames = gameHistory.getUserGames(userAuth.getCurrentUser()->userId);
    
    for (const auto& game : userGames) {
        int row = gameHistoryTable->rowCount();
        gameHistoryTable->insertRow(row);
        
        QTableWidgetItem* dateItem = new QTableWidgetItem(QString::fromStdString(game.timestamp));
        dateItem->setData(Qt::UserRole, QVariant(QString::fromStdString(game.gameId)));
        
        gameHistoryTable->setItem(row, 0, dateItem);
        gameHistoryTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(game.player2Id)));
        gameHistoryTable->setItem(row, 2, new QTableWidgetItem(formatGameResult(game.result)));
        gameHistoryTable->setItem(row, 3, new QTableWidgetItem(QString::number(game.moveHistory.size())));
    }
}

void GUIInterface::updateTimer() { timerLabel->setText(formatTime(gameTimeSeconds)); }
QString GUIInterface::formatTime(int totalSeconds) { return QString("%1:%2").arg(totalSeconds / 60, 2, 10, QChar('0')).arg(totalSeconds % 60, 2, 10, QChar('0')); }
QString GUIInterface::formatGameResult(GameResult result) { if (result == GameResult::X_WINS) return "You Won!"; if (result == GameResult::O_WINS) return "Opponent Won"; if (result == GameResult::DRAW) return "It's a Draw"; return "In Progress"; }
QString GUIInterface::getPlayerName(Player player) { if (player == Player::X) return "X"; if (player == Player::O) return "O"; return ""; }
QColor GUIInterface::getPlayerColor(Player player) { if (player == Player::X) return QColor("#3498DB"); if (player == Player::O) return QColor("#E74C3C"); return Qt::white; }
void GUIInterface::showNotification(const QString& message, const QString& type) { QMessageBox msgBox(this); msgBox.setText(message); msgBox.setIcon(type == "error" ? QMessageBox::Critical : QMessageBox::Information); msgBox.setWindowTitle(type == "error" ? "Error" : "Notification"); msgBox.exec(); }
// This slot is called by the "Game History" button.
void GUIInterface::onViewHistoryClicked() { switchToHistoryView();}
// This slot is called by the "Statistics" button.
void GUIInterface::onViewStatsClicked() { switchToStatsView();}
void GUIInterface::onBackToGameClicked() { switchToGameView(); }

// This slot is called when the "Next Move" (⏩) button is clicked.
void GUIInterface::onReplayNextClicked() {
    if (!isReplayMode || replayMoveIndex >= replayHistory.size()) {
        if(replayAutoTimer->isActive()) {
            replayAutoTimer->stop();
            replayAutoButton->setText("▶️");
        }
        return;
    }
    const Move& nextMove = replayHistory[replayMoveIndex];
    gameLogic.makeMove(nextMove.row, nextMove.col);
    replayMoveIndex++;
    updateBoard(true);
    updateReplayControls();
}

// This slot is called when the "Previous Move" (⏪) button is clicked.
void GUIInterface::onReplayPrevClicked() {
    if (!isReplayMode || replayMoveIndex <= 0) return;
    replayMoveIndex--;
    gameLogic.resetBoard();
    for (int i = 0; i < replayMoveIndex; ++i) {
        gameLogic.makeMove(replayHistory[i].row, replayHistory[i].col);
    }
    updateBoard(true);
    updateReplayControls();
}

// This slot is called when the "Go to Start" (⏮️) button is clicked.
void GUIInterface::onReplayStartClicked() {
    if (!isReplayMode) return;
    replayMoveIndex = 0;
    gameLogic.resetBoard();
    updateBoard(true);
    updateReplayControls();
}

// This slot is called when the "Auto-Play/Pause" (▶️/⏸️) button is clicked.
void GUIInterface::onReplayAutoPlay() {
    if (!isReplayMode) return;
    if (replayAutoTimer->isActive()) {
        replayAutoTimer->stop();
        replayAutoButton->setText("▶️");
    } else {
        if (replayMoveIndex >= replayHistory.size()) {
            onReplayStartClicked();
        }
        replayAutoTimer->start(1200);
        replayAutoButton->setText("⏸️");
    }
}

// This helper function updates the replay UI elements.
void GUIInterface::updateReplayControls() {
    if (!isReplayMode) return;
    replayPositionLabel->setText(QString("Move: %1 / %2").arg(replayMoveIndex).arg(replayHistory.size()));
    replayPrevButton->setEnabled(replayMoveIndex > 0);
    replayStartButton->setEnabled(replayMoveIndex > 0);
    replayNextButton->setEnabled(replayMoveIndex < replayHistory.size());
}

// Applies a standard drop shadow effect to a widget for a sense of depth.
void GUIInterface::addDropShadow(QWidget* widget) {
    if (!widget) return;
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 5);
    widget->setGraphicsEffect(shadow);
}

// Applies a colored glow effect, useful for highlighting or hover effects.
void GUIInterface::addGlowEffect(QWidget* widget, const QColor& color) {
    if (!widget) return;
    QGraphicsDropShadowEffect* glow = new QGraphicsDropShadowEffect(this);
    glow->setBlurRadius(25);
    glow->setColor(color);
    glow->setOffset(0, 0);
    widget->setGraphicsEffect(glow);
}

// Smoothly fades a widget into view.
void GUIInterface::fadeInWidget(QWidget* widget) {
    if (!animationsEnabled || !widget) {
        widget->setVisible(true);
        return;
    }

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);
    widget->setVisible(true);

    QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity", this);
    anim->setDuration(animationSpeed);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void GUIInterface::displayGameForReplay(const GameState& game) {
    isReplayMode = true;
    gameLogic.resetBoard();
    replayHistory = game.moveHistory;
    replayMoveIndex = 0;
    
    updateBoard(true); // Initial empty board state for replay
    updateReplayControls();
    
    statusLabel->setText("Replay: " + QString::fromStdString(game.timestamp));
    
    // Show/Hide the correct UI elements
    setupReplayControls(true);  // Show replay controls
    timerLabel->setVisible(false);
    scoreFrame->setVisible(false);
    gameModeTab->setVisible(false);
    controlsFrame->setVisible(false); // Hide New Game, Undo, Hint buttons
    
    // Add a temporary button to exit the replay
    backToGameButton->setVisible(true);

    switchToGameView();
}

void GUIInterface::switchToGameSetupView() {
    isGameInProgress = true; // Set the flag to allow starting a game
    isReplayMode = false;
    gameTimeSeconds = 0;
    
    // Reset the board logic and UI elements
    gameLogic.resetBoard();
    updateBoard(); // Redraws the empty board
    statusLabel->setText("Choose a mode and start playing!");
    updateTimer();
    gameTimer->start(1000);
    
    // Ensure all normal game controls are visible and replay controls are hidden
    setupReplayControls(false);
    timerLabel->setVisible(true);
    scoreFrame->setVisible(true);
    gameModeTab->setVisible(true);
    controlsFrame->setVisible(true);
    backToGameButton->setVisible(false);
    
    // Enable all buttons for the new game
    for(int r=0; r<3; ++r) for(int c=0; c<3; ++c) {
        boardButtons[r][c]->setEnabled(true);
    }
    undoButton->setEnabled(true);
    hintButton->setEnabled(true);
    
    // Finally, switch to the main game screen
    switchToGameView();
}

// New slot to handle exiting the replay mode cleanly
void GUIInterface::onExitReplayClicked() {
    isReplayMode = false;

    // Restore the normal game view UI
    setupReplayControls(false); // Hide replay controls
    timerLabel->setVisible(true);
    scoreFrame->setVisible(true);
    gameModeTab->setVisible(true);
    controlsFrame->setVisible(true);
    backToGameButton->setVisible(false);

    // Go back to the game setup screen
    switchToGameSetupView();
}

void GUIInterface::exportGameHistory() {}
void GUIInterface::loadSettings() {}
void GUIInterface::saveSettings() {}
void GUIInterface::applySettings() {}
void GUIInterface::setLoading(QPushButton* button, bool loading) { button->setEnabled(!loading); }