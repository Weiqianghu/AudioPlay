package com.weiqianghu.audioplay;

import androidx.appcompat.app.AppCompatActivity;

import android.media.AudioManager;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText("text ...");
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new OpenSlAudioPlayer().start("sdcard/test.pcm");
//                AudioTrackManager.audioTrackManager.startPlay("sdcard/test.pcm");
            }
        });
    }
}
