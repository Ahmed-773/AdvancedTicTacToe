//---------------------------------------------------------------------------------------
// gui_interface.cpp - Part 1 of 4: The Foundation (Setup and Styling)
#include "gui_interface.h"

// All necessary Qt includes
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
#include <QIcon>
#include <QTimer>
#include <QHeaderView>

// Define the time limit for each turn in seconds.
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

    aiEngine.setDifficulty(3); // Default difficulty
    switchToLoginView();
}

GUIInterface::~GUIInterface() {
    saveSettings();
}

// --- SETUP FUNCTIONS (Creation and Layout of all UI elements) ---

void GUIInterface::setupUI() {
    setWindowTitle("Advanced Tic Tac Toe - Pro Edition");
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
    
    // Create each screen view
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
    
    QVBoxLayout *navLayout = new QVBoxLayout(navigationFrame);
    navLayout->setContentsMargins(10, 20, 10, 20);
    navLayout->setSpacing(10);
    
    QLabel *titleLabel = new QLabel("TicTacToe\nPro");
    titleLabel->setObjectName("appTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    gameNavButton = new QPushButton("🎮 Game");
    historyNavButton = new QPushButton("📈 History");
    statsNavButton = new QPushButton("📊 Statistics");
    settingsNavButton = new QPushButton("⚙️ Settings");
    
    QButtonGroup* navGroup = new QButtonGroup(this);
    navGroup->setExclusive(true);
    QPushButton* navButtons[] = {gameNavButton, historyNavButton, statsNavButton, settingsNavButton};
    for(auto* button : navButtons) {
        button->setObjectName("navButton");
        button->setCheckable(true);
        navGroup->addButton(button);
        navLayout->addWidget(button);
    }
    
    connect(gameNavButton, &QPushButton::clicked, this, &GUIInterface::switchToGameView);
    connect(historyNavButton, &QPushButton::clicked, this, &GUIInterface::switchToHistoryView);
    connect(statsNavButton, &QPushButton::clicked, this, &GUIInterface::switchToStatsView);
    connect(settingsNavButton, &QPushButton::clicked, this, &GUIInterface::switchToSettingsView);
    
    navLayout->addWidget(titleLabel, 0, Qt::AlignTop);
    navLayout->addSpacing(30);
    navLayout->addStretch();
    
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
    
    welcomeFrame = new QFrame();
    welcomeFrame->setObjectName("welcomeFrame");
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

    loginFrame = new QFrame();
    loginFrame->setObjectName("loginFrame");
    loginFrame->setMaximumWidth(400);
    QVBoxLayout *loginLayout = new QVBoxLayout(loginFrame);
    loginLayout->setContentsMargins(40, 40, 40, 40);
    loginLayout->setSpacing(20);
    
    QLabel *loginTitle = new QLabel("Sign In or Register");
    loginTitle->setObjectName("loginTitle");
    loginTitle->setAlignment(Qt::AlignCenter);
    
    QFormLayout *formLayout = new QFormLayout();
    usernameInput = new QLineEdit();
    passwordInput = new QLineEdit();
    passwordInput->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Username:", usernameInput);
    formLayout->addRow("Password:", passwordInput);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    loginButton = new QPushButton("Sign In");
    registerButton = new QPushButton("Register");
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);

    guestButton = new QPushButton("Play as Guest");
    loginStatusLabel = new QLabel("");
    
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

void GUIInterface::setupScoreDisplay(QVBoxLayout* layout) {
    scoreFrame = new QFrame();
    QVBoxLayout *scoreLayout = new QVBoxLayout(scoreFrame);
    scoreLayout->setContentsMargins(15, 15, 15, 15);
    scoreLayout->setSpacing(5);
    
    QLabel* scoreTitle = new QLabel("SCOREBOARD");
    scoreTitle->setObjectName("sectionTitle");
    
    playerXScoreLabel = new QLabel("Player (You): W:0 L:0 T:0");
    playerOScoreLabel = new QLabel("Opponent: W:0 L:0 T:0");
    streakLabel = new QLabel("Current Streak: 0");
    winRateLabel = new QLabel("Win Rate: 0%");
    
    scoreLayout->addWidget(scoreTitle, 0, Qt::AlignCenter);
    scoreLayout->addWidget(playerXScoreLabel);
    scoreLayout->addWidget(playerOScoreLabel);
    scoreLayout->addWidget(streakLabel);
    scoreLayout->addWidget(winRateLabel);
    
    layout->addWidget(scoreFrame);
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
    
    layout->addWidget(gameModeTab);
    connect(gameModeTab, &QTabWidget::currentChanged, this, &GUIInterface::onGameModeChanged);
}

void GUIInterface::setupGameControls(QVBoxLayout* layout) {
    controlsFrame = new QFrame();
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
}

void GUIInterface::setupHistoryView() {
    historyWidget = new QWidget();
    QVBoxLayout *historyLayout = new QVBoxLayout(historyWidget);
    
    historySplitter = new QSplitter(Qt::Horizontal);
    QFrame* tableFrame = new QFrame;
    QVBoxLayout* tableLayout = new QVBoxLayout(tableFrame);
    gameHistoryTable = new QTableWidget();
    gameHistoryTable->setColumnCount(4);
    gameHistoryTable->setHorizontalHeaderLabels({"Date", "Opponent", "Result", "Moves"});
    gameHistoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableLayout->addWidget(gameHistoryTable);
    
    historyDetailsFrame = new QFrame;
    QVBoxLayout* detailsLayout = new QVBoxLayout(historyDetailsFrame);
    gameDetailsText = new QTextEdit();
    gameDetailsText->setReadOnly(true);
    detailsLayout->addWidget(new QLabel("Game Details"));
    detailsLayout->addWidget(gameDetailsText);
    
    historySplitter->addWidget(tableFrame);
    historySplitter->addWidget(historyDetailsFrame);
    
    backToGameButton = new QPushButton("Back to Game");
    exportHistoryButton = new QPushButton("Export as CSV");
    
    historyLayout->addWidget(new QLabel("Game History"));
    historyLayout->addWidget(historySplitter, 1);
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(exportHistoryButton);
    btnLayout->addWidget(backToGameButton);
    historyLayout->addLayout(btnLayout);

    connect(backToGameButton, &QPushButton::clicked, this, &GUIInterface::onBackToGameClicked);
    connect(exportHistoryButton, &QPushButton::clicked, this, &GUIInterface::exportGameHistory);
    connect(gameHistoryTable, &QTableWidget::itemDoubleClicked, this, [this](QTableWidgetItem* item){
        onGameHistoryItemClicked(item->row(), item->column());
    });
    
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

    statsMainLayout->addWidget(new QLabel("Player Statistics"));
    statsMainLayout->addWidget(statsScrollArea);
    
    mainStack->addWidget(statsWidget);
}

void GUIInterface::setupSettingsView() {
    settingsWidget = new QWidget();
    QFormLayout *settingsLayout = new QFormLayout(settingsWidget);
    
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
    animationSpeedSlider->setRange(100, 1000); //ms
    settingsLayout->addRow("Animation Speed:", animationSpeedSlider);
    
    mainStack->addWidget(settingsWidget);
    
    connect(themeGroup, &QButtonGroup::idClicked, this, &GUIInterface::onThemeChanged);
    connect(animationsEnabledCheck, &QCheckBox::toggled, this, [this](bool checked){ animationsEnabled = checked; });
    connect(animationSpeedSlider, &QSlider::valueChanged, this, &GUIInterface::onAnimationSpeedChanged);
}

void GUIInterface::setupReplayControls() {
    replayControlsFrame = new QFrame();
    QHBoxLayout *replayLayout = new QHBoxLayout(replayControlsFrame);
    
    replayStartButton = new QPushButton("⏮️");
    replayPrevButton = new QPushButton("⏪");
    replayAutoButton = new QPushButton("▶️");
    replayNextButton = new QPushButton("⏩");
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


// --- THEME & STYLING HELPERS ---

void GUIInterface::applyTheme(Theme theme) {
    currentTheme = theme;
    QString styleSheet;
    // In a real application, these QSS strings would be loaded from resource files.
    if (theme == DARK) {
        styleSheet = "/* A full, complex dark theme stylesheet would go here */";
    } else if (theme == LIGHT) {
        styleSheet = "/* A full, complex light theme stylesheet would go here */";
    } else { // NEON
        styleSheet = "/* A full, complex neon theme stylesheet would go here */";
    }
    qApp->setStyleSheet(styleSheet);
    updateButtonStyles();
}

void GUIInterface::updateButtonStyles() {
    // This helper would be called after a theme change to update any elements
    // that have styles set directly (like the X and O colors).
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (boardButtons[i][j]->text() == "X") {
                boardButtons[i][j]->setStyleSheet(QString("color: %1;").arg(getPlayerColor(Player::X).name()));
            } else if (boardButtons[i][j]->text() == "O") {
                boardButtons[i][j]->setStyleSheet(QString("color: %1;").arg(getPlayerColor(Player::O).name()));
            }
        }
    }
}

