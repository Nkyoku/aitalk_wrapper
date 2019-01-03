#pragma once

#include <stdint.h>

enum AIAudioResultCode {
    AIAUDIOERR_SUCCESS = 0,
    AIAUDIOERR_INTERNAL_ERROR = -1,
    AIAUDIOERR_UNSUPPORTED = -2,
    AIAUDIOERR_INVALID_ARGUMENT = -3,
    AIAUDIOERR_WAIT_TIMEOUT = -4,
    AIAUDIOERR_NOT_INITIALIZED = -10,
    AIAUDIOERR_NOT_OPENED = -11,
    AIAUDIOERR_ALREADY_OPENED = 11,
    AIAUDIOERR_NO_AUDIO_HARDWARE = -20,
    AIAUDIOERR_DEVICE_INVALIDATED = -21,
    AIAUDIOERR_INSUFFICIENT = -30,
    AIAUDIOERR_OUT_OF_MEMORY = -100,
    AIAUDIOERR_INVALID_CALL = -101,
    AIAUDIOERR_INVALID_EVENT_ORDINAL = -102,
    AIAUDIOERR_NO_NOTIFICATION_CALLBACK = 103,
    AIAUDIOERR_NO_PLAYING = 111,
    AIAUDIOERR_WRITE_FAULT = -201
};

enum AIAudioFormatType {
    AIAUIDOTYPE_NONE = 0,
    AIAUDIOTYPE_PCM_16,
    AIAUDIOTYPE_PCM_8 = 769,
    AIAUDIOTYPE_MULAW_8 = 7,
    AIAUDIOTYPE_ALAW_8 = 6
};

using AIAudioProcNotify = void(__stdcall *)(uint64_t tick, void *userData);

#pragma pack(push, 1)
struct AIAudio_TConfig {
    AIAudioProcNotify procNotify;
    uint32_t msecLatency;
    uint32_t lenBufferBytes;
    uint32_t hzSamplesPerSec;
    AIAudioFormatType formatTag;
    int32_t __reserved__;
    const char *descDevice;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct AIAudio_TWaveFormat {
    int32_t header;
    uint32_t hzSamplesPerSec;
    AIAudioFormatType formatTag;
};
#pragma pack(pop)
