#include "memory.h"
#include <cstring>
#include "audio/audio_commands.h"

void PushAudioPlay(MemoryArena* memory, AudioSound soundId, float volume) {
    size_t size = sizeof(AudioCommandPlay);
    auto* playCmd = (AudioCommandPlay*)ArenaAlloc(memory, size);
    playCmd->header.type = AUDIO_CMD_PLAY;
    playCmd->header.size = (uint32_t)size;
    playCmd->soundId = soundId;
    playCmd->volume = volume;
}

void PushAudioPlayStream(MemoryArena* memory, AudioSound soundId, float volume) {
    size_t size = sizeof(AudioCommandPlay);
    auto* playCmd = (AudioCommandPlay*)ArenaAlloc(memory, size);
    playCmd->header.type = AUDIO_CMD_PLAY_STREAM;
    playCmd->header.size = (uint32_t)size;
    playCmd->soundId = soundId;
    playCmd->volume = volume;
}
