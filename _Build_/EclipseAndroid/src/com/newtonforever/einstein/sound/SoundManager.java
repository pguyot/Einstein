package com.newtonforever.einstein.sound;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
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
        int bytesReceived = Native.fillSoundBuffer(soundBuffer);
        Log.e("SoundManager.playSound", "Received " + bytesReceived + " bytes");
        // TODO FG For now we will only play a short beep
        //this.m_toneGenerator.startTone(ToneGenerator.TONE_DTMF_0, 100);
        
        if (audioTrack==null) {
        	deviceBufferSize = android.media.AudioTrack.getMinBufferSize(
                    22050, AudioFormat.CHANNEL_CONFIGURATION_MONO,
                    AudioFormat.ENCODING_PCM_16BIT);
            audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
                    22050, AudioFormat.CHANNEL_CONFIGURATION_MONO,
                    AudioFormat.ENCODING_PCM_16BIT, deviceBufferSize,
                    AudioTrack.MODE_STREAM);
            audioTrack.play();
        }
        int samplesRemaining = bytesReceived/2;
        int samplesStart = 0;
        int bytesInBuffer = 0;
        while (samplesRemaining>0) {
        	int samplesWritten = audioTrack.write(soundBuffer, samplesStart, samplesRemaining);
        	bytesInBuffer += 2*samplesWritten;
        	Log.e("SoundManager.playSound", "Wrote " + samplesWritten + " bytes");
        	samplesStart += samplesWritten;
        	samplesRemaining -= samplesWritten;
        }
        // AudioTrack does not start playing a sound until the buffer is half full. This is a hack to flush the buffer.
        if (bytesInBuffer<deviceBufferSize) {
        	int fill = deviceBufferSize - bytesInBuffer;
        	final short[] tmpBuffer = new short[fill/2];
        	audioTrack.write(tmpBuffer, 0, fill/2);
        	audioTrack.flush();
        }
        
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

    AudioTrack audioTrack = null;
    int deviceBufferSize = 0;
}

