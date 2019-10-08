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

#include "SimpleAudioEngine.h"

void AudioPlayer::preload()
{
    for (auto& item : GAMECONFIG.getAudioMap())
    {
        std::string filepath = item.second + CONSTANTS.audioFileType;
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(filepath.c_str());
    }
    std::string musicFilepath = "audio/music_menu" + CONSTANTS.audioFileType;
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(musicFilepath.c_str());
}

void AudioPlayer::playFx(const std::string& audio)
{
    if (GAMEDATA.getAudioFxEnabled() && !audio.empty())
    {
        std::string filepath = GAMECONFIG.getAudio(audio) + CONSTANTS.audioFileType;
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(filepath.c_str());
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
        CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic(false);
    }
}

void AudioPlayer::pauseMusic()
{
    if (GAMEDATA.getMusicEnabled())
    {
        CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    }
}
void AudioPlayer::resumeMusic()
{
    if (GAMEDATA.getMusicEnabled())
    {
        CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    }
}

void AudioPlayer::playMusic(const std::string& audio)
{
    if (GAMEDATA.getMusicEnabled())
    {
        CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(1.0F);
        std::string filename = audio + CONSTANTS.audioFileType;
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(filename.c_str(), true);
    }
}
