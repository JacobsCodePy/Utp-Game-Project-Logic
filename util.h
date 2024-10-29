//
// Created by Kuuba Puacz on 22/10/2024.
//

#ifndef UTP_GAME_PROJECT_LOGIC_UTIL_H
#define UTP_GAME_PROJECT_LOGIC_UTIL_H

#include "Game.h"
#include <vector>
#include <map>
#include <jni.h>

namespace java {

    // Global Java integration mapping
    extern const std::map<Game::Tile, std::string> g_mappingTile;
    extern const std::map<Game::Player, std::string> g_mappingPlayer;

    std::pair<int, int> positionToCpp(JNIEnv *env, jobject const &pos);
    Game::Player playerToCpp(JNIEnv *env, jobject const &player);

    jobject positionToJava(JNIEnv * env, std::pair<int, int> const &pos);
    jobject tileToJava(JNIEnv *env, Game::Tile const &tile);
    jobject playerToJava(JNIEnv *env, Game::Player const & player);
    jobject resultsToJava(JNIEnv * env, Game::MoveResult const &results);

}

#endif //UTP_GAME_PROJECT_LOGIC_UTIL_H
