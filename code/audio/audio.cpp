#include "audio/audio.h"
#include <cstdio>

bool InitializeAudio(PlatformAudio *audio) {
    ma_result result = ma_engine_init(NULL, &audio->engine);
    if (result != MA_SUCCESS) {
        printf("miniaudio error: %d\n", result);
        return false;
    }

    audio->initialized = true;
    return true;
}

void DestroyAudio(PlatformAudio *audio) {
    ma_engine_uninit(&audio->engine);
    audio->initialized = false;
}

AudioBus CreateAudioBus(PlatformAudio *audio) {
    int b = audio->busCount++;
    audio->busVolume[b] = 1.0f;
    audio->busMuted[b] = false;
    return b;
}

void SetAudioBusVolume(PlatformAudio *audio, AudioBus bus, float volume) {
    audio->busVolume[bus] = volume;
}

void SetAudioBusMuted(PlatformAudio *audio, AudioBus bus, bool muted) {
    audio->busMuted[bus] = muted;
}

bool LoadMusic(PlatformAudio* audio, const char* path, ma_sound* out, AudioBus bus) {
    if (ma_sound_init_from_file(&audio->engine, path,
                                MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_LOOPING,
                                NULL, NULL, out) != MA_SUCCESS) {
        return false;
    }

    return true;
}

void PlaySound(PlatformAudio *audio, ma_sound *sound, AudioBus bus) {
    //float v = Audio_GetEffectiveVolume(audio, bus, baseVol);
    ma_sound_set_volume(sound, 0.2f);
    ma_sound_start(sound);
}

void StopSound(ma_sound *sound) {
    ma_sound_stop(sound);
}

void FadeInSound(PlatformAudio *audio, ma_sound *sound, AudioBus bus, float ms) {
    ma_sound_set_volume(sound, 0.2f);
    ma_sound_set_stop_time_in_milliseconds(sound, ~(ma_uint64)0);
    ma_sound_set_fade_in_milliseconds(sound, 0.0f, 1.0f, 2000.0f);
    ma_sound_seek_to_pcm_frame(sound, 0);
    ma_sound_start(sound);
}

void FadeOutSound(ma_sound *sound, float ms) {
    ma_sound_stop_with_fade_in_milliseconds(sound, ms);
}
