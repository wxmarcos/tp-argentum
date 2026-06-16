#include "audio/audio_manager.h"

#include <SDL2/SDL.h>
#include <SDL_mixer.h>

#include <algorithm>
#include <iostream>

AudioManager::AudioManager(const ClientConfig& config):
    initialized(false), config(config), music(nullptr) {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cerr << "[Audio] No se pudo iniciar el subsistema de audio: "
                  << SDL_GetError() << "\n";
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "[Audio] No se pudo abrir el dispositivo de audio: "
                  << Mix_GetError() << "\n";
        return;
    }

    Mix_VolumeMusic(config.music_volume);
    initialized = true;
}

AudioManager::~AudioManager() {
    if (!initialized) {
        return;
    }
    stop_music();
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
    for (auto& kv : effects) {
        if (kv.second) {
            Mix_FreeChunk(kv.second);
        }
    }
    effects.clear();
    Mix_CloseAudio();
}

void AudioManager::load_effect(const std::string& key,
                               const std::string& path) {
    if (!initialized) {
        return;
    }
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk) {
        std::cerr << "[Audio] No se pudo cargar el efecto '" << key
                  << "' desde " << path << ": " << Mix_GetError() << "\n";
        return;
    }
    effects[key] = chunk;
}

void AudioManager::play_effect(const std::string& key, int volume) {
    if (!initialized) {
        return;
    }
    auto it = effects.find(key);
    if (it == effects.end() || !it->second) {
        return;
    }
    Mix_VolumeChunk(it->second, volume < 0 ? config.effects_volume : volume);
    Mix_PlayChannel(-1, it->second, 0);
}

void AudioManager::play_effect_at(const std::string& key,
                                  int distance_in_tiles) {
    if (!initialized) {
        return;
    }
    auto it = effects.find(key);
    if (it == effects.end() || !it->second) {
        return;
    }

    const int max_audible_tiles = 15;
    int vol = config.effects_volume;
    if (distance_in_tiles > 0) {
        double factor =
            1.0 - static_cast<double>(distance_in_tiles) / max_audible_tiles;
        factor = std::max(0.0, factor);
        vol = static_cast<int>(config.effects_volume * factor);
    }
    Mix_VolumeChunk(it->second, vol);
    Mix_PlayChannel(-1, it->second, 0);
}

void AudioManager::play_music(const std::string& path) {
    if (!initialized) {
        return;
    }
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
    music = Mix_LoadMUS(path.c_str());
    if (!music) {
        std::cerr << "[Audio] No se pudo cargar la musica desde " << path
                  << ": " << Mix_GetError() << "\n";
        return;
    }
    Mix_PlayMusic(music, -1);
}

void AudioManager::stop_music() {
    if (!initialized) {
        return;
    }
    Mix_HaltMusic();
}

bool AudioManager::is_initialized() const { return initialized; }