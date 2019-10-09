/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef GAMEDATA_GAMECONFIG_H_
#define GAMEDATA_GAMECONFIG_H_

#include <map>
#include <string>
#include <vector>
#include "math/CCGeometry.h"

#include "math/CCGeometry.h"

#include "external/json/document.h"
#include "platform/CCPlatformMacros.h"

#define GAMECONFIG GameConfig::getInstance()

class Block;
class Bullet;
class Enemy;
class Player;
class Upgrade;

struct UpgradeActions
{
    int life{};
    int money{};
    int playerId{};
    bool gameOver{};
    std::string audio;
};

struct Background
{
    std::vector<std::string> spriteSheets;
    std::vector<std::string> parallaxSprites;
    std::vector<float> parallaxRatio;
    std::vector<std::string> colors;  // TODO optional
};

struct LevelConfig
{
    int id;
    std::string tilemap;  // TODO optional // TODO currently not parsed for whole world
    std::string levelSelectIcon;
    std::string title;
    std::string particle;  // TODO optional // TODO currently not parsed for whole world
};

struct WorldConfig : public LevelConfig
{
    std::string audio;  // TODO optional
    Background backgrounds;
    std::vector<LevelConfig> levels;
};

struct SceneConfig
{
    std::string tilemap;
    std::string title;
    std::string fontColor;
    std::vector<std::string> background;
};

struct SharingConfig
{
    std::string title;
    std::string icon;
    std::string url;
    int reward;
};

struct ControlConfig
{
    std::string type;
    std::string sprite;  // TODO optional
};

struct SpriteResolutionConfig
{
    std::string tileMapIdentifier;
    cocos2d::Size size;
};

struct GameplayConfig
{
    int playerMaxLife;
    int reviveStartCash;
    int reviveMultiply;
};
/**
 * GameConfig reads configuration from JSON
 *
 */
class GameConfig
{
  public:
    static GameConfig& getInstance()
    {
        static GameConfig instance;  // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    void parseGameObjects();
    std::vector<SharingConfig> getSharingconfig();

    [[nodiscard]] GameplayConfig getGameplayConfig() const;

    /**
     * Get Player Object
     *
     * @param id
     *
     */
    Player* getPlayerObject(int id);

    /**
     * Get Enemy Object
     *
     * @param id
     *
     */
    Enemy* getEnemyObject(const std::string& key);

    /**
     * Get Upgrade Object
     *
     * @param id
     *
     */
    Upgrade* getUpgradeObject(const std::string& key);

    /**
     * Get Block Object
     *
     * @param id
     *
     */
    Block* getBlockObject(const std::string& key);
    /**
     * Get Bullet Object
     *
     * @param id
     *
     */
    Bullet* getBulletObject(const std::string& key);

    /**
     * Get World Overview Map
     */
    std::map<int, WorldConfig> getWorldOverview();

    /**
     * Get World Config for id
     *
     * @param id
     *
     */
    WorldConfig getWorldConfig(int id);

    /**
     * Get Control Object
     *
     * @param key
     *
     */
    ControlConfig getControlConfig(const std::string& key);

    std::vector<std::string> getSpriteSheets(int id);
    std::vector<std::string> getBackgroundSprites(int id);
    std::string getWorldColors(int worldId, int colorId);

    std::string getMusicForWorld(int id);

    ssize_t getWorldCount();
    ssize_t getLevelCountForWorld(int worldId);

    int getLevelInfoPopupType();
    int getMainSceneAnimation();

    std::string getPlayerCrashRectangleColor();
    std::string getBackgroundColor();
    std::string getTextColor();

    bool isEnemyType(const std::string& key);
    bool isBlockType(const std::string& key);
    bool isUpgradeType(const std::string& key);
    bool isPlayerType(const std::string& key);

    SceneConfig* getMainSceneConfig();
    std::string getFont(const std::string& key);
    std::string getSprite(const std::string& key);
    std::string getAudio(const std::string& key);

    std::map<std::string, std::string> getAudioMap();
    std::string getLocalizedString(std::string key);

    std::vector<SpriteResolutionConfig> getSpriteResolutions();

    // MARK: Fileparser
    rapidjson::Document parseJSON(const std::string& filename);

  private:
    GameConfig();

    CC_DISALLOW_COPY_AND_ASSIGN(GameConfig);

    // MARK: parse files
    /**
     *  Parse files
     */
    void parseFiles();
    /**
     *  Parse Bullet Json - Note has to be called before Player/Enemy
     */
    void parseBulletJSON();
    void parseBlockJSON();
    /**
     *  Parse Enemy Json - Note has to be called after Bullet JSON parser
     */
    void parseEnemyJSON();
    void parseGameConfigJSON();
    void parseLocalizationJSON();
    /**
     *  Parse Player Json - Note has to be called after Bullet JSON parser
     */
    void parsePlayerJSON();
    void parseUpgradeJSON();
    void parseWorldJSON();

    // MARK:
    std::vector<SharingConfig> _sharingVector;

    GameplayConfig _gameplayConfig{};

    // font, audio, and Sprites for pre caching
    std::map<std::string, std::string> _fontMap;
    std::map<std::string, std::string> _audioFxMap;
    std::map<std::string, std::string> _spriteMap;

    // World and GameObject configs
    std::vector<Player*> _playerGameObjectVector;
    std::map<std::string, Enemy*> _enemyGameObjectMap;
    std::map<std::string, Upgrade*> _upgradeGameObjectMap;
    std::map<std::string, Block*> _blockGameObjectMap;
    std::map<std::string, Bullet*> _bulletGameObjectMap;
    std::map<int, WorldConfig> _worldConfigMap;
    SceneConfig _mainSceneConfig;

    int _levelInfoPopupType;
    int _mainSceneAnimation;
    std::string _playerCrashRectangleColor;
    std::string _backgroundColor;
    std::string _textColor;
    std::vector<std::string> _languages;
    std::vector<SpriteResolutionConfig> _spriteResolutions;
    std::map<std::string, ControlConfig> _controlConfigMap;

    CC_SYNTHESIZE_READONLY(std::string, _deviceOrientation, DeviceOrientation)
    rapidjson::Document _localizationDocument;
};

#endif  // GAMEDATA_GAMECONFIG_H_
