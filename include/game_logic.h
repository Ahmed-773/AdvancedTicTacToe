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

    Move(int r, int c) : row(r), col(c) {}
};

struct GameState {
    std::string gameId;
    std::string player1Id;
    std::string player2Id;
    bool isAIOpponent;
    std::vector<Move> moveHistory;
    GameResult result;
    std::string timestamp;

    GameState() : result(GameResult::IN_PROGRESS), isAIOpponent(false) {}
};

class GameLogic {
public:
    GameLogic();

    void resetBoard();
    bool makeMove(int row, int col);
    bool isValidMove(int row, int col) const;
    GameResult checkGameResult() const;

    // This function is now public for testing purposes.
    bool isBoardFull() const;

    Player getCurrentPlayer() const;
    Player getCell(int row, int col) const;
    const std::vector<Move>& getMoveHistory() const;
    std::vector<Move> getAvailableMoves() const;
    const std::array<std::array<Player, 3>, 3>& getBoard() const;
    
private:
    std::array<std::array<Player, 3>, 3> board;
    Player currentPlayer;
    std::vector<Move> moveHistory;

    bool checkWin(Player player) const;
    void recordMove(int row, int col);
    void undoLastMove();
};

#endif // GAME_LOGIC_H