void GUIInterface::animateButton(QWidget* widget) {
    if (!animationsEnabled) return;
    QPropertyAnimation *animation = new QPropertyAnimation(widget, "geometry");
    animation->setDuration(animationSpeed / 2);
    QRect geom = widget->geometry();
    animation->setKeyValueAt(0.0, geom);
    animation->setKeyValueAt(0.1, QRect(geom.x() - 3, geom.y(), geom.width(), geom.height()));
    animation->setKeyValueAt(0.3, QRect(geom.x() + 3, geom.y(), geom.width(), geom.height()));
    animation->setKeyValueAt(0.5, QRect(geom.x() - 3, geom.y(), geom.width(), geom.height()));
    animation->setKeyValueAt(0.7, QRect(geom.x() + 3, geom.y(), geom.width(), geom.height()));
    animation->setKeyValueAt(0.9, QRect(geom.x() - 3, geom.y(), geom.width(), geom.height()));
    animation->setKeyValueAt(1.0, geom);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void GUIInterface::addDropShadow(QWidget* widget) {
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15.0);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 100));
    widget->setGraphicsEffect(shadow);
}

// gui_interface.cpp - Part 2 of 4: Interactive Logic (Slots)

// --- SLOT IMPLEMENTATIONS (The "Logic" behind user actions) ---

