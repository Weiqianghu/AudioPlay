package com.weiqianghu.audioplay;

public class OpenSlAudioPlayer {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public native void start(String pcmPath);

    public native void stop();
}
