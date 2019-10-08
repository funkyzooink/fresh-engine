/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef HELPERS_USERDEFAULTS_H_
#define HELPERS_USERDEFAULTS_H_

#include <map>
#include <string>
#include <vector>

namespace cocos2d
{
class Data;
}
/**
 * UserDefaults Helper class to read and write to UserDefaults
 * this class is only for read and write access with as little logic as possible @see GameData for logic
 */
class UserDefaults
{
  public:
    static void writeDataToUserdefaults(std::string key, cocos2d::Data data);
    static cocos2d::Data readDataFromUserdefaults(std::string key, cocos2d::Data defaultData);

    // MARK: write
    static void saveAudioFxToUserdefaults(bool enabled);
    static void saveCashToUserdefaults(int cash);
    static void saveMusicFxToUserdefaults(bool enabled);
    static void savePlayerToUserdefaults(int type);
    static void saveSharingRewardsToUserdefaults(std::string key);
    static void saveReviveCashToUserdefaults(int cash);

    // MARK: read
    static int getPlayerIdFromUserdefaults(int defaultValue);
    static int getReviveCashFromUserdefaults(int defaultValue);
    static int getTotalCashFromUserdefaults(int defaultValue);
    static std::map<std::string, bool> getSharingRewardsFromUserdefaults();
    static void getAudioFromUserdefaults(bool& isAudioFxEnabled, bool& isMusicEnabled);

    static void reset();
};

#endif  // HELPERS_USERDEFAULTS_H_
