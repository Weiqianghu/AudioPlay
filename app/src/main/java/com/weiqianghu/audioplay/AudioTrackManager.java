package com.weiqianghu.audioplay;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

public class AudioTrackManager {
    private AudioTrack audioTrack;
    private DataInputStream dataInputStream;
    private Thread recordThread;
    static volatile AudioTrackManager audioTrackManager = new AudioTrackManager();

    private static final int STREAM_TYPE = AudioManager.STREAM_MUSIC;
    private static final int SAMPLE_RATE = 44100;
    private static final int CHANNEL_CONFIG = AudioFormat.CHANNEL_OUT_STEREO;
    private static final int AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT;

    private int minBufferSize;
    private int mode = AudioTrack.MODE_STREAM;

    private boolean isRunning = false;

    public AudioTrackManager() {
        initData();
    }

    public void startPlay(String path) {
        try {
            setPath(path);
            startThread();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }

    private void initData() {
        minBufferSize = AudioTrack.getMinBufferSize(SAMPLE_RATE, CHANNEL_CONFIG, AUDIO_FORMAT);
        audioTrack = new AudioTrack(STREAM_TYPE, SAMPLE_RATE, CHANNEL_CONFIG, AUDIO_FORMAT, minBufferSize, mode);
    }

    private void destroyThread() {
        isRunning = false;
        if (recordThread != null && recordThread.getState() == Thread.State.RUNNABLE) {
            recordThread.interrupt();
            recordThread = null;
        }
    }

    private void startThread() {
        destroyThread();
        isRunning = true;
        if (recordThread == null) {
            recordThread = new Thread(recordRunnable);
            recordThread.start();
        }
    }

    private Runnable recordRunnable = new Runnable() {
        @Override
        public void run() {
            byte[] tempBuffer = new byte[minBufferSize];
            int readCount = 0;

            try {
                while (dataInputStream.available() > 0) {
                    readCount = dataInputStream.read(tempBuffer);
                    if (readCount == AudioTrack.ERROR_INVALID_OPERATION || readCount == AudioTrack.ERROR_BAD_VALUE) {
                        continue;
                    }
                    if (readCount != 0 && readCount != -1) {
                        if (audioTrack.getState() == AudioTrack.STATE_UNINITIALIZED) {
                            initData();
                        }
                        audioTrack.play();
                        audioTrack.write(tempBuffer, 0, readCount);
                    }
                }
                stopPlay();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    };

    private void setPath(String path) throws FileNotFoundException {
        File file = new File(path);
        dataInputStream = new DataInputStream(new FileInputStream(file));
    }

    private void stopPlay() {
        destroyThread();
        if (audioTrack != null) {
            if (audioTrack.getState() == AudioTrack.STATE_INITIALIZED) {
                audioTrack.stop();
            }
            audioTrack.release();
        }
        if (dataInputStream != null) {
            try {
                dataInputStream.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