void GUIInterface::onLoginButtonClicked() {
    // This slot is called when the "Sign In" button is clicked.
    if (loginButton->property("loading").toBool()) return; // Prevent multiple clicks
    
    setLoading(loginButton, true); // Show loading indicator
    
    // Use a short timer to simulate a network request and make the UI feel responsive.
    QTimer::singleShot(300, this, [this]() {
        std::string username = usernameInput->text().toStdString();
        std::string password = passwordInput->text().toStdString();
        
        if (userAuth.loginUser(username, password)) {
            showNotification("Login Successful!", "success");
            switchToGameView();
        } else {
            showNotification("Invalid Credentials", "error");
            animateButton(loginFrame); // Shake the login form on error
        }
        setLoading(loginButton, false);
    });
}

void GUIInterface::onRegisterButtonClicked() {
    if (registerButton->property("loading").toBool()) return;
    setLoading(registerButton, true);
    
    QTimer::singleShot(300, this, [this](){
        std::string username = usernameInput->text().toStdString();
        std::string password = passwordInput->text().toStdString();

        if (userAuth.registerUser(username, password)) {
            dbManager.saveUser(*userAuth.getCurrentUser());
            showNotification("Registration Successful! Please log in.", "success");
            // Clear inputs for login
            usernameInput->clear();
            passwordInput->clear();
        } else {
            showNotification("Username already exists.", "error");
        }
        setLoading(registerButton, false);
    });
}

void GUIInterface::onGuestPlayClicked() {
    userAuth.logoutUser(); // Ensure we are in a logged-out state
    showNotification("Playing as Guest. Progress will not be saved.", "info");
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
    if (!button || !button->text().isEmpty()) return;

    int row = button->property("row").toInt();
    int col = button->property("col").toInt();

    if (gameLogic.makeMove(row, col)) {
        animateCellPlacement(row, col, gameLogic.getCurrentPlayer() == Player::O ? Player::X : Player::O);
        
        GameResult result = gameLogic.checkGameResult();
        if (result != GameResult::IN_PROGRESS) {
            handleGameOver(result);
        } else {
            updateTimer(); // Reset timer for the next player
            if (gameModeTab->currentIndex() == 0) { // AI Mode
                QTimer::singleShot(animationSpeed + 50, this, &GUIInterface::makeAIMove);
            }
        }
    }
}

void GUIInterface::onNewGameButtonClicked() {
    isGameInProgress = true;
    isReplayMode = false;
    gameTimeSeconds = 0;
    
    gameLogic.resetBoard();
    resetBoardHighlights();
    updateBoard();
    updateTimer();
    gameTimer->start(1000);
    
    setupReplayControls(false); // Hide replay controls
    for(int r=0; r<3; ++r) {
        for(int c=0; c<3; ++c) {
            boardButtons[r][c]->setEnabled(true);
        }
    }
    
    showNotification("New game started!", "info");
}

