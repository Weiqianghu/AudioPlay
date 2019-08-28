#include <jni.h>
#include <string>
#include "OpenSLESPlayer.h"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "smallest", __VA_ARGS__)

OpenSLESPlayer *player = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_weiqianghu_audioplay_OpenSlAudioPlayer_start(JNIEnv *env, jobject thiz, jstring pcm_path) {
    LOGD("testPlayPCM");
    const char *filePath = env->GetStringUTFChars(pcm_path, 0);
    player = new OpenSLESPlayer();
    player->setDataSource(filePath);
    player->prepare();
    player->start();
    env->ReleaseStringUTFChars(pcm_path, filePath);
}extern "C"
JNIEXPORT void JNICALL
Java_com_weiqianghu_audioplay_OpenSlAudioPlayer_stop(JNIEnv *env, jobject thiz) {
    LOGD("stop");
    player->stop();
    player->release();
    delete (player);
    player = nullptr;
}