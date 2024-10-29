//
// Created by Kuuba Puacz on 22/10/2024.
//

#include <ranges>
#include <iostream>
#include "Game.h"

////////////////////////////////////////////////
//////////// HELPER FUNCTIONS
////////////////////////////////////////////////

bool isInClosed(int value, std::pair<int, int> const & range) {
    return value <= range.second && value >= range.first;
}

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
////////////////////////////////////////////////


Game::Game(Settings & settings, int size): m_settings(settings) {
    // Init the board
    m_data = std::vector<std::unique_ptr<std::vector<Tile>>>();
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

void Game::fill(Game::Tile pawn, std::vector<int> const & range) {
    for (auto i : range) {
        auto condition = (i + i / m_data.size()) % 2 == 1;
        if (condition) m_data[i / m_data.size()]->at(i % m_data.size()) = pawn;
    }
}

Game::Tile const & Game::get(std::pair<int, int> const & where) const {
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
    return isInClosed(displacement.first, {0, 1}) &&
           (displacement.second == -1 || displacement.second == 1);
}

bool Game::isQueenTransformation(const std::pair<int, int> &to) const {
    if (m_current == Game::Player::White) {
        if (to.first == 0 && !m_settings.isBoardInverted) return true;
        if (to.first == m_data.size() - 1 && m_settings.isBoardInverted) return true;
    } else {
        if (to.first == 0 && m_settings.isBoardInverted) return true;
        if (to.first == m_data.size() - 1 && !m_settings.isBoardInverted) return true;
    }
    return false;
}

std::pair<int, int> Game::getRelativeDisplacement(std::pair<int, int> from, std::pair<int, int> to) const {
    auto isWhite = m_current == Game::Player::White;
    if ((isWhite && !m_settings.isBoardInverted) || (!isWhite && m_settings.isBoardInverted))
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

Game::Tile Game::getCurrentPawn() const {
    if (m_current == Game::Player::White) return Game::Tile::WhitePawn;
    else if (m_current == Game::Player::Black) return Game::Tile::BlackPawn;
    else return Game::Tile::Blank;
}

Game::Tile Game::getCurrentQueen() const {
    if (m_current == Game::Player::White) return Game::Tile::WhiteQueen;
    else if (m_current == Game::Player::Black) return Game::Tile::BlackQueen;
    else return Game::Tile::Blank;
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
    if (m_current == Game::Player::White) m_whiteAmount -= static_cast<int>(result.takenPawns.size());
    else m_blackAmount -= static_cast<int>(result.takenPawns.size());

    // Check if someone has won
    if (m_whiteAmount == 0) result.winner = Game::Player::White;
    else if (m_blackAmount == 0) result.winner = Game::Player::Black;

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
        // to previous checks.
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
    if (getPawnColor(position) == m_current) {
        result.message = "Invalid move. When queen moves on further diagonal it can take at most "
                         "one pawn, which is right behind her final position.";
        return;
    }

    // Making sure there are no other pawns in the way of diagonal move
    while ((position = position - step) != from) {
        if (!isFree(position)) {
            result.message = "Invalid move. When queen moves on further diagonal it can take at most "
                             "one pawn, which is right behind her final position.";
            return;
        }
    }
    result.isCorrect = true;
    result.takenPawns = std::vector<std::pair<int, int>>{to - step};
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

void Game::processCapturePaths(PathTree & tree,
                               std::shared_ptr<PathTree::Leaf> const & start,
                               std::pair<int, int> const & position,
                               std::pair<int, int> const & target) const {
    auto positions = getClosestOpponentsPawns(position);
    std::cout << "Processing position " << position.first << " " << position.second <<  "...\n";
    for (auto const & p : positions) {

        // Else creating more branches out of this position using recursion
        std::cout << "Adding new leaf..\n";
        auto leaf = std::make_shared<PathTree::Leaf>(nullptr, p);
        tree.addLeaf(leaf);

        // If reached the proper end of the tree store it and create new branch from start position.
        if (p.first == target.first && p.second == target.second) {
            std::cout << "Adding new branch..\n";
            tree.addBranch(start);
            continue;
        }

        // If this position is taken by an opponent or a repetition of some previous positions then cut the branch
        if (!hasPosition(p) || !isFree(p) || !tree.isUnique(p)) {
            std::cout << "Killing new branch..\n";
            tree.killBranch();
            tree.addBranch(start);
            continue;
        }

        tree.addUnique(p);
        processCapturePaths(tree, leaf, p, target);
    }
}

std::vector<std::pair<int, int>> Game::processCapturingOpponentsPawns(std::pair<int, int> const & from,
                                                                std::pair<int, int> const & to) const {
    // Finding path, which user could have thought as a way
    // to jump move and take some of the opponent's pawns
    auto startLeaf = std::make_shared<PathTree::Leaf>(nullptr, to);
    auto tree = PathTree(startLeaf);
    processCapturePaths(tree, startLeaf, to, from);

    // If there are no results, the move is incorrect
    auto results = std::vector<std::pair<int, int>>();
    if (tree.getSize() == 0) return results;
    auto positions = tree.getTraceOf(0);
    std::cout << "Trace positions:\n";
    for (auto & pos : positions) std::cout << pos.first << " " << pos.second << '\n';

    // If move is correct the function should return the taken opponent's pawns as result
    auto directions = std::vector<std::pair<int, int>>();
    for (int i = 0; i < positions.size() - 1; i++) directions.push_back(positions[i + 1] - positions[i]);
    std::ranges::transform(directions, directions.begin(), [](auto d){ return d / 2; });
    for (int i = 0; i < directions.size(); i++) positions[i] += directions[i];
    positions.pop_back(); // last position does not hold taken pawn place here
    return positions;
}

void Game::reset() {
    // Init the board
    m_data.clear();
    for (auto & row : m_data) {
        for (auto &tile: *row) tile = Game::Tile::Blank;
    }
    m_current = Game::Player::White;
    m_blackAmount = 12;
    m_whiteAmount = 12;

    // Filling board
    auto totalAmount = m_data.size() * m_data.size();
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

Game::Player Game::getCurrentPlayer() const {
    return m_current;
}