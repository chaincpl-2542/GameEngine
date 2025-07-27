#pragma once
#include "miniaudio.h"
#include <string>

namespace CPL 
{
    class SoundManager
    {
    public:
        static SoundManager& Get();

        void Init();
        void Destroy();
        void PlaySound(const std::string& filename);

        ma_engine* GetEngine() { return &engine; }

    private:
        SoundManager() = default;
        ~SoundManager() = default;
        SoundManager(const SoundManager&) = delete;
        SoundManager& operator=(const SoundManager&) = delete;

        ma_engine engine;
        bool initialized = false;
    };
}