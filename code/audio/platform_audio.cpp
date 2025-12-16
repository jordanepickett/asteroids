#include "audio/audio.h"
#include "audio/audio_commands.h"
#include <cstdint>
#include <cstdio>

static void PlatformAudioPlay(PlatformAudio* audio, SoundLibrary* soundLibrary, void* buffer, size_t size) {
    uint8_t* ptr = (uint8_t*)buffer;
    uint8_t* end = ptr + size;
    while (ptr < end) {
        AudioCommandHeader* header = (AudioCommandHeader*)ptr;

        switch (header->type) {
            case AUDIO_CMD_PLAY_STREAM: {
                auto* c = (AudioCommandPlay*)ptr;
                switch(c->soundId) {
                    case SOUND_TITLE_MUSIC: {
                        PlaySound(audio, soundLibrary->titleMusic, *soundLibrary->musicBus);
                    } break;
                    case SOUND_AMBIENT_WIND: {
                        PlaySound(audio, soundLibrary->ambientWind, *soundLibrary->ambientBus);
                    } break;
                    default: {
                    } break;
                }
            } break;
            case AUDIO_CMD_PLAY: {
                auto* c = (AudioCommandPlay*)ptr;
                switch(c->soundId) {
                    case SOUND_LASER: {
                        SoundPoolPlay(soundLibrary->laser, 0.2f);
                    }
                    default: {
                    } break;
                }
            }
            default: {
            } break;
        }

        ptr += header->size; // move to next command
    }
}
