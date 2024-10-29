//
// Created by Kuuba Puacz on 25/10/2024.
//
#include <iostream>
#include "PathTree.h"

PathTree::Leaf::Leaf(std::shared_ptr<Leaf> const & p, std::pair<int, int> pos) : parent(p), position(pos) {}

PathTree::PathTree(std::shared_ptr<Leaf> const & leaf) {
    m_branches.push_back(leaf);
    m_uniques = std::set<std::pair<int, int>>();
    m_uniques.insert(leaf->position);
}

void PathTree::addBranch(const std::shared_ptr<Leaf> &leaf) {
    m_branches.push_back(leaf);
}

void PathTree::addLeaf(std::shared_ptr<Leaf> const & leaf) {
    // The leaf is lost but the reference still exists as it uses shared_ptr.
    // It will be cleaned up when its child gets erased, since it holds pointer to
    leaf->parent = m_branches.back();
    m_branches.back() = leaf;
}

void PathTree::killBranch() {
    m_branches.pop_back(); // It kills the top leaf of the branch causing a domino effect
}

bool PathTree::isUnique(std::pair<int, int> const & position) const {
    if (m_uniques.empty()) return true;
    return std::ranges::find(m_uniques, position) == m_uniques.end();
}

void PathTree::addUnique(const std::pair<int, int> &position) {
    m_uniques.insert(position);
}

std::vector<std::shared_ptr<PathTree::Leaf>>::size_type PathTree::getSize() const {
    return m_branches.size();
}

std::vector<std::pair<int, int>> PathTree::getTraceOf(int branch) const {
    auto path = std::vector<std::pair<int, int>>();
    auto top = m_branches[branch];
    while(top != nullptr) {
        path.push_back(top->position);
        top = top->parent;
    }
    if (!path.empty()) std::ranges::reverse(path);
    return path;
}