void GUIInterface::onUndoMoveClicked() {
    if (!isGameInProgress || isReplayMode || gameLogic.getMoveHistory().empty()) return;
    
    gameLogic.undoLastMove();
    if (gameModeTab->currentIndex() == 0 && !gameLogic.getMoveHistory().empty()) {
        // If vs AI, undo the AI's move as well
        gameLogic.undoLastMove();
    }
    updateBoard();
}

void GUIInterface::onHintClicked() {
    if (!isGameInProgress || isReplayMode) return;
    Move hint = aiEngine.getBestMove(gameLogic);
    animateButton(boardButtons[hint.row][hint.col]);
}

void GUIInterface::onGameModeChanged() {
    onNewGameButtonClicked();
}

void GUIInterface::onThemeChanged(int id) {
    applyTheme(static_cast<Theme>(id));
}

void GUIInterface::onAnimationSpeedChanged(int value) {
    // Slider is 1-10, we map it to a reasonable ms range
    animationSpeed = 1100 - (value * 100);
}

void GUIInterface::onViewHistoryClicked() {
    switchToHistoryView();
}

void GUIInterface::onViewStatsClicked() {
    switchToStatsView();
}

void GUIInterface::onGameHistoryItemClicked(int row, int) {
    if (row < 0 || !gameHistoryTable->item(row, 0)) return;

    QVariant data = gameHistoryTable->item(row, 0)->data(Qt::UserRole);
    std::string gameId = data.toString().toStdString();
    GameState game = gameHistory.getGameById(gameId);

    if (!game.gameId.empty()) {
        displayGameForReplay(game);
    }
}

void GUIInterface::onBackToGameClicked() {
    switchToGameView();
}

void GUIInterface::onReplayNextClicked() {
    if (isReplayMode && replayMoveIndex < replayHistory.size()) {
        const auto& move = replayHistory[replayMoveIndex];
        gameLogic.makeMove(move.row, move.col);
        replayMoveIndex++;
        updateBoard(true);
        updateReplayControls();
    } else {
        if (replayAutoTimer->isActive()) {
            replayAutoTimer->stop();
            replayAutoButton->setText("▶️");
        }
    }
}

void GUIInterface::onReplayPrevClicked() {
    if (isReplayMode && replayMoveIndex > 0) {
        gameLogic.resetBoard();
        replayMoveIndex--;
        for (int i = 0; i < replayMoveIndex; ++i) {
            gameLogic.makeMove(replayHistory[i].row, replayHistory[i].col);
        }
        updateBoard(true);
        updateReplayControls();
    }
}

void GUIInterface::onReplayStartClicked() {
    if (isReplayMode) {
        replayMoveIndex = 0;
        gameLogic.resetBoard();
        updateBoard(true);
        updateReplayControls();
    }
}

void GUIInterface::onReplayAutoPlay() {
    if (!isReplayMode) return;

    if (replayAutoTimer->isActive()) {
        replayAutoTimer->stop();
        replayAutoButton->setText("▶️");
    } else {
        if(replayMoveIndex >= replayHistory.size()) {
            onReplayStartClicked();
        }
        replayAutoTimer->start(1100 - replaySpeedSlider->value());
        replayAutoButton->setText("⏸️");
    }
}

void GUIInterface::onGameTimerUpdate() {
    if(isGameInProgress && !isReplayMode) {
        gameTimeSeconds++;
        updateTimer();
    }
}

// gui_interface.cpp - Part 3 of 4: Core Helper and Game Logic Functions

// --- HELPER METHOD IMPLEMENTATIONS (The "Workhorse" functions) ---
void GUIInterface::handleGameOver(GameResult result) {
    isGameInProgress = false;
    gameTimer->stop();
    
    QString message = formatGameResult(result);
    statusLabel->setText(message);
    
    // Highlight the winning cells
    if (result == GameResult::X_WINS || result == GameResult::O_WINS) {
        highlightWinningCells(gameLogic.findWinningCombination());
    }

    // Disable all board buttons
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardButtons[i][j]->setEnabled(false);
        }
    }
    undoButton->setEnabled(false);
    hintButton->setEnabled(false);

    // Save game and update stats if a user is logged in
    if (userAuth.isLoggedIn()) {
        const UserProfile* currentUser = userAuth.getCurrentUser();
        // Determine opponent
        std::string opponentId = (gameModeTab->currentIndex() == 0) ? "AI" : "Player2";
        
        gameHistory.saveGame(currentUser->userId, opponentId, (gameModeTab->currentIndex() == 0), gameLogic.getMoveHistory(), result);
        dbManager.saveGameHistory(gameHistory.getAllGames());
        userAuth.updateUserStats(result);
        dbManager.saveUser(*currentUser);
        updateScoreDisplay();
    }
    
    animateGameOver(result);
}

