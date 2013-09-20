package com.newtonforever.einstein.sound;

import android.media.AudioManager;
import android.media.ToneGenerator;
import android.util.Log;

import com.newtonforever.einstein.jni.Native;

public class SoundManager {

    private final ToneGenerator m_toneGenerator = new ToneGenerator(AudioManager.STREAM_MUSIC, 100);
   
    public SoundManager() {
        super();
    }

    public void playSound() {
        final int bufferSize = Native.getSoundBufferSize();
        Log.e("SoundManager.playSound", "BufferSize is " + bufferSize);
        final short[] soundBuffer = new short[bufferSize];
        Native.fillSoundBuffer(soundBuffer);
        // TODO FG For now we will only play a short beep
        this.m_toneGenerator.startTone(ToneGenerator.TONE_DTMF_0, 100);
        Native.soundBufferFinishedOrCanceled();
    }

    public void stopSound() {
        Log.e("SoundManager.stopSound", "");
        this.m_toneGenerator.stopTone();
    }

    public void changeVolume() {
        final int volume = Native.getSoundVolume();
        Log.e("SoundManager.changeVolume", "Changed volume to " + volume);
    }

}
