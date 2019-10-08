//
//  UserDefaults.cpp
//  fresh engine
//
//  Created by Gabriel Heilig on 08.05.18.
//
#include "UserDefaults.h"

#include "cocos2d.h"

#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"
#include "Utility.h"

void UserDefaults::writeDataToUserdefaults(std::string key, cocos2d::Data data)
{
    cocos2d::UserDefault::getInstance()->setDataForKey(key.c_str(), data);
}

cocos2d::Data UserDefaults::readDataFromUserdefaults(std::string key, cocos2d::Data defaultData)
{
    return cocos2d::UserDefault::getInstance()->getDataForKey(key.c_str(), defaultData);
}

void UserDefaults::savePlayerToUserdefaults(int type)
{
    cocos2d::UserDefault::getInstance()->setIntegerForKey(CONSTANTS.userDefaultPlayerId.c_str(), type);
}

void UserDefaults::saveSharingRewardsToUserdefaults(std::string key)
{
    std::string computedKey = CONSTANTS.userDefaultRewards + key;
    cocos2d::UserDefault::getInstance()->setBoolForKey(computedKey.c_str(), true);
}

void UserDefaults::saveCashToUserdefaults(int cash)
{
    cocos2d::UserDefault::getInstance()->setIntegerForKey(CONSTANTS.userDefaultPlayerCash.c_str(), cash);
}

void UserDefaults::saveReviveCashToUserdefaults(int cash)
{
    cocos2d::UserDefault::getInstance()->setIntegerForKey(CONSTANTS.userDefaultPlayerReviveCash.c_str(), cash);
}

void UserDefaults::saveAudioFxToUserdefaults(bool enabled)
{
    cocos2d::UserDefault::getInstance()->setBoolForKey(CONSTANTS.userDefaultSoundFx.c_str(), enabled);
}
void UserDefaults::saveMusicFxToUserdefaults(bool enabled)
{
    cocos2d::UserDefault::getInstance()->setBoolForKey(CONSTANTS.userDefaultSoundMusic.c_str(), enabled);
}
// MARK: read
void UserDefaults::getAudioFromUserdefaults(bool& isAudioFxEnabled, bool& isMusicEnabled)
{
    isAudioFxEnabled =
        cocos2d::UserDefault::getInstance()->getBoolForKey(CONSTANTS.userDefaultSoundFx.c_str(), isAudioFxEnabled);
    isMusicEnabled =
        cocos2d::UserDefault::getInstance()->getBoolForKey(CONSTANTS.userDefaultSoundMusic.c_str(), isMusicEnabled);
}

std::map<std::string, bool> UserDefaults::getSharingRewardsFromUserdefaults()
{
    std::map<std::string, bool> ratings;
    auto sharingConfig = GAMECONFIG.getSharingconfig();
    for (auto& sharing : sharingConfig)
    {
        auto sharingTitle = sharing.title;
        std::string key = CONSTANTS.userDefaultRewards + sharingTitle;
        auto rating = cocos2d::UserDefault::getInstance()->getBoolForKey(key.c_str(), false);
        ratings[sharingTitle] = rating;
    }

    return ratings;
}

int UserDefaults::getReviveCashFromUserdefaults(int defaultValue)
{
    return cocos2d::UserDefault::getInstance()->getIntegerForKey(CONSTANTS.userDefaultPlayerReviveCash.c_str(),
                                                                 defaultValue);
}
int UserDefaults::getTotalCashFromUserdefaults(int defaultValue)
{
    return cocos2d::UserDefault::getInstance()->getIntegerForKey(CONSTANTS.userDefaultPlayerCash.c_str(), defaultValue);
}
int UserDefaults::getPlayerIdFromUserdefaults(int defaultValue)
{
    return cocos2d::UserDefault::getInstance()->getIntegerForKey(CONSTANTS.userDefaultPlayerId.c_str(), defaultValue);
}

void UserDefaults::reset()
{
    cocos2d::UserDefault::getInstance()->destroyInstance();
    //    auto strFilePath = CCFileUtils::getWriteablePath() + XML_FILE_NAME;
    //    remove(strFilePath.c_str());
}
