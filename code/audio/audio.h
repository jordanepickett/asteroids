#pragma once
#include "miniaudio.h"

#define MAX_AUDIO_BUSES 8
#define MAX_POOL_INSTANCES 4

typedef int AudioBus;

typedef struct {
    ma_engine engine;
    bool initialized;
    float busVolume[MAX_AUDIO_BUSES];
    bool busMuted[MAX_AUDIO_BUSES];
    int busCount;
} PlatformAudio;

typedef struct {
    ma_sound instances[MAX_POOL_INSTANCES];
    AudioBus bus;
    bool used[MAX_POOL_INSTANCES];
    int count;

    bool running;
    ma_thread thread;
    PlatformAudio *engine;
} SoundPool;

typedef struct {
    ma_sound* titleMusic;
    ma_sound* ambientWind;
    SoundPool* laser;
    SoundPool* explosion;

    AudioBus* musicBus;
    AudioBus* ambientBus;
    AudioBus* sfxBus;
} SoundLibrary;

// PlatformAudio
bool InitializeAudio(PlatformAudio *audio);
void DestroyAudio(PlatformAudio *audio);

AudioBus CreateAudioBus(PlatformAudio *audio, float volume);
void SetAudioBusVolume(PlatformAudio* audio, AudioBus bus, float volume);
void SetAudioBusMuted(PlatformAudio* audio, AudioBus bus, bool muted);

bool LoadMusic(PlatformAudio* audio, const char* path, ma_sound* out, AudioBus bus); 

void PlaySound(PlatformAudio* audio, ma_sound* sound, AudioBus bus); 
void StopSound(ma_sound* sound);

void FadeInSound(PlatformAudio* audio, ma_sound* sound, AudioBus bus, float ms);
void FadeOutSound(ma_sound* sound, float ms);

bool InitializeSoundPool(PlatformAudio* audio, const char* path, int count, SoundPool* soundPool, AudioBus bus);
void SoundPoolPlay(SoundPool* pool, float volume);