void GUIInterface::makeAIMove() {
    aiThinkingBar->setVisible(true);
    // Use a timer to simulate AI thinking and prevent the GUI from freezing
    QTimer::singleShot(animationSpeed, this, [this]() {
        Move aiMove = aiEngine.getBestMove(gameLogic);
        if (gameLogic.makeMove(aiMove.row, aiMove.col)) {
            animateCellPlacement(aiMove.row, aiMove.col, Player::O);
            
            GameResult result = gameLogic.checkGameResult();
            if (result != GameResult::IN_PROGRESS) {
                handleGameOver(result);
            } else {
                updateTimer(); // Reset timer for the player's turn
            }
        }
        aiThinkingBar->setVisible(false);
        undoButton->setEnabled(true);
    });
}

void GUIInterface::showHint() {
    if (!isGameInProgress || isReplayMode) return;
    Move hint = aiEngine.getBestMove(gameLogic);
    if(hint.row != -1){
        animateButton(boardButtons[hint.row][hint.col]);
    }
}

void GUIInterface::updateBoard(bool isReplay) {
    if (!isReplay) {
        statusLabel->setText(QString::fromStdString(getPlayerName(gameLogic.getCurrentPlayer())) + "'s Turn");
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Player cell = gameLogic.getCell(i, j);
            QPushButton* button = boardButtons[i][j];

            // Update text only if it has changed to avoid re-triggering animations
            QString currentText = (cell == Player::X) ? "X" : (cell == Player::O) ? "O" : "";
            if(button->text() != currentText) {
                 button->setText(currentText);
            }

            // Update style based on player
            if (cell != Player::NONE) {
                 button->setStyleSheet(QString("color: %1;").arg(getPlayerColor(cell).name()));
            } else {
                 button->setStyleSheet(""); // Reset stylesheet for empty cells
            }
        }
    }
}

void GUIInterface::updateScoreDisplay() {
    if (userAuth.isLoggedIn()) {
        const UserProfile* user = userAuth.getCurrentUser();
        playerXScoreLabel->setText(QString("Player (You): W:%1 L:%2 T:%3").arg(user->gamesWon).arg(user->gamesLost).arg(user->gamesTied));
    } else {
        playerXScoreLabel->setText("Player (You): W:0 L:0 T:0");
    }
    // Opponent score is a placeholder for now
    playerOScoreLabel->setText("Opponent: W:0 L:0 T:0");
    // In a real app, you would calculate streak and win rate here
    streakLabel->setText("Streak: 0");
    winRateLabel->setText("Win Rate: 0%");
}

void GUIInterface::updateGameStats() {
    if (!userAuth.isLoggedIn()) {
        totalGamesLabel->setText("Log in to see your stats!");
        winRateStatsLabel->setText("");
        averageGameTimeLabel->setText("");
        longestStreakLabel->setText("");
        favoriteOpponentLabel->setText("");
        return;
    }
    const UserProfile* user = userAuth.getCurrentUser();
    totalGamesLabel->setText(QString("Total Games Played: %1").arg(user->gamesPlayed));
    if (user->gamesPlayed > 0) {
        double rate = (static_cast<double>(user->gamesWon) / user->gamesPlayed) * 100.0;
        winRateStatsLabel->setText(QString("Overall Win Rate: %1%").arg(rate, 0, 'f', 1));
    } else {
        winRateStatsLabel->setText("Overall Win Rate: N/A");
    }
    averageGameTimeLabel->setText("Average Game Time: 00:42"); // Placeholder
    longestStreakLabel->setText("Longest Win Streak: 0"); // Placeholder
    favoriteOpponentLabel->setText("Favorite Mode: vs AI"); // Placeholder
}

void GUIInterface::updateTimer() {
    timerLabel->setText(formatTime(gameTimeSeconds));
}

