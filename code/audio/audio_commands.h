#include <cstdint>

enum AudioCommandType {
    AUDIO_CMD_PLAY,
    AUDIO_CMD_PLAY_3D,
    AUDIO_CMD_SET_VOLUME,
    AUDIO_CMD_STOP,
    AUDIO_CMD_FADE,
    AUDIO_CMD_PLAY_MUSIC,
};


struct AudioCommandHeader {
    AudioCommandType type;
    uint32_t size;
};

struct AudioCommandPlay {
    AudioCommandHeader header;
    uint32_t soundId;
    float volume;
};
