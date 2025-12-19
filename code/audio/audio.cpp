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

AudioBus CreateAudioBus(PlatformAudio *audio, float volume) {
    int b = audio->busCount++;
    audio->busVolume[b] = volume;
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
    ma_sound_set_volume(sound, audio->busVolume[bus]);
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

bool InitializeSoundPool(PlatformAudio* audio, const char* path, int count, SoundPool* soundPool, AudioBus bus) { 

    if (count > MAX_POOL_INSTANCES) count = MAX_POOL_INSTANCES;

    soundPool->count = count;
    soundPool->bus = bus;
    soundPool->engine = audio;

    for (int i = 0; i < count; i++) {
        soundPool->used[i] = false;
        if (ma_sound_init_from_file(&audio->engine, path,
                                    MA_SOUND_FLAG_DECODE,
                                    NULL, NULL,
                                    &soundPool->instances[i]) != MA_SUCCESS) {
            return false;
        }
    }

    soundPool->running = true;
    //ma_thread_init(NULL, SoundPool_Thread, pool, &pool->thread);

    return true;
}

void SoundPoolPlay(SoundPool *pool, float volume) {
    for (int i = 0; i < pool->count; i++) {
        if(pool->used[i] && ma_sound_at_end(&pool->instances[i])) {
            pool->used[i] = false;
        }

        if (!pool->used[i]) {
            pool->used[i] = true;

            //float v = Audio_GetEffectiveVolume(pool->engine, pool->bus, pool->baseVolume * vol);
            ma_sound_seek_to_pcm_frame(&pool->instances[i], 0);
            ma_sound_set_volume(&pool->instances[i], volume);

            ma_sound_start(&pool->instances[i]);
            return;
        }
    }

    // fallback: steal instance 0
    pool->used[0] = true;
    //float v = Audio_GetEffectiveVolume(pool->engine, pool->bus, pool->baseVolume * vol);
    ma_sound_set_volume(&pool->instances[0], volume);
    ma_sound_start(&pool->instances[0]);
}
