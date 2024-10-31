//
// Created by Kuuba Puacz on 22/10/2024.
//

#include <iostream>
#include <math.h>
#include "Game.h"


////////////////////////////////////////////////
////////////////////////////////////////////////
/// Helper Functions
////////////////////////////////////////////////
////////////////////////////////////////////////


auto operator + (std::pair<int, int> const & left,
                                std::pair<int, int> const & right) {
    return std::pair<int, int>{left.first + right.first, left.second + right.second};
}

auto operator += (std::pair<int, int> & left,
                 std::pair<int, int> const & right) {
    left.first += right.first;
    left.second += right.second;
}

auto operator - (std::pair<int, int> const & left,
                 std::pair<int, int> const & right) {
    return std::pair<int, int>{left.first - right.first, left.second - right.second};
}

auto operator -= (std::pair<int, int> & left,
                  std::pair<int, int> const & right) {
    left.first -= right.first;
    left.second -= right.second;
}

auto operator * (std::pair<int, int> const & position, int value) {
    return std::pair<int, int>{position.first * value, position.second * value};
}

auto operator / (std::pair<int, int> const & position, int value) {
    return std::pair<int, int>{position.first / value, position.second / value};
}


////////////////////////////////////////////////
////////////////////////////////////////////////
/// Class Member Helpers
////////////////////////////////////////////////
////////////////////////////////////////////////


Game::Tile const & Game::get(std::pair<int, int> const & where) const {
    if (!hasPosition(where)) throw std::runtime_error("Tried to access incorrect position of the board.");
    return m_data[where.first]->at(where.second);
}

void Game::capture(std::pair<int, int> const & where) const {
    m_data[where.first]->at(where.second) = Game::Tile::Blank;
}

bool Game::hasPosition(std::pair<int, int> where) const {
    return where.first < m_data.size() && where.first >= 0 &&
           where.second < m_data.size() && where.second >= 0;
}

bool Game::isPawnWhite(Game::Tile tile) const {
    return tile == Game::Tile::WhitePawn || tile == Game::Tile::WhiteQueen;
}

bool Game::isPawnBlack(Game::Tile tile) const {
    return tile == Game::Tile::BlackPawn || tile == Game::Tile::BlackQueen;
}

bool Game::isFree(std::pair<int, int> const & position) const {
    return get(position) == Game::Tile::Blank;
}

bool Game::isSingleMoveForward(const std::pair<int, int> &displacement) const {
    return displacement.first == 1 && (displacement.second == -1 || displacement.second == 1);
}

bool Game::isQueenTransformation(const std::pair<int, int> &to) const {
    if (m_current == Game::Player::White && to.first == 0) return true;
    else if (to.first == m_data.size() - 1) return true;
    return false;
}

std::pair<int, int> Game::getRelativeDisplacement(std::pair<int, int> from, std::pair<int, int> to) const {
    if (m_current == Game::Player::White)
        return {from.first - to.first, from.second - to.second};
    else
        return {to.first - from.first, to.second - from.second};
}

Game::Player Game::getPawnColor(std::pair<int, int> const & position) const {
    auto tile = get(position);
    if (isPawnBlack(tile)) return Player::Black;
    else if (isPawnWhite(tile)) return Player::White;
    else return Player::None;
}

Game::Tile Game::getCurrentQueen() const {
    if (m_current == Game::Player::White) return Game::Tile::WhiteQueen;
    else if (m_current == Game::Player::Black) return Game::Tile::BlackQueen;
    else return Game::Tile::Blank;
}

Game::Player Game::getCurrentPlayer() const {
    return m_current;
}

Game::Player Game::getOpponent() const {
    return m_current == Player::White ? Player::Black : Player::White;
}

int Game::getBlackPawnsAmount() const {
    return m_blackAmount;
}

int Game::getWhitePawnsAmount() const {
    return m_whiteAmount;
}


////////////////////////////////////////////////
////////////////////////////////////////////////
/// Class Main Functionality
////////////////////////////////////////////////
////////////////////////////////////////////////


Game::Game() {
    // Init the board
    m_data = std::vector<std::unique_ptr<std::vector<Tile>>>();
    auto size = 8;
    for (auto i = 0; i < size; i++) {
        auto row = std::make_unique<std::vector<Tile>>(10, Tile::Blank);
        m_data.push_back(std::move(row));
    }
    m_current = Game::Player::White;
    m_blackAmount = 12;
    m_whiteAmount = 12;

    // Filling board
    auto totalAmount = size * size;
    auto pawnTilesAmount = 24; // amount of tiles with possibility of having a pawn
    auto indices = std::vector<int>(pawnTilesAmount);
    for (auto i = 0; i < indices.size(); i++) indices[i] = i;
    fill(Tile::BlackPawn, indices);
    std::ranges::reverse(indices);
    std::ranges::transform(indices, indices.begin(),
                           [totalAmount, pawnTilesAmount](int i)
                           { return i + totalAmount - pawnTilesAmount; });
    fill(Tile::WhitePawn, indices);
}

Game::Game(const std::vector<Game::Tile> &state, Player const& currentPlayer)
: m_current(currentPlayer) {
    // Init the board
    m_data = std::vector<std::unique_ptr<std::vector<Tile>>>();
    auto size = static_cast<int>(sqrt(static_cast<double>(state.size())));
    if (size * size != state.size()) throw std::runtime_error("GameState module was given a flat state of "
                                                              "incorrect size.");
    m_blackAmount = 0;
    m_whiteAmount = 0;
    for (auto i = 0; i < size; i++) {
        auto row = std::make_unique<std::vector<Tile>>();
        for (int j = 0; j < size; j++) {
            auto tile = state[i * size + j];
            row->push_back(tile);
            if (tile == Game::Tile::BlackPawn || tile == Game::Tile::BlackQueen) m_blackAmount++;
            else if (tile == Game::Tile::WhitePawn || tile == Game::Tile::WhiteQueen) m_whiteAmount++;
        }
        m_data.push_back(std::move(row));
    }
}

void Game::fill(Game::Tile pawn, std::vector<int> const & range) {
    for (auto i : range) {
        auto condition = (i + i / m_data.size()) % 2 == 1;
        if (condition) m_data[i / m_data.size()]->at(i % m_data.size()) = pawn;
    }
}

Game::MoveResult Game::process(std::pair<int, int> const & from, std::pair<int, int> const & to) {
    auto result = MoveResult();
    result.winner = Player::None;
    result.isCorrect = false;
    result.message = "";
    result.takenPawns = std::vector<std::pair<int, int>>();
    result.isQueen = false;
    auto pawn = get(from);

    // Check if from position marks some pawn
    if (isFree(from)) {
        result.message = "Incorrect move. You can't move without a pawn being selected.";
        return result;
    }

    // Checking if from tile contains correct pawns
    if (getPawnColor(from) != m_current) {
        auto who = std::string(m_current == Game::Player::White ? "white" : "black");
        result.message = "Illegal move. Now is the " + who + " move.";
        return result;
    }

    // Checking if the move is to the already occupied spot
    if (!isFree(to)) {
        result.message = "Illegal move. Can't move to already occupied spots.";
        return result;
    }

    // Pawn type specific checking
    if (pawn == Game::Tile::BlackQueen || pawn == Game::Tile::WhiteQueen) {
        processPawn(result, from, to);
        if (!result.isCorrect) processQueen(result, from, to);
    }
    else { // Pawn moves
        processPawn(result, from, to);
    }

    // Checks whether checking process succeeded
    if (!result.isCorrect) return result;

    // Process taken pawns
    for (auto & p : result.takenPawns) capture(p);

    // Subtract taken pawns
    if (m_current == Game::Player::White)
        m_blackAmount -= static_cast<int>(result.takenPawns.size());
    else m_whiteAmount -= static_cast<int>(result.takenPawns.size());

    // Check if someone has won
    if (m_whiteAmount == 0) result.winner = Game::Player::Black;
    else if (m_blackAmount == 0) result.winner = Game::Player::White;

    // Updating the state
    m_data[to.first]->at(to.second) = m_data[from.first]->at(from.second);
    m_data[from.first]->at(from.second) = Game::Tile::Blank;

    // Should the pawn be transformed into the Queen?
    if (isQueenTransformation(to)) {
        result.isQueen = true;
        m_data[to.first]->at(to.second) = getCurrentQueen();
    }

    // Switch the current player
    m_current = m_current == Game::Player::White ? Game::Player::Black : Game::Player::White;

    return result;
}

void Game::processPawn(MoveResult & result, const std::pair<int, int> &from,
                                   const std::pair<int, int> &to) const {
    auto displacement = getRelativeDisplacement(from, to);
    // Action specific checking
    if (isSingleMoveForward(displacement)) {
        // Check if tile is not occupied (notice that tile is assured not to be blank thanks
        // to previous checks).
        result.isCorrect = true;
        return;
    }
    else {
        auto takenPawns = processCapturingOpponentsPawns(from, to);
        if (takenPawns.empty()) {
            result.message ="Selected move is incorrect.";
            return;
        }
        result.takenPawns = takenPawns;
        result.isCorrect = true;
        return;
    }

}

