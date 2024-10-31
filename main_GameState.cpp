//
// Created by Kuuba Puacz on 29/10/2024.
//

#include <iostream>
#include "main_GameState.h"
#include "jni.h"
#include "Game.h"
#include "util.h"

// Global default GameState
std::unique_ptr<Game> g_state;

JNIEXPORT void JNICALL Java_main_GameState_init__(JNIEnv * env, jobject self) {
    g_state = std::make_unique<Game>();
}

JNIEXPORT void JNICALL Java_main_GameState_init___3Lmain_GamePawnType_2Lmain_GamePlayerType_2(
        JNIEnv * env, jobject self, jobjectArray jState, jobject jCurrentPlayer) {
    auto currentPlayer = java::playerToCpp(env, jCurrentPlayer);
    auto jFlatState = java::readJavaArray(env, jState);
    auto flatState = std::vector<Game::Tile>(jFlatState.size());
    std::ranges::transform(jFlatState, flatState.begin(), [env](auto tile){ return java::tileToCpp(env, tile); });
    g_state = std::make_unique<Game>(flatState, currentPlayer);
}

JNIEXPORT jobject JNICALL Java_main_GameState_process(JNIEnv * env, jobject self,
                                                      jobject jFromPosition, jobject jToPosition) {
    auto fromPosition = java::positionToCpp(env, jFromPosition);
    auto toPosition = java::positionToCpp(env, jToPosition);
    auto results = g_state->process(fromPosition, toPosition);
    return java::resultsToJava(env, results);
}

JNIEXPORT jobject JNICALL Java_main_GameState_get(JNIEnv * env, jobject self, jobject jPosition) {
    auto position = java::positionToCpp(env, jPosition);
    if (!g_state->hasPosition(position)) return nullptr;
    auto tile = g_state->get(position);
    return java::tileToJava(env, tile);
}

JNIEXPORT void JNICALL Java_main_GameState_reset(JNIEnv * env, jobject self) {
    g_state = std::make_unique<Game>();
}

JNIEXPORT jobject JNICALL Java_main_GameState_getCurrentPlayer(JNIEnv * env, jobject self) {
    auto current = g_state->getCurrentPlayer();
    return java::playerToJava(env, current);
}

JNIEXPORT jint JNICALL Java_main_GameState_getWhitePawnsAmount(JNIEnv * env, jobject self) {
    return static_cast<jint>(g_state->getWhitePawnsAmount());
}

JNIEXPORT jint JNICALL Java_main_GameState_getBlackPawnsAmount(JNIEnv * env, jobject self) {
    return static_cast<jint>(g_state->getBlackPawnsAmount());
}