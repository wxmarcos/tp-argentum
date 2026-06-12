#ifndef CLIENT_AUDIO_MANAGER_H
#define CLIENT_AUDIO_MANAGER_H

#include <map>
#include <memory>
#include <string>

#include "config/client_config.h"

struct Mix_Chunk;
struct _Mix_Music;
typedef struct _Mix_Music Mix_Music;

class AudioManager {
private:
    bool initialized;
    const ClientConfig& config;

    Mix_Music* music;
    std::map<std::string, Mix_Chunk*> effects;

public:
    explicit AudioManager(const ClientConfig& config);
    ~AudioManager();

    void load_effect(const std::string& key, const std::string& path);

    void play_effect(const std::string& key);

    void play_effect_at(const std::string& key, int distance_in_tiles);

    void play_music(const std::string& path);
    void stop_music();

    bool is_initialized() const;

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
};

#endif
