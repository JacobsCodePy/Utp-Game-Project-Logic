//
// Created by Kuuba Puacz on 22/10/2024.
//

#ifndef UTP_GAME_PROJECT_LOGIC_GAME_H
#define UTP_GAME_PROJECT_LOGIC_GAME_H

#include <vector>
#include "PathTree.h"

// Contains whole game state management
class Game {

public:

    // Contains overall game settings
    struct Settings {
        bool isBoardInverted = false;
    };

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

    // Initializes the checker's board with given size as side size of the board.
    Game(Settings & settings, int size);

    // Fills the board with the tiles in the correct positions.
    void fill(Tile pawn, std::vector<int> const & range);

    bool isSingleMoveForward(std::pair<int, int> const & displacement) const;
    bool isQueenTransformation(std::pair<int, int> const & to) const;
    bool isFree(std::pair<int, int> const & position) const;
    bool isPawnWhite(Tile tile) const;
    bool isPawnBlack(Tile tile) const;
    bool hasPosition(std::pair<int, int> where) const;

    Game::Tile getCurrentPawn() const;
    Game::Tile getCurrentQueen() const;

    std::pair<int, int> getRelativeDisplacement(std::pair<int, int> from, std::pair<int, int> to) const;
    Game::Player getPawnColor(std::pair<int, int> const & position) const;

    void processPawn(MoveResult & result, std::pair<int, int> const & from, std::pair<int, int> const & to) const;
    void processQueen(MoveResult & result, std::pair<int, int> const & from, std::pair<int, int> const & to) const;

    MoveResult process(std::pair<int, int> const & from, std::pair<int, int> const & to);

    Tile const & get (std::pair<int, int> const & where) const;
    void capture(std::pair<int, int> const & where) const;

    void reset();
    Player getCurrentPlayer() const;

private:

    // Checks the closest opponents pawn and returns position
    // behind it if it is not taken by some other pawn. If there is more than
    // one such positions returns more.
    std::vector<std::pair<int, int>> getClosestOpponentsPawns(std::pair<int, int> const & position) const;

    // Uses `getClosestOpponentsPawns` recursively with certain rules to find the path
    // between the target position and current pawns position during jump beating.
    void processCapturePaths(PathTree & tree,
                             std::shared_ptr<PathTree::Leaf> const & start,
                             std::pair<int, int> const & position,
                             std::pair<int, int> const & target) const;

    // Returns vector of positions, which contain beaten opponents pawns during the jumping process.
    // If returned vector is empty, it means that the move is illegal.
    std::vector<std::pair<int, int>> processCapturingOpponentsPawns(std::pair<int, int> const & from,
                                                                    std::pair<int, int> const & to) const;



private:
    std::vector<std::unique_ptr<std::vector<Tile>>> m_data;
    Player m_current;
    int m_whiteAmount;
    int m_blackAmount;
    Settings &m_settings;
};

#endif //UTP_GAME_PROJECT_LOGIC_GAME_H
