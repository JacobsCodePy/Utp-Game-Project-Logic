//
// Created by Kuuba Puacz on 22/10/2024.
//
#include <iostream>
#include "util.h"

namespace java {

    ///////////////////////////////////////////////////////
    /// Global mappings
    ///////////////////////////////////////////////////////


    std::map<Game::Tile, std::string> const g_mappingTile = {
            {Game::Tile::WhitePawn, "WhitePawn"},
            {Game::Tile::BlackPawn, "BlackPawn"},
            {Game::Tile::WhiteQueen, "WhiteQueen"},
            {Game::Tile::BlackQueen, "BlackQueen"},
            {Game::Tile::Blank, "Blank"},
    };
    std::map<Game::Player, std::string> const g_mappingPlayer = {
            {Game::Player::White, "White"},
            {Game::Player::Black, "Black"},
            {Game::Player::None, "None"},
    };


    ///////////////////////////////////////////////////////
    /// Conversion from Java to C++
    ///////////////////////////////////////////////////////


    Game::Player playerToCpp(JNIEnv *env, jobject const &player) {
        auto cls = env->GetObjectClass(player);
        auto method = env->GetMethodID(cls, "name", "()Ljava/lang/String;");
        auto nameRaw = (jstring) env->CallObjectMethod(player, method);
        auto name = std::string(env->GetStringUTFChars(nameRaw, nullptr));

        auto mapping = g_mappingPlayer;
        auto it = std::find_if(mapping.begin(), mapping.end(), [name](auto pair){ return pair.second == name; });
        return it->first;
    }

    std::pair<int, int> positionToCpp(JNIEnv *env, jobject const &pos) {
        auto cls = env->GetObjectClass(pos);
        auto row = env->GetFieldID(cls, "row", "I");
        auto col = env->GetFieldID(cls, "col", "I");
        return {env->GetIntField(pos, row), env->GetIntField(pos, col)};
    }

    Game::Tile tileToCpp(JNIEnv *env, jobject const &tile) {
        auto cls = env->GetObjectClass(tile);
        auto method = env->GetMethodID(cls, "name", "()Ljava/lang/String;");
        auto nameRaw = (jstring) env->CallObjectMethod(tile, method);
        auto name = std::string(env->GetStringUTFChars(nameRaw, nullptr));

        auto mapping = g_mappingTile;
        auto it = std::find_if(mapping.begin(), mapping.end(), [name](auto pair){ return pair.second == name; });
        return it->first;
    }


    ///////////////////////////////////////////////////////
    /// Conversions from C++ to Java
    ///////////////////////////////////////////////////////


    jobject positionToJava(JNIEnv *env, std::pair<int, int> const &pos) {
        auto cls = env->FindClass("main/GamePosition");
        auto constructor = env->GetMethodID(cls, "<init>", "(II)V");
        auto object = env->NewObject(cls, constructor, pos.first, pos.second);
        return object;
    }

    jobject tileToJava(JNIEnv *env, Game::Tile const &tile) {
        auto cls = env->FindClass("main/GamePawnType");
        auto field = env->GetStaticFieldID(cls, g_mappingTile.at(tile).c_str(), "Lmain/GamePawnType;");
        return env->GetStaticObjectField(cls, field);

    }

    jobject playerToJava(JNIEnv *env, Game::Player const &player) {
        auto cls = env->FindClass("main/GamePlayerType");
        auto field = env->GetStaticFieldID(cls, g_mappingPlayer.at(player).c_str(), "Lmain/GamePlayerType;");
        return env->GetStaticObjectField(cls, field);
    }

    jobject resultsToJava(JNIEnv * env, Game::MoveResult const &results) {
        auto cls = env->FindClass("main/GameMoveResult");
        auto constructor = env->GetMethodID(cls, "<init>", "(ZZ[Lmain/GamePosition;Lmain/GamePlayerType;Ljava/lang/String;)V");
        auto array = env->NewObjectArray(static_cast<jsize>(results.takenPawns.size()),
                                         env->FindClass(("main/GamePosition")), nullptr);
        for (auto i = 0; i < results.takenPawns.size(); i++)
            env->SetObjectArrayElement(array, i, positionToJava(env, results.takenPawns[i]));
        return env->NewObject(
             cls, constructor,
             results.isCorrect ? JNI_TRUE : JNI_FALSE,
             results.isQueen ? JNI_TRUE : JNI_FALSE,
             array,
             playerToJava(env, results.winner),
             env->NewStringUTF(results.message.c_str())
        );
    }

    std::vector<jobject> readJavaArray(JNIEnv * env, jobjectArray const &array) {
        auto objects = std::vector<jobject>();
        for (auto i = 0; i < env->GetArrayLength(array); i++) {
            objects.push_back(env->GetObjectArrayElement(array, i));
        }
        return objects;
    }

}