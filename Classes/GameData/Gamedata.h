/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef GAMEDATA_GAMEDATA_H_
#define GAMEDATA_GAMEDATA_H_

#include <map>
#include <vector>
#include "platform/CCPlatformMacros.h"

#define GAMEDATA Gamedata::getInstance()

struct LevelSaveConfig
{
    int id;
    int unlocked;
    int starMoney;
    int starEnemy;
    int starLifeForce;
};

struct WorldSaveConfig
{
    int id;
    int unlocked;
    std::vector<LevelSaveConfig> levels;
};
/**
 * Gamedata preapres data before and after reading and writing to UserDefaults
 *
 */
class Gamedata
{
  public:
    static Gamedata& getInstance()
    {
        static Gamedata instance;  // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    void saveData();
    void loadData();

    /**
     * check if level in world is unlocked
     *
     * @param world, level
     *
     */
    bool isLevelUnlocked(int world, int level);

    /**
     * check if world is unlocked
     *
     * @param world
     *
     */
    bool isWorldUnlocked(int world);

    /**
     *  unlock world and save to user defaults
     *
     * @param world id, unlock value
     *
     */
    void unlockWorld(int world, int value);

    /**
     *  unlock level for world and save to user defaults
     *
     * @param world id, unlock value
     *
     */
    void unlockLevel(int world, int level, int value);

    void setPlayerId(int type);
    bool getSharingReward(std::string key);
    void setPlayerTotalCash(int cash);
    void setRevive();

    void getStarsForLevel(int world, int level, int& money, int& enemies, int& heart);
    void setStarMap(int world, int level, bool money, bool enemies, bool heart);

  private:
    Gamedata();
    CC_DISALLOW_COPY_AND_ASSIGN(Gamedata);

    std::vector<int> _unlockedWorldsConfig;                    // TODO change to bools
    std::map<int, std::vector<int>> _unlockedLevelsConfigMap;  // TODO change to bools

    // stars
    std::map<int, std::vector<int>> _starMap;
    std::map<std::string, bool> _sharingRewards;

    // player
    CC_SYNTHESIZE_READONLY(int, _playerId, PlayerId)
    CC_SYNTHESIZE_READONLY(int, _playerTotalCash, PlayerTotalCash)
    CC_SYNTHESIZE_READONLY(int, _playerReviveCash, PlayerReviveCash)

    // settings
    CC_SYNTHESIZE(bool, _isAudioFxEnabled, AudioFxEnabled)
    CC_SYNTHESIZE(bool, _isMusicEnabled, MusicEnabled)
};

#endif  // GAMEDATA_GAMEDATA_H_
