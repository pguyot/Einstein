package com.newtonforever.einstein.sound;

import java.util.Calendar;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.ToneGenerator;
import android.util.Log;

import com.newtonforever.einstein.jni.Native;
import com.newtonforever.einstein.startup.StartupConstants;
import com.newtonforever.einstein.utils.debug.DebugUtils;

public class SoundManager {

    private ToneGenerator m_toneGenerator;

    private final AudioTrack m_audioTrack;
    
    private int m_volume;

    public SoundManager() {
        super();
        m_volume = 100; //Native.getSoundVolume();
        this.m_toneGenerator = new ToneGenerator(AudioManager.STREAM_MUSIC, m_volume);
        final int streamType = AudioManager.STREAM_MUSIC;
        final int sampleRateInHz = 22050;
        final int channelConfig = AudioFormat.CHANNEL_CONFIGURATION_MONO;
        final int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
        final int minBufferSize = StartupConstants.AUDIO_TRACK_MIN_BUFFER_SIZE;
        final int bufferMode = AudioTrack.MODE_STREAM;
        this.m_audioTrack = new AudioTrack(streamType, sampleRateInHz, channelConfig, audioFormat, minBufferSize, bufferMode);
        this.m_audioTrack.setStereoVolume(m_volume/100.0f, m_volume/100.0f);
        this.m_audioTrack.play(); // Note that this only puts the AudioTrack into play mode. It won't play anything yet.
    }

    public void playSound(final int soundBufferSize) {
        if (0 == soundBufferSize) {
            return;
        }
        DebugUtils.logGreen("SoundManager: ", "Entering playSound()");
        DebugUtils.logGreen("SoundManager: ", "Sound buffer size: " + soundBufferSize);
        final short[] soundBuffer = new short[soundBufferSize];
        final int bytesReceived = Native.fillSoundBuffer(soundBuffer);
        final int samplesReceived = bytesReceived / 2;
        final int samplesWritten = m_audioTrack.write(soundBuffer, 0, samplesReceived);
        final int bytesWritten = 2 * samplesWritten;
        // AudioTrack does not start playing a sound until the buffer is half full. This is a hack to flush the buffer.
        if (bytesWritten < StartupConstants.AUDIO_TRACK_MIN_BUFFER_SIZE) {
            final int tmpBufferSize = (StartupConstants.AUDIO_TRACK_MIN_BUFFER_SIZE - bytesWritten) / 2;
            final short[] tmpBuffer = new short[tmpBufferSize];
            m_audioTrack.setStereoVolume(m_volume/100.0f, m_volume/100.0f);
            m_audioTrack.write(tmpBuffer, 0, tmpBufferSize);
            m_audioTrack.flush();
        }
        DebugUtils.logGreen("SoundManager: ", "Leaving playSound()");
    }

    public void stopSound() {
        DebugUtils.logRed("SoundManager: ", "Entering stopSound()");                
        this.m_toneGenerator.stopTone();
    }

    public void changeVolume() {
        m_volume = Native.getSoundVolume();
        Log.e("SoundManager.changeVolume", "Changed volume to " + m_volume);
    }
}