void GUIInterface::highlightWinningCells(const std::vector<Move>& cells) {
    if (cells.empty()) return;
    
    QColor highlightColor = getPlayerColor(gameLogic.getCell(cells[0].row, cells[0].col)).lighter(150);
    
    for (const auto& cell : cells) {
        boardButtons[cell.row][cell.col]->setStyleSheet(
            QString("background-color: %1; color: %2; border: 2px solid white;")
            .arg(highlightColor.name())
            .arg(getPlayerColor(gameLogic.getCell(cell.row, cell.col)).name())
        );
    }
}

void GUIInterface::resetBoardHighlights() {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardButtons[i][j]->setStyleSheet(""); // Reset to default stylesheet
        }
    }
}

// --- ANIMATION HELPERS ---

void GUIInterface::animateCellPlacement(int row, int col, Player player) {
    if (!animationsEnabled) {
        updateBoard();
        return;
    }
    
    QPushButton* button = boardButtons[row][col];
    button->setText(getPlayerName(player));
    button->setStyleSheet(QString("color: %1;").arg(getPlayerColor(player).name()));

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(button);
    button->setGraphicsEffect(effect);
    
    QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(animationSpeed);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void GUIInterface::addGlowEffect(QWidget* widget, const QColor& color) {
    QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(this);
    glow->setColor(color);
    glow->setBlurRadius(25);
    glow->setOffset(0);
    widget->setGraphicsEffect(glow);
    widget->graphicsEffect()->setEnabled(false); // Disabled by default
    // In a real app, you would enable this on hover via event filters
}

// gui_interface.cpp - Part 4 of 4: Final Helpers and Utilities

// --- NAVIGATION HELPERS ---

void GUIInterface::switchToLoginView() {
    mainStack->setCurrentWidget(loginWidget);
    // Hide the main navigation sidebar on the login screen
    navigationFrame->setVisible(false);
}

void GUIInterface::switchToGameView() {
    mainStack->setCurrentWidget(gameWidget);
    navigationFrame->setVisible(true);
    gameNavButton->setChecked(true);
    updateScoreDisplay();
}

void GUIInterface::switchToHistoryView() {
    loadUserGames(); // Refresh history every time we switch to it
    mainStack->setCurrentWidget(historyWidget);
    historyNavButton->setChecked(true);
}

void GUIInterface::switchToStatsView() {
    updateGameStats(); // Refresh stats every time we switch to it
    mainStack->setCurrentWidget(statsWidget);
    statsNavButton->setChecked(true);
}

void GUIInterface::switchToSettingsView() {
    mainStack->setCurrentWidget(settingsWidget);
    settingsNavButton->setChecked(true);
}

void GUIInterface::updateNavigationButtons() {
    // This function can be used to visually update the navigation sidebar
    // based on the current view, but setChecked handles it for us with a QButtonGroup.
}

// --- HISTORY AND REPLAY HELPERS ---

void GUIInterface::loadUserGames() {
    if (!userAuth.isLoggedIn()) return;
    
    const UserProfile* currentUser = userAuth.getCurrentUser();
    std::vector<GameState> userGames = gameHistory.getUserGames(currentUser->userId);
    
    gameHistoryTable->setRowCount(0); // Clear the table
    
    for (const auto& game : userGames) {
        int row = gameHistoryTable->rowCount();
        gameHistoryTable->insertRow(row);
        
        // Store the gameId in the item's data role for later retrieval
        QTableWidgetItem* dateItem = new QTableWidgetItem(QString::fromStdString(game.timestamp));
        dateItem->setData(Qt::UserRole, QVariant(QString::fromStdString(game.gameId)));
        gameHistoryTable->setItem(row, 0, dateItem);
        
        QString opponent = game.isAIOpponent ? "AI" : "Player2";
        gameHistoryTable->setItem(row, 1, new QTableWidgetItem(opponent));
        
        gameHistoryTable->setItem(row, 2, new QTableWidgetItem(formatGameResult(game.result)));
        
        QString mode = game.isAIOpponent ? "vs AI" : "vs Player";
        gameHistoryTable->setItem(row, 3, new QTableWidgetItem(mode));
    }
}

void GUIInterface::displayGameForReplay(const GameState& game) {
    if (game.gameId.empty()) return;

    isReplayMode = true;
    replayHistory = game.moveHistory;
    
    onReplayStartClicked(); // Reset to the beginning of the replay
    
    statusLabel->setText("Replay: " + QString::fromStdString(game.timestamp));
    setupReplayControls(true); // Show the replay control panel
    switchToGameView();
}

void GUIInterface::updateReplayControls() {
    if (!isReplayMode) return;
    replayPositionLabel->setText(QString("Move: %1 / %2").arg(replayMoveIndex).arg(replayHistory.size()));
    replayPrevButton->setEnabled(replayMoveIndex > 0);
    replayStartButton->setEnabled(replayMoveIndex > 0);
    replayNextButton->setEnabled(replayMoveIndex < replayHistory.size());
}

void GUIInterface::exportGameHistory() {
    if (!userAuth.isLoggedIn()) {
        showNotification("You must be logged in to export history.", "warning");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, "Save Game History", "tictactoe_history.csv", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "GameID,Date,Opponent,Result,Mode,MoveHistory\n";
        auto games = gameHistory.getUserGames(userAuth.getCurrentUser()->userId);
        for (const auto& game : games) {
            out << QString::fromStdString(game.gameId) << ","
                << "\"" << QString::fromStdString(game.timestamp) << "\","
                << (game.isAIOpponent ? "AI" : "Player2") << ","
                << formatGameResult(game.result) << ","
                << (game.isAIOpponent ? "vs AI" : "vs Player") << ",";
            QString moves;
            for(size_t i = 0; i < game.moveHistory.size(); ++i) {
                moves += QString("(%1,%2)").arg(game.moveHistory[i].row).arg(game.moveHistory[i].col);
                if (i < game.moveHistory.size() - 1) moves += ";";
            }
            out << "\"" << moves << "\"\n";
        }
        file.close();
        showNotification("History exported successfully!", "success");
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(file).path()));
    } else {
        showNotification("Failed to save file.", "error");
    }
}

// --- SETTINGS HELPERS ---

void GUIInterface::loadSettings() {
    QSettings settings("TicTacToePro", "AppSettings");
    currentTheme = static_cast<Theme>(settings.value("theme", static_cast<int>(DARK)).toInt());
    animationsEnabled = settings.value("animationsEnabled", true).toBool();
    animationSpeed = settings.value("animationSpeed", 300).toInt();
    applySettings();
}

void GUIInterface::saveSettings() {
    QSettings settings("TicTacToePro", "AppSettings");
    settings.setValue("theme", static_cast<int>(currentTheme));
    settings.setValue("animationsEnabled", animationsEnabled);
    settings.setValue("animationSpeed", animationSpeed);
}

void GUIInterface::applySettings() {
    themeGroup->button(currentTheme)->setChecked(true);
    animationsEnabledCheck->setChecked(animationsEnabled);
    animationSpeedSlider->setValue(1100 - animationSpeed);
    applyTheme(currentTheme);
}

// --- ANIMATION & FORMATTING UTILITIES ---

void GUIInterface::fadeInWidget(QWidget* widget) {
    if (!animationsEnabled) {
        widget->setVisible(true);
        return;
    }
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);
    widget->setVisible(true);
    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(animationSpeed);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

QString GUIInterface::formatTime(int totalSeconds) {
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

QString GUIInterface::formatGameResult(GameResult result) {
    switch(result) {
        case GameResult::X_WINS: return "Player X Won";
        case GameResult::O_WINS: return "Player O Won";
        case GameResult::DRAW: return "Draw";
        default: return "In Progress";
    }
}

QString GUIInterface::getPlayerName(Player player) {
    return (player == Player::X) ? "X" : "O";
}

QColor GUIInterface::getPlayerColor(Player player) {
    if (player == Player::X) return QColor("#3498DB"); // Blue for X
    if (player == Player::O) return QColor("#E74C3C"); // Red for O
    return Qt::white;
}

void GUIInterface::showNotification(const QString& message, const QString& type) {
    QMessageBox msgBox;
    if (type == "error") {
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("Error");
    } else if (type == "success") {
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Success");
    } else {
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Info");
    }
    msgBox.setText(message);
    msgBox.exec();
}

void GUIInterface::setLoading(QPushButton* button, bool loading) {
    if (loading) {
        button->setProperty("loading", true);
        button->setEnabled(false);
        // In a real app, you could add a QProgressBar or spinner icon
    } else {
        button->setProperty("loading", false);
        button->setEnabled(true);
    }
}