/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "Gamedata.h"
#include "../Helpers/UserDefaults.h"
#include "../Helpers/Utility.h"
#include "Constants.h"
#include "GameConfig.h"
#include "cocos2d.h"
#include "external/flatbuffers/util.h"

Gamedata::Gamedata()
  : _playerId(0)
  , _playerTotalCash(0)
  , _playerReviveCash(GAMECONFIG.getGameplayConfig().reviveStartCash)
  , _isAudioFxEnabled(true)
  , _isMusicEnabled(true)

{
    UserDefaults::getAudioFromUserdefaults(_isAudioFxEnabled, _isMusicEnabled);

    // create world and level vectors
    for (int i = 0; i < GAMECONFIG.getWorldCount(); ++i)
    {
        int worldUnlocked = i == 0 ? 1 : 0;  // unlock first world
        _unlockedWorldsConfig.push_back(worldUnlocked);
        std::vector<int> levelVector;
        std::vector<int> starVector;
        for (int j = 0; j < GAMECONFIG.getLevelCountForWorld(i); ++j)
        {
            int levelUnlocked = j == 0 ? 1 : 0;  // unlock first level of each world
            levelVector.push_back(levelUnlocked);

            // stars
            starVector.push_back(0);  // fill with 0
        }
        _unlockedLevelsConfigMap[i] = levelVector;

        _starMap[i] = starVector;
    }
}

void Gamedata::saveData()
{
    for (int i = 0; i < _unlockedLevelsConfigMap.size(); ++i)
    {
        auto levelVector = _unlockedLevelsConfigMap.at(i);
        cocos2d::Data levelData;
        levelData.copy((unsigned char*)levelVector.data(), levelVector.size() * sizeof(int));

        std::string key = CONSTANTS.userDefaultUnlockedLevel + Utility::numberToString(i);
        UserDefaults::writeDataToUserdefaults(key, levelData);
    }

    cocos2d::Data worldData;
    worldData.copy((unsigned char*)_unlockedWorldsConfig.data(), _unlockedWorldsConfig.size() * sizeof(int));
    UserDefaults::writeDataToUserdefaults(CONSTANTS.userDefaultUnlockedWorld, worldData);
}

void Gamedata::loadData()
{
    _playerReviveCash = UserDefaults::getReviveCashFromUserdefaults(_playerReviveCash);
    _playerTotalCash = UserDefaults::getTotalCashFromUserdefaults(_playerTotalCash);
    _playerId = UserDefaults::getPlayerIdFromUserdefaults(_playerId);

    _sharingRewards = UserDefaults::getSharingRewardsFromUserdefaults();

    for (int i = 0; i < _unlockedLevelsConfigMap.size(); ++i)
    {
        // default level data
        cocos2d::Data levelData;
        auto levelVector = _unlockedLevelsConfigMap.at(i);
        levelData.copy((unsigned char*)levelVector.data(), levelVector.size() * sizeof(int));

        std::string key = CONSTANTS.userDefaultUnlockedLevel + Utility::numberToString(i);
        levelData = UserDefaults::readDataFromUserdefaults(key, levelData);
        int* levelBuffer = (int*)levelData.getBytes();
        ssize_t levelBufferLength = levelData.getSize() / sizeof(int);

        for (int j = 0; j < levelBufferLength; j++)
        {
            levelVector.at(j) = (levelBuffer[j]);
        }
        _unlockedLevelsConfigMap[i] = levelVector;

        // stars
        // default star data
        cocos2d::Data starData;
        auto starVector = _starMap.at(i);
        starData.copy((unsigned char*)starVector.data(), starVector.size() * sizeof(int));

        key = CONSTANTS.userDefaultStarCount + Utility::numberToString(i);
        starData = UserDefaults::readDataFromUserdefaults(key, starData);
        int* starBuffer = (int*)starData.getBytes();
        ssize_t starBufferLength = starData.getSize() / sizeof(int);

        for (int j = 0; j < starBufferLength; j++)
        {
            starVector.at(j) = (starBuffer[j]);
        }
        _starMap[i] = starVector;
    }
    // default world data
    cocos2d::Data worldData;
    worldData.copy((unsigned char*)_unlockedWorldsConfig.data(), _unlockedWorldsConfig.size() * sizeof(int));

    worldData = UserDefaults::readDataFromUserdefaults(CONSTANTS.userDefaultUnlockedWorld, worldData);
    int* buffer = (int*)worldData.getBytes();
    ssize_t length = worldData.getSize() / sizeof(int);

    std::ostringstream ss;
    ss << std::setprecision(2) << std::fixed;
    for (int i = 0; i < length; i++)
    {
        _unlockedWorldsConfig.at(i) = buffer[i];
    }
}

// MARK: getter

bool Gamedata::getSharingReward(std::string key)
{
    if (_sharingRewards.count(key) > 0)
        return _sharingRewards[key.c_str()];

    return false;
}
bool Gamedata::isLevelUnlocked(int world, int level)
{
    return _unlockedLevelsConfigMap[world].at(level) == 1;
}

bool Gamedata::isWorldUnlocked(int world)
{
    return _unlockedWorldsConfig.at(world) == 1;
}

// MARK: setter

void Gamedata::unlockWorld(int world, int value)
{
    _unlockedWorldsConfig.at(world) = value;
    saveData();
}

void Gamedata::unlockLevel(int world, int level, int value)
{
    _unlockedLevelsConfigMap[world].at(level) = value;
    saveData();
}

void Gamedata::setPlayerTotalCash(int cash)
{
    _playerTotalCash = cash;
    UserDefaults::saveCashToUserdefaults(_playerTotalCash);
}

void Gamedata::setRevive()
{
    _playerTotalCash -= _playerReviveCash;
    _playerReviveCash = _playerReviveCash * GAMECONFIG.getGameplayConfig().reviveMultiply;
    UserDefaults::saveReviveCashToUserdefaults(_playerReviveCash);
}

void Gamedata::setPlayerId(int type)
{
    _playerId = type;
    UserDefaults::savePlayerToUserdefaults(_playerId);
}

// MARK: stars

void Gamedata::getStarsForLevel(int world, int level, int& money, int& enemies, int& heart)
{
    int value = _starMap[world].at(level);
    if (value >= 100)
    {
        money = 1;
        value -= 100;
    }
    if (value >= 10)
    {
        enemies = 1;
        value -= 10;
    }
    if (value >= 1)
    {
        heart = 1;
    }
}

void Gamedata::setStarMap(int world, int level, bool money, bool enemies, bool heart)
{
    int value = (money * 100) + (enemies * 10) + heart;
    _starMap[world].at(level) = value;

    auto starVector = _starMap.at(world);
    auto key = CONSTANTS.userDefaultStarCount + Utility::numberToString(world);

    cocos2d::Data data;
    data.copy((unsigned char*)starVector.data(), starVector.size() * sizeof(int));
    UserDefaults::writeDataToUserdefaults(key, data);
}
