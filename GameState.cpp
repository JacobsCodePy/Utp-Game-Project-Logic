
#include <iostream>
#include "GameState.h"
#include "jni.h"
#include "Game.h"
#include "util.h"

// Global default GameState
auto g_settings = Game::Settings();
std::unique_ptr<Game> g_state;

JNIEXPORT void JNICALL Java_GameState_init(JNIEnv * env, jobject self, jint size) {
    g_state = std::make_unique<Game>(g_settings, size);
}

JNIEXPORT jobject JNICALL Java_GameState_get(JNIEnv * env, jobject self, jobject jPosition) {
    auto position = java::positionToCpp(env, jPosition);
    if (!g_state->hasPosition(position)) return nullptr;
    auto tile = g_state->get(position);
    return java::tileToJava(env, tile);
}

JNIEXPORT jobject JNICALL Java_GameState_process(JNIEnv * env, jobject self,
                                                 jobject jFromPosition, jobject jToPosition) {
    auto fromPosition = java::positionToCpp(env, jFromPosition);
    auto toPosition = java::positionToCpp(env, jToPosition);
    auto results = g_state->process(fromPosition, toPosition);
    return java::resultsToJava(env, results);
}

JNIEXPORT void JNICALL Java_GameState_reset(JNIEnv * env, jobject self) {
    g_state.reset();
}

JNIEXPORT jobject JNICALL Java_GameState_getCurrentPlayer(JNIEnv * env, jobject self) {
    auto current = g_state->getCurrentPlayer();
    return java::playerToJava(env, current);
}

JNIEXPORT void JNICALL Java_GameState_initFromState(JNIEnv * env, jobject self, jobjectArray jState,
                                                    jobject jCurrentPlayer) {
    auto currentPlayer = java::playerToCpp(env, jCurrentPlayer);

}