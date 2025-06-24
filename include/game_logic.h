// game_logic.h
#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <array>
#include <string>
#include <vector>

enum class Player { NONE, X, O };
enum class GameResult { IN_PROGRESS, X_WINS, O_WINS, DRAW };

struct Move {
    int row;
    int col;

    Move(int r = -1, int c = -1) : row(r), col(c) {}
};

struct GameState {
    std::string gameId;
    std::string player1Id;
    std::string player2Id;
    bool isAIOpponent;
    std::vector<Move> moveHistory;
    GameResult result;
    std::string timestamp;
    int durationSeconds = 0;
    
    GameState() : result(GameResult::IN_PROGRESS), isAIOpponent(false) {}
};

class GameLogic {
public:
    GameLogic();

    void resetBoard();
    bool makeMove(int row, int col);
    bool isValidMove(int row, int col) const;
    GameResult checkGameResult() const;
    bool isBoardFull() const;

    // NEW: This function will return the coordinates of the 3 winning cells.
    std::vector<Move> findWinningCombination() const;

    Player getCurrentPlayer() const;
    Player getCell(int row, int col) const;
    const std::vector<Move>& getMoveHistory() const;
    std::vector<Move> getAvailableMoves() const;
    const std::array<std::array<Player, 3>, 3>& getBoard() const;

    // Now public so the replay system can use it easily.
    void undoLastMove();

private:
    std::array<std::array<Player, 3>, 3> board;
    Player currentPlayer;
    std::vector<Move> moveHistory;

    bool checkWin(Player player) const;
    void recordMove(int row, int col);
};
#endif // GAME_LOGIC_H