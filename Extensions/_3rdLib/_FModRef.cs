//--------------------------------------------------------------------
//
// This is a Unity behaviour script that demonstrates how to capture
// audio data from a VideoPlayer using Unity's AudioSampleProvider and
// play it back through an FMOD.Sound. This example uses the
// VideoPlayer's APIOnly output mode and can be used to get audio from
// a video when UnityAudio is disabled.
//
// For documentation on writing audio data to an FMOD.Sound. See
// https://fmod.com/docs/2.02/api/core-api-sound.html#sound_lock
//
// This document assumes familiarity with Unity scripting. See
// https://unity3d.com/learn/tutorials/topics/scripting for resources
// on learning Unity scripting.
//
//--------------------------------------------------------------------

// https://qa.fmod.com/t/is-unitys-built-in-audio-engine-fmod-studio-fmod-5/18911/10

using Unity.Collections;
using UnityEngine;
using UnityEngine.Experimental.Audio;
using UnityEngine.Experimental.Video;
using UnityEngine.Video;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System;

public class ScriptUsageVideoPlayback : MonoBehaviour
{
    private const int LATENCY_MS = 50;
    private const int DRIFT_MS = 1;
    private const float DRIFT_CORRECTION_PERCENTAGE = 0.5f;

    private AudioSampleProvider mProvider;

    private FMOD.CREATESOUNDEXINFO mExinfo;
    private FMOD.Channel mChannel;
    private FMOD.Sound mSound;

    private List<float> mBuffer = new List<float>();

    private int mSampleRate;
    private uint mDriftThreshold;
    private uint mTargetLatency;
    private uint mAdjustedLatency;
    private int mActualLatency;

    private uint mTotalSamplesWritten;
    private uint mMinimumSamplesWritten = uint.MaxValue;

    private uint mLastReadPosition;
    private uint mTotalSamplesRead;

    private void Start()
    {
        VideoPlayer vp = GetComponent<VideoPlayer>();
        vp.audioOutputMode = VideoAudioOutputMode.APIOnly;
        vp.prepareCompleted += Prepared;
        vp.loopPointReached += VideoEnded;
        vp.Prepare();

        mSampleRate = (int)(vp.GetAudioSampleRate(0) * vp.playbackSpeed);

        mDriftThreshold = (uint)(mSampleRate * DRIFT_MS) / 1000;
        mTargetLatency = (uint)(mSampleRate * LATENCY_MS) / 1000;
        mAdjustedLatency = mTargetLatency;
        mActualLatency = (int)mTargetLatency;

        mExinfo.cbsize = Marshal.SizeOf(typeof(FMOD.CREATESOUNDEXINFO));
        mExinfo.numchannels = vp.GetAudioChannelCount(0);
        mExinfo.defaultfrequency = mSampleRate;
        mExinfo.length = mTargetLatency * (uint)mExinfo.numchannels * sizeof(float);
        mExinfo.format = FMOD.SOUND_FORMAT.PCMFLOAT;

        FMODUnity.RuntimeManager.CoreSystem.createSound("", FMOD.MODE.LOOP_NORMAL | FMOD.MODE.OPENUSER, ref mExinfo, out mSound);
    }

    private void OnDestroy()
    {
        mChannel.stop();
        mSound.release();
    }

    private void VideoEnded(VideoPlayer vp)
    {
        if (!vp.isLooping)
        {
            mChannel.setPaused(true);
        }
    }

    private void Prepared(VideoPlayer vp)
    {
        mProvider = vp.GetAudioSampleProvider(0);
        mProvider.sampleFramesAvailable += SampleFramesAvailable;
        mProvider.enableSampleFramesAvailableEvents = true;
        mProvider.freeSampleFrameCountLowThreshold = mProvider.maxSampleFrameCount - mTargetLatency;
        vp.Play();
    }

    private void SampleFramesAvailable(AudioSampleProvider provider, uint sampleFrameCount)
    {
        using (NativeArray<float> buffer = new NativeArray<float>((int)sampleFrameCount * provider.channelCount, Allocator.Temp))
        {
            uint written = provider.ConsumeSampleFrames(buffer);
            mBuffer.AddRange(buffer);

            /*
             * Drift compensation
             * If we are behind our latency target, play a little faster
             * If we are ahead of our latency target, play a little slower
             */
            uint samplesWritten = (uint)buffer.Length;
            mTotalSamplesWritten += samplesWritten;

            if (samplesWritten != 0 && (samplesWritten < mMinimumSamplesWritten))
            {
                mMinimumSamplesWritten = samplesWritten;
                mAdjustedLatency = Math.Max(samplesWritten, mTargetLatency);
            }

            int latency = (int)mTotalSamplesWritten - (int)mTotalSamplesRead;
            mActualLatency = (int)((0.93f * mActualLatency) + (0.03f * latency));

            int playbackRate = mSampleRate;
            if (mActualLatency < (int)(mAdjustedLatency - mDriftThreshold))
            {
                playbackRate = mSampleRate - (int)(mSampleRate * (DRIFT_CORRECTION_PERCENTAGE / 100.0f));
            }
            else if (mActualLatency > (int)(mAdjustedLatency + mDriftThreshold))
            {
                playbackRate = mSampleRate + (int)(mSampleRate * (DRIFT_CORRECTION_PERCENTAGE / 100.0f));
            }
            mChannel.setFrequency(playbackRate);
        }
    }

    private void Update()
    {
        /*
         * Need to wait before playing to provide adequate space between read and write positions
         */
        if (!mChannel.hasHandle() && mTotalSamplesWritten >= mAdjustedLatency)
        {
            FMOD.ChannelGroup mMasterChannelGroup;
            FMODUnity.RuntimeManager.CoreSystem.getMasterChannelGroup(out mMasterChannelGroup);
            FMODUnity.RuntimeManager.CoreSystem.playSound(mSound, mMasterChannelGroup, false, out mChannel);
        }

        if (mBuffer.Count > 0 && mChannel.hasHandle())
        {
            uint readPosition;
            mChannel.getPosition(out readPosition, FMOD.TIMEUNIT.PCMBYTES);

            /*
             * Account for wrapping
             */
            uint bytesRead = readPosition - mLastReadPosition;
            if (readPosition < mLastReadPosition)
            {
                bytesRead += mExinfo.length;
            }

            if (bytesRead > 0 && mBuffer.Count >= bytesRead)
            {
                /*
                 * Fill previously read data with fresh samples
                 */
                IntPtr ptr1, ptr2;
                uint len1, len2;
                var res = mSound.@lock(mLastReadPosition, bytesRead, out ptr1, out ptr2, out len1, out len2);
                if (res != FMOD.RESULT.OK) Debug.LogError(res);

                // Though exinfo.format is float, data retrieved from Sound::lock is in bytes, therefore we only copy (len1+len2)/sizeof(float) full float values across
                int sampleLen1 = (int)(len1 / sizeof(float));
                int sampleLen2 = (int)(len2 / sizeof(float));
                int samplesRead = sampleLen1 + sampleLen2;
                float[] tmpBuffer = new float[samplesRead];

                mBuffer.CopyTo(0, tmpBuffer, 0, tmpBuffer.Length);
                mBuffer.RemoveRange(0, tmpBuffer.Length);

                if (len1 > 0)
                {
                    Marshal.Copy(tmpBuffer, 0, ptr1, sampleLen1);
                }
                if (len2 > 0)
                {
                    Marshal.Copy(tmpBuffer, sampleLen1, ptr2, sampleLen2);
                }

                res = mSound.unlock(ptr1, ptr2, len1, len2);
                if (res != FMOD.RESULT.OK) Debug.LogError(res);
                mLastReadPosition = readPosition;
                mTotalSamplesRead += (uint)samplesRead;
            }
        }
    }
}