//
// Created by Kuuba Puacz on 25/10/2024.
//

#ifndef UTP_GAME_PROJECT_LOGIC_PATHTREE_H
#define UTP_GAME_PROJECT_LOGIC_PATHTREE_H

#include <memory>
#include <set>
#include <vector>

// Used for finding the path of capturing multiple pawns in one move.
// Inserting the leaves goes from left to right.
class PathTree {

public:

    struct Leaf {
        std::shared_ptr<Leaf> parent;
        std::pair<int, int> position;
        Leaf(std::shared_ptr<Leaf> const & p, std::pair<int, int> pos);
    };

    PathTree(std::shared_ptr<Leaf> const & origin);
    void addLeaf(std::shared_ptr<Leaf> const & leaf);
    void addBranch(std::shared_ptr<Leaf> const & leaf);
    void killBranch();
    void addUnique(std::pair<int, int> const & position);

    std::vector<std::pair<int, int>> getTraceOf(int branch) const;
    std::vector<std::shared_ptr<Leaf>>::size_type getSize() const;
    bool isUnique(std::pair<int, int> const & position) const;

private:
    std::vector<std::shared_ptr<Leaf>> m_branches;
    std::set<std::pair<int, int>> m_uniques;

};


#endif //UTP_GAME_PROJECT_LOGIC_PATHTREE_H