void Game::processQueen(MoveResult & result, std::pair<int, int> const & from,
                        std::pair<int, int> const & to) const {
    // Checking if move is diagonal
    auto displacement = to - from;
    if (abs(displacement.first) != abs(displacement.second)) {
        result.message = "Invalid move. Queen can either move or take single pawn on diagonal "
                         "or behave like a normal pawn.";
        return;
    }


    // If pawn is captured it has to be 1 place afterward else it has to be move without capturing.
    auto step = std::pair<int, int>{displacement.first / abs(displacement.first),
                                    displacement.second / abs(displacement.second)};
    auto position = to - step;
    if (position != from && getPawnColor(position) == m_current) {
        result.message = "Invalid move. When queen moves on further diagonal it can take at most "
                         "one pawn, which is right behind her final position. It can't move behind "
                         "its own pawns.";
        return;
    }

    // If it was single move then terminate
    if (position == from) {
        result.isCorrect = true;
        return;
    }

    // Making sure there are no other pawns in the way of diagonal move if such occurred
    while ((position = position - step) != from) {
        if (!isFree(position)) {
            result.message = "Invalid move. When queen moves on further diagonal it can take at most "
                             "one pawn, which is right behind her final position. The rest of the diagonal "
                             "ought to be free.";
            return;
        }
    }

    // If the place behind destination was an opponent pawn then it is captured
    if (getPawnColor(to - step) == getOpponent())
        result.takenPawns = std::vector<std::pair<int, int>>{to - step};
    result.isCorrect = true;
}

std::vector<std::pair<int, int>> Game::getClosestOpponentsPawns(std::pair<int, int> const & position) const {
    // All possible cross directions, in which opponents may be placed
    auto directions = std::vector<std::pair<int, int>>{
        std::pair<int, int>{1, 1}, std::pair<int, int>{1, -1},
        std::pair<int, int>{-1, 1}, std::pair<int, int>{-1, -1}
    };
    auto positions = std::vector<std::pair<int, int>>(directions);
    std::ranges::transform(positions, positions.begin(), [position](auto & dir){ return position + dir; });
    auto zipped = std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>();
    for (auto i = 0; i < positions.size(); i++) zipped.emplace_back(positions[i], directions[i]);

    // Only the places, which contain opponent are a valid direction for capturing opponents pawn
    auto range = std::ranges::remove_if(zipped, [this](auto pair)
    { return !hasPosition(pair.first) || isFree(pair.first) || getPawnColor(pair.first) == m_current; });
    zipped.erase(range.begin(), range.end());

    // Transforming into possible next jump moves
    auto newPositions = std::vector<std::pair<int, int>>(zipped.size());
    std::ranges::transform(zipped, newPositions.begin(), [](auto pair){ return pair.first + pair.second; });
    return newPositions;
}

void Game::processCapturedPaths(std::vector<std::vector<std::pair<int, int>>> & paths,
                                std::vector<std::pair<int, int>> & uniques,
                                std::pair<int, int> const & position,
                                std::pair<int, int> const & target) const {
    auto positions = getClosestOpponentsPawns(position);
    for (auto const & p : positions) {

        // Else creating more branches out of this position using recursion
        paths.back().push_back(p);

        // If reached the proper end of the tree store it and create new branch from start position.
        if (p.first == target.first && p.second == target.second) {
            paths.push_back(paths.back());
            paths.back().pop_back();
            uniques.push_back(p);
            continue;
        }

        // If this position is taken by an opponent or a repetition of some previous positions then cut the branch
        if (!hasPosition(p) || !isFree(p) || std::ranges::find(uniques, p) != uniques.end()) {
            paths.back().pop_back();
            uniques.push_back(p);
            continue;
        }

        uniques.push_back(p);
        processCapturedPaths(paths, uniques, p, target);
        paths.back().pop_back();
    }
}

std::vector<std::pair<int, int>> Game::processCapturingOpponentsPawns(std::pair<int, int> const & from,
                                                                std::pair<int, int> const & to) const {
    // Finding paths, which user could have used as a way
    // to jump and take some of the opponent's pawns
    auto paths = std::vector<std::vector<std::pair<int, int>>>();
    auto uniques = std::vector<std::pair<int, int>>();
    paths.push_back(std::vector<std::pair<int, int>>{from});
    processCapturedPaths(paths, uniques, from, to);

    // If there are no resulting paths, the move is incorrect
    auto results = std::vector<std::pair<int, int>>();
    if (paths.size() == 1 && paths.back().size() == 1) return results;

    // Assumes the best case scenario of the pawns capturing, which is the longest one
    std::ranges::sort(paths, std::ranges::less(), [](auto vector){ return vector.size(); });
    auto positions = paths.back();

    // If move is correct the function should return the taken opponent's pawns positions as a result
    // thus the jump path positions are transformed into the opponents pawns positions
    auto directions = std::vector<std::pair<int, int>>();
    for (int i = 0; i < positions.size() - 1; i++) directions.push_back(positions[i + 1] - positions[i]);
    std::ranges::transform(directions, directions.begin(), [](auto d){ return d / 2; });
    for (int i = 0; i < directions.size(); i++) positions[i] += directions[i];
    positions.pop_back(); // last position does not hold taken pawn place here
    return positions;
}