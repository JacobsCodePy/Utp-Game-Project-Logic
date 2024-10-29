/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class GameState */

#ifndef _Included_GameState
#define _Included_GameState
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     GameState
 * Method:    init
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_GameState_init
  (JNIEnv *, jobject, jint);

/*
 * Class:     GameState
 * Method:    process
 * Signature: (LGamePosition;LGamePosition;)LGameMoveResult;
 */
JNIEXPORT jobject JNICALL Java_GameState_process
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     GameState
 * Method:    get
 * Signature: (LGamePosition;)LGamePawnType;
 */
JNIEXPORT jobject JNICALL Java_GameState_get
  (JNIEnv *, jobject, jobject);

/*
 * Class:     GameState
 * Method:    reset
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_GameState_reset
  (JNIEnv *, jobject);

/*
 * Class:     GameState
 * Method:    getCurrentPlayer
 * Signature: ()LGamePlayerType;
 */
JNIEXPORT jobject JNICALL Java_GameState_getCurrentPlayer
  (JNIEnv *, jobject);

/*
 * Class:     GameState
 * Method:    initFromState
 * Signature: ([LGamePawnType;LGamePlayerType;)V
 */
JNIEXPORT void JNICALL Java_GameState_initFromState
  (JNIEnv *, jobject, jobjectArray, jobject);

#ifdef __cplusplus
}
#endif
#endif
