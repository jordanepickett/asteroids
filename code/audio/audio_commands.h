#include <cstdint>

enum AudioCommandType {
    AUDIO_CMD_PLAY,
    AUDIO_CMD_SET_VOLUME,
    AUDIO_CMD_STOP,
    AUDIO_CMD_FADE_IN,
    AUDIO_CMD_FADE_OUT,
    AUDIO_CMD_PLAY_MUSIC,
};

enum AudioSound {
    TITLE_MUSIC,
    AMBIENT_WIND,
    LASER
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
