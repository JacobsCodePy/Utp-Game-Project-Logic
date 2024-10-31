//
// Created by Kuuba Puacz on 22/10/2024.
//

#ifndef UTP_GAME_PROJECT_LOGIC_GAME_H
#define UTP_GAME_PROJECT_LOGIC_GAME_H

#include <vector>

// Contains whole game state management
class Game {

public:

    // Represents a single game tile
    enum class Tile {
        Blank, BlackPawn, WhitePawn, BlackQueen, WhiteQueen,
    };

    // Represents game player side
    enum class Player {
        White, Black, None
    };

    // Contains information about move process.
    struct MoveResult {
        std::vector<std::pair<int, int>> takenPawns; // Positions of beaten pawns by the move
        Player winner; // Winner color if there is a winner
        bool isQueen; // Whether this move led the pawn to transfer into the Queen
        bool isCorrect; // Whether move was correct or not
        std::string message; // Description of an error if such occurred
    };

    ////////////////////////////////////
    ////////////////////////////////////

    [[nodiscard]] bool isSingleMoveForward(std::pair<int, int> const & displacement) const;
    [[nodiscard]] bool isQueenTransformation(std::pair<int, int> const & to) const;
    [[nodiscard]] bool isFree(std::pair<int, int> const & position) const;
    [[nodiscard]] bool isPawnWhite(Tile tile) const;
    [[nodiscard]] bool isPawnBlack(Tile tile) const;
    [[nodiscard]] bool hasPosition(std::pair<int, int> where) const;

    [[nodiscard]] Tile getCurrentQueen() const;
    [[nodiscard]] Tile const & get (std::pair<int, int> const & where) const;
    [[nodiscard]] Player getPawnColor(std::pair<int, int> const & position) const;
    [[nodiscard]] Player getCurrentPlayer() const;
    [[nodiscard]] Player getOpponent() const;

    [[nodiscard]] int getWhitePawnsAmount() const;
    [[nodiscard]] int getBlackPawnsAmount() const;

    ////////////////////////////////////
    ////////////////////////////////////

    Game();
    Game(std::vector<Game::Tile> const &state, Player const& currentPlayer);
    MoveResult process(std::pair<int, int> const & from, std::pair<int, int> const & to);
    void processPawn(MoveResult & result, std::pair<int, int> const & from, std::pair<int, int> const & to) const;
    void processQueen(MoveResult & result, std::pair<int, int> const & from, std::pair<int, int> const & to) const;
    void capture(std::pair<int, int> const & where) const;
    void fill(Tile pawn, std::vector<int> const & range);

private:

    /// Given 2 positions calculates the difference between them taking into account
    /// the direction, in which current player can move. In result for white positive value of
    /// e.g. row means moving to the top, whereas for black it is moving to the bottom.
    [[nodiscard]] std::pair<int, int> getRelativeDisplacement(std::pair<int, int> from, std::pair<int, int> to) const;

    // Checks the closest opponents pawn on diagonal and returns position
    // behind it if it is not taken by some other pawn. If there is more than
    // one such positions returns all of them.
    [[nodiscard]] std::vector<std::pair<int, int>> getClosestOpponentsPawns(std::pair<int, int> const & position) const;

    // Recursively calls itself with certain rules to explore all the paths
    // between the target position and current pawns position during jump capture of opponents pawns.
    void processCapturedPaths(std::vector<std::vector<std::pair<int, int>>> & paths,
                              std::vector<std::pair<int, int>> & uniques,
                              std::pair<int, int> const & position,
                              std::pair<int, int> const & target) const;

    // Returns a vector of positions, which contain captured opponents pawns during the jumping process.
    // If returned vector is empty, it means that the move is illegal.
    [[nodiscard]] std::vector<std::pair<int, int>> processCapturingOpponentsPawns(std::pair<int, int> const & from,
                                                                    std::pair<int, int> const & to) const;


private:
    std::vector<std::unique_ptr<std::vector<Tile>>> m_data;
    Player m_current;
    int m_whiteAmount;
    int m_blackAmount;
};

#endif //UTP_GAME_PROJECT_LOGIC_GAME_H
