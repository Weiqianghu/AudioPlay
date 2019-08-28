//
// Created by weiqianghu on 2019/8/28.
//

#include "OpenSLESPlayer.h"

FILE *pcmFile;
void *buffer;
uint8_t *out_buffer;
SLAndroidSimpleBufferQueueItf simpleBufferQueue;

int32_t getPcmData(void **pcm, FILE *pcmFile) {
    while (!feof(pcmFile)) {
        size_t size = fread(out_buffer, 1, 44100 * 2 * 2, pcmFile);
        *pcm = out_buffer;
        return size;
    }
    return 0;
}

OpenSLESPlayer::OpenSLESPlayer() {
    engineObject = nullptr;
    engineItf = nullptr;
    outputMixObject = nullptr;
    playerObject = nullptr;
    playItf = nullptr;
    playerVolume = nullptr;
}

void pcmBufferCallback(SLAndroidSimpleBufferQueueItf bf, void *context) {
    int32_t size = getPcmData(&buffer, pcmFile);
    LOGD("pcmBufferCallBack, size=%d", size);
    if (nullptr != buffer && size > 0) {
        (*simpleBufferQueue)->Enqueue(simpleBufferQueue, buffer, static_cast<SLuint32>(size));
    }
}

SLresult OpenSLESPlayer::createEngine() {
    LOGD("createEngine()");
    SLresult result = slCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("slCreateEngine failed, result=%d", result);
        return result;
    }

    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("engineObject Realize failed, result=%d", result);
        return result;
    }

    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineItf);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("engineObject GetInterface failed, result=%d", result);
        return result;
    }
    return result;
}

int32_t OpenSLESPlayer::setDataSource(const std::string &pcmPath) {
    pcmFile = fopen(pcmPath.c_str(), "r");
    if (pcmFile == NULL) {
        LOGD("open path=%s failed.", pcmPath.c_str());
        return -1;
    }
    return 0;
}

int32_t OpenSLESPlayer::prepare() {
    out_buffer = (uint8_t *) malloc(44100 * 2 * 2);
    SLresult result = createEngine();
    if (result != SL_RESULT_SUCCESS) {
        LOGD("createEngine() failed.");
        return result;
    }

    SLDataLocator_AndroidSimpleBufferQueue androidSimpleBufferQueue =
            {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM slDataFormatPcm = {
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource slDataSource = {&androidSimpleBufferQueue, &slDataFormatPcm};

    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineItf)->CreateOutputMix(engineItf, &outputMixObject, 1, mids, mreq);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("CreateOutputMix failed, ret=%d", result);
        return result;
    }
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Realize failed, result=%d", result);
        return result;
    }
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSink = {&outputMix, nullptr};

    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    result = (*engineItf)->CreateAudioPlayer(engineItf, &playerObject, &slDataSource, &audioSink, 1,
                                             ids, req);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("CreateAudioPlayer() failed.");
        return result;
    }
    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("playerObject Realize() failed.");
        return result;
    }
    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playItf);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("playerObject GetInterface(SL_IID_PLAY) failed.");
        return result;
    }
    result = (*playerObject)->GetInterface(playerObject, SL_IID_BUFFERQUEUE, &simpleBufferQueue);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("playerObject GetInterface(SL_IID_BUFFERQUEUE) failed.");
        return result;
    }

    result = (*simpleBufferQueue)->RegisterCallback(simpleBufferQueue, pcmBufferCallback, this);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("SLAndroidSimpleBufferQueueItf RegisterCallback() failed.");
        return result;
    }
    return result;
}

void OpenSLESPlayer::start() {
    (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_PLAYING);
    pcmBufferCallback(simpleBufferQueue, this);
}

void OpenSLESPlayer::stop() {
    LOGD("stop");
    (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_STOPPED);
}

void OpenSLESPlayer::release() {
    if (playerObject != nullptr) {
        (*playerObject)->Destroy(playerObject);
        playerObject = nullptr;
        playItf = nullptr;
        playerVolume = nullptr;
        simpleBufferQueue = nullptr;
        pcmFile = nullptr;
        buffer = nullptr;
        free(out_buffer);
        out_buffer = nullptr;
    }
    if (outputMixObject != nullptr) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = nullptr;
    }

    if (engineObject != nullptr) {
        (*engineObject)->Destroy(engineObject);
        engineObject = nullptr;
        engineItf = nullptr;
    }
}
