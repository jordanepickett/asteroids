#include "audio/audio.h"
#include "memory.h"
#include <cstdint>

enum AudioCommandType {
    AUDIO_CMD_PLAY,
    AUDIO_CMD_SET_VOLUME,
    AUDIO_CMD_STOP,
    AUDIO_CMD_FADE_IN,
    AUDIO_CMD_FADE_OUT,
    AUDIO_CMD_PLAY_STREAM,
};

enum AudioSound {
    SOUND_TITLE_MUSIC,
    SOUND_AMBIENT_WIND,
    SOUND_LASER,
    SOUND_EXPLOSION
};

struct AudioCommandHeader {
    AudioCommandType type;
    uint32_t size;
};

struct AudioCommandPlay {
    AudioCommandHeader header;
    AudioSound soundId;
    float volume;
};

struct AudioCommandSetVolume {
    AudioCommandHeader header;
    AudioBus bus;
    float volume;
};

void PushAudioPlay(MemoryArena* memory, AudioSound soundId, float volume); 
void PushAudioPlayStream(MemoryArena* memory, AudioSound soundId, float volume); 
void PushAudioSetVolume(MemoryArena* memory, AudioBus bus, float volume);
