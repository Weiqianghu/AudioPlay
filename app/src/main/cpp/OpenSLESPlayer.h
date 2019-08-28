//
// Created by weiqianghu on 2019/8/28.
//

#ifndef AUDIOPLAY_OPENSLESPLAYER_H
#define AUDIOPLAY_OPENSLESPLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>
#include <string>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "smallest", __VA_ARGS__)

class OpenSLESPlayer {
public:
    OpenSLESPlayer();

    int32_t setDataSource(const std::string &pcmPath);

    int32_t prepare();

    void start();

    void stop();

    void release();

private:
    SLresult createEngine();

    SLObjectItf engineObject;
    SLEngineItf engineItf;

    SLObjectItf outputMixObject;
    SLObjectItf playerObject;
    SLPlayItf playItf;
    SLVolumeItf playerVolume;
};


#endif //AUDIOPLAY_OPENSLESPLAYER_H
