#include "SoundManager.hpp"
#include <iostream>

namespace CPL
{
    SoundManager& SoundManager::Get()
    {
        static SoundManager instance;
        return instance;
    }

    void SoundManager::Init()
    {
        if (initialized) return;
        if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
            std::cerr << "Failed to initialize audio engine." << std::endl;
            return;
        }
        initialized = true;
    }

    void SoundManager::Destroy()
    {
        if (!initialized) return;
        ma_engine_uninit(&engine);
        initialized = false;
    }

    void SoundManager::PlaySound(const std::string& filename)
    {
        if (!initialized) return;
        ma_engine_play_sound(&engine, filename.c_str(), NULL);
    }
}