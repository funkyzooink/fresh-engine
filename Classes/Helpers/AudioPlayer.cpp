/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "AudioPlayer.h"
#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"
#include "UserDefaults.h"

#include "AudioEngine.h"

int AudioPlayer::musicAudioID = -1;

void AudioPlayer::playFx(const std::string& audio)
{
    if (GAMEDATA.getAudioFxEnabled() && !audio.empty())
    {
        std::string filepath = GAMECONFIG.getAudio(audio) + CONSTANTS.audioFileType;
        cocos2d::AudioEngine::play2d(filepath.c_str());
    }
}

void AudioPlayer::playMenuMusic()
{
    playMusic("audio/music_menu");
}

void AudioPlayer::playMusicForWorld(int world)
{
    playMusic(GAMECONFIG.getMusicForWorld(world));
}

void AudioPlayer::stopMusic()
{
    if (GAMEDATA.getMusicEnabled())
    {
        cocos2d::AudioEngine::stop(musicAudioID);
    }
}

void AudioPlayer::pauseMusic()
{
    if (GAMEDATA.getMusicEnabled())
    {
        cocos2d::AudioEngine::pause(musicAudioID);
    }
}
void AudioPlayer::resumeMusic()
{
    if (GAMEDATA.getMusicEnabled())
    {
        cocos2d::AudioEngine::resume(musicAudioID);
    }
}

void AudioPlayer::playMusic(const std::string& audio)
{
    if (GAMEDATA.getMusicEnabled())
    {
        std::string filepath = audio + CONSTANTS.audioFileType;
        musicAudioID = cocos2d::AudioEngine::play2d(filepath.c_str(), true);
    }
}
