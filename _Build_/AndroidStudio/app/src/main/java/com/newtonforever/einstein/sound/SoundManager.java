package com.newtonforever.einstein.sound;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.ToneGenerator;
import android.util.Log;

import com.newtonforever.einstein.jni.Native;
import com.newtonforever.einstein.startup.StartupConstants;

public class SoundManager {

    private static final String TAG = SoundManager.class.toString();

    private ToneGenerator toneGenerator;
    private final AudioTrack audioTrack;
    private int volume;

    public SoundManager() {
        volume = 100;
        toneGenerator = new ToneGenerator(AudioManager.STREAM_MUSIC, volume);
        final int streamType = AudioManager.STREAM_MUSIC;
        final int sampleRateInHz = 22050;
        final int channelConfig = AudioFormat.CHANNEL_CONFIGURATION_MONO;
        final int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
        final int minBufferSize = StartupConstants.AUDIO_TRACK_MIN_BUFFER_SIZE;
        final int bufferMode = AudioTrack.MODE_STREAM;
        audioTrack = new AudioTrack(streamType, sampleRateInHz, channelConfig, audioFormat, minBufferSize, bufferMode);
        audioTrack.setStereoVolume(volume / 100.0f, volume / 100.0f);

        // Put AudioTrack into play mode (it won't play anything yet)
        audioTrack.play();
    }

    public void playSound(final int soundBufferSize) {
        if (0 == soundBufferSize) {
            return;
        }

        final short[] soundBuffer = new short[soundBufferSize];
        final int bytesReceived = Native.fillSoundBuffer(soundBuffer);
        final int samplesReceived = bytesReceived / 2;
        final int samplesWritten = audioTrack.write(soundBuffer, 0, samplesReceived);
        final int bytesWritten = 2 * samplesWritten;

        // AudioTrack does not start playing a sound until the buffer is half full.
        // This is a hack to flush the buffer.
        if (bytesWritten < StartupConstants.AUDIO_TRACK_MIN_BUFFER_SIZE) {
            final int tmpBufferSize =
                    (StartupConstants.AUDIO_TRACK_MIN_BUFFER_SIZE - bytesWritten) / 2;
            final short[] tmpBuffer = new short[tmpBufferSize];
            audioTrack.setStereoVolume(volume / 100.0f, volume / 100.0f);
            audioTrack.write(tmpBuffer, 0, tmpBufferSize);
            audioTrack.flush();
        }
    }

    public void stopSound() {
        toneGenerator.stopTone();
    }

    public void changeVolume() {
        volume = Native.getSoundVolume();
        Log.i(TAG, "changeVolume: Changed volume to " + volume);
    }
}

