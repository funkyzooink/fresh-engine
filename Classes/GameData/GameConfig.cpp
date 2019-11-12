/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "GameConfig.h"
#include "../Helpers/Utility.h"
#include "../GameObjects/Block.h"
#include "../GameObjects/Bullet.h"
#include "../GameObjects/Enemy.h"
#include "../GameObjects/Player.h"
#include "../GameObjects/Upgrade.h"
#include "cocos2d.h"
#include "external/json/filereadstream.h"

// MARK - constants
static const char* LOCALIZATION_PATH = "json/localization/";
static const char* PLAYER_JSON_PATH = "json/characters/player.json";
static const char* BULLET_JSON_PATH = "json/characters/bullet.json";
static const char* BLOCK_JSON_PATH = "json/characters/block.json";
static const char* ENEMY_JSON_PATH = "json/characters/enemy.json";
static const char* UPGRADE_JSON_PATH = "json/characters/upgrade.json";
static const char* WORLD_JSON_PATH = "json/worlds.json";
static const char* GAMECONFIG_JSON_PATH = "json/gameconfig.json";

static const char* KEY_GAMEOBJECT_UPGRADE = "upgrade";
static const char* KEY_GAMEOBJECT_BULLETTYPE = "bullet_type";
static const char* KEY_GAMEOBJECT_CANKILL = "can_kill";
static const char* KEY_GAMEOBJECT_FLIPX = "flip_animation_x";
static const char* KEY_GAMEOBJECT_CUSTOMBUTTON1 = "custom_action_1";
static const char* KEY_GAMEOBJECT_CUSTOMBUTTON2 = "custom_action_2";
static const char* KEY_GAMEOBJECT_ANIMATIONS = "animations";
static const char* KEY_GAMEOBJECT_ANIMATION_IDLE = "idle_right";
static const char* KEY_GAMEOBJECT_ID = "id";
static const char* KEY_GAMEOBJECT_SPEEDX = "x_speed";
static const char* KEY_GAMEOBJECT_SPEEDY = "y_speed";
static const char* KEY_GAMEOBJECT_LIFETIME = "life_time";
static const char* KEY_GAMEOBJECT_ROTATIONANIMATION = "rotation_animation";
static const char* KEY_GAMEOBJECT_AUDIO = "audio";
static const char* KEY_GAMEOBJECT_ATTACK = "can_attack";
static const char* KEY_GAMEOBJECT_MOVE = "can_move";
static const char* KEY_GAMEOBJECT_FLY = "can_fly";
static const char* KEY_GAMEOBJECT_JUMP = "can_jump";
static const char* KEY_GAMEOBJECT_GRAVITY = "gravity";
static const char* KEY_GAMEOBJECT_LIFE = "life";
static const char* KEY_GAMEOBJECT_MONEY = "money";
static const char* KEY_GAMEOBJECT_PLAYER = "player";
static const char* KEY_GAMEOBJECT_GAMEOVER = "gameover";
static const char* KEY_GAMEOBJECT_ACTION = "action";

static const char* KEY_WORLD_SPRITES = "sprites";
static const char* KEY_WORLD_SPRITESHEETS = "sprite_sheets";
static const char* KEY_WORLD_PARALLAXSPRITES = "parallax_sprites";
static const char* KEY_WORLD_PARALLAXRATIO = "parallax_ratio";
static const char* KEY_WORLD_COLORS = "colors";
static const char* KEY_WORLD_LEVELS = "levels";
static const char* KEY_WORLD_PARTICLE = "particle";
static const char* KEY_WORLD_LEVELSELECTICON = "level_select_icon";

static const char* KEY_CONFIG_REWARD = "reward";
static const char* KEY_CONFIG_TITLE = "title";
static const char* KEY_CONFIG_ICON = "icon";
static const char* KEY_CONFIG_URL = "url";
static const char* KEY_CONFIG_RESSOURCES = "ressources";
static const char* KEY_CONFIG_FONTS = "fonts";
static const char* KEY_CONFIG_ORIENTATION = "device_orientation";
static const char* KEY_CONFIG_RESOLUTION = "sprite_resolution";
static const char* KEY_CONFIG_WIDTH = "width";
static const char* KEY_CONFIG_HEIGHT = "height";
static const char* KEY_CONFIG_UI = "ui";
static const char* KEY_CONFIG_MAINSCENE = "mainscene";
static const char* KEY_CONFIG_TILEMAP = "tilemap";
static const char* KEY_CONFIG_FONTCOLOR = "font_color";
static const char* KEY_CONFIG_BACKGROUNDS = "backgrounds";
static const char* KEY_CONFIG_LEVELINFO = "level_info_popup";
static const char* KEY_CONFIG_COLOR = "color";
static const char* KEY_CONFIG_BACKGROUND = "background";  // TODO replace with plural?
static const char* KEY_CONFIG_CRASH = "player_crash_rectangle";
static const char* KEY_CONFIG_TEXT = "text";
static const char* KEY_CONFIG_CONTROLS = "controls";
static const char* KEY_CONFIG_SPRITE = "sprite";
static const char* KEY_CONFIG_TYPE = "type";
static const char* KEY_CONFIG_LOCALIZATION = "localization";
static const char* KEY_CONFIG_GAMEPLAY = "gameplay";
static const char* KEY_CONFIG_PLAYERLIFE = "player_life";
static const char* KEY_CONFIG_REVIVESTART = "revive_start_cash";
static const char* KEY_CONFIG_REVIVEMULTI = "revive_multiply";

GameConfig::GameConfig()
  : _levelInfoPopupType(0)
  , _mainSceneAnimation(0)
  , _playerCrashRectangleColor("FF000066")
  , _backgroundColor("000000FF")
  , _textColor("FFFFFFFF")
  , _deviceOrientation("landscape")
{
    _gameplayConfig = { 3, 20, 2 };
    parseFiles();
}

std::vector<SharingConfig> GameConfig::getSharingconfig()
{
    return _sharingVector;
}

GameplayConfig GameConfig::getGameplayConfig() const
{
    return _gameplayConfig;
}

Player* GameConfig::getPlayerObject(int id)
{
    return _playerGameObjectVector.at(id);
}

Player* GameConfig::getPlayerObjectForKey(const std::string& key)
{  // TODO nullpntr handling
    auto result = std::find_if(_playerGameObjectVector.begin(), _playerGameObjectVector.end(),
                               [&](const auto& v) { return v->getName() == key; });
    return *result;
}

Enemy* GameConfig::getEnemyObject(const std::string& key)
{
#if COCOS2D_DEBUG > 0
    auto message = "Config does not exist: " + key;
    CCASSERT(_enemyGameObjectMap.count(key) > 0, message.c_str());
#endif
    return _enemyGameObjectMap[key];
}

Upgrade* GameConfig::getUpgradeObject(const std::string& key)
{
#if COCOS2D_DEBUG > 0
    auto message = "Config does not exist: " + key;
    CCASSERT(_upgradeGameObjectMap.count(key) > 0, message.c_str());
#endif
    return _upgradeGameObjectMap[key];
}

Block* GameConfig::getBlockObject(const std::string& key)
{
#if COCOS2D_DEBUG > 0
    auto message = "Config does not exist: " + key;
    CCASSERT(_blockGameObjectMap.count(key) > 0, message.c_str());
#endif
    return _blockGameObjectMap[key];
}

Bullet* GameConfig::getBulletObject(const std::string& key)
{
#if COCOS2D_DEBUG > 0
    auto message = "Config does not exist: " + key;
    CCASSERT(_bulletGameObjectMap.count(key) > 0, message.c_str());
#endif
    return _bulletGameObjectMap[key];
}

std::map<int, WorldConfig> GameConfig::getWorldOverview()
{
    return _worldConfigMap;
}

WorldConfig GameConfig::getWorldConfig(int id)
{
    return _worldConfigMap[id];
}

ControlConfig GameConfig::getControlConfig(const std::string& key)
{
#if COCOS2D_DEBUG > 0
    auto message = "Config does not exist: " + key;
    CCASSERT(_controlConfigMap.count(key) > 0, message.c_str());
#endif
    return _controlConfigMap[key];
}

std::vector<std::string> GameConfig::getSpriteSheets(int id)
{
    return _worldConfigMap[id].backgrounds.spriteSheets;
}

std::vector<std::string> GameConfig::getBackgroundSprites(int id)
{
    return _worldConfigMap[id].backgrounds.parallaxSprites;
}

std::string GameConfig::getWorldColors(int worldId, int colorId)  // TODO LRA
{
#if COCOS2D_DEBUG > 0
    auto message = "Color does not exist: " + Utility::numberToString(colorId);
    CCASSERT(_worldConfigMap.count(worldId) > 0, message.c_str());
#endif
    auto colors = _worldConfigMap[worldId].backgrounds.colors;
    return colors.at(colorId);
}

std::string GameConfig::getMusicForWorld(int id)
{
    return _worldConfigMap[id].audio;
}

ssize_t GameConfig::getWorldCount()
{
    return _worldConfigMap.size();
}

ssize_t GameConfig::getLevelCountForWorld(int worldId)
{
    return _worldConfigMap[worldId].levels.size();
}

int GameConfig::getLevelInfoPopupType()
{
    return _levelInfoPopupType;
}

int GameConfig::getMainSceneAnimation()
{
    return _mainSceneAnimation;
}

std::string GameConfig::getPlayerCrashRectangleColor()
{
    return _playerCrashRectangleColor;
}

std::string GameConfig::getBackgroundColor()
{
    return _backgroundColor;
}

std::string GameConfig::getTextColor()
{
    return _textColor;
}

bool GameConfig::isEnemyType(const std::string& key)
{
    return _enemyGameObjectMap.count(key) > 0;
}

bool GameConfig::isBlockType(const std::string& key)
{
    return _blockGameObjectMap.count(key) > 0;
}

bool GameConfig::isPlayerType(const std::string& key)
{
    auto result = std::find_if(_playerGameObjectVector.begin(), _playerGameObjectVector.end(),
                               [&](const auto& v) { return v->getName() == key; });
    return (result != _playerGameObjectVector.end());
}

bool GameConfig::isPlayerEntry(const std::string& key)  // TODO remove
{
    return key == KEY_GAMEOBJECT_PLAYER;
}

bool GameConfig::isUpgradeType(const std::string& key)
{
    return _upgradeGameObjectMap.count(key) > 0;
}

SceneConfig* GameConfig::getMainSceneConfig()
{
    return &_mainSceneConfig;
}

std::string GameConfig::getFont(const std::string& key)
{
    return _fontMap[key];
}

std::string GameConfig::getSprite(const std::string& key)
{
    return _spriteMap[key];
}

std::string GameConfig::getAudio(const std::string& key)
{
#if COCOS2D_DEBUG > 0
    auto message = "Key does not exist: " + key;
    CCASSERT(_audioFxMap.count(key) > 0, message.c_str());
#endif
    return _audioFxMap[key];
}

std::map<std::string, std::string> GameConfig::getAudioMap()
{
    return _audioFxMap;
}

std::string GameConfig::getLocalizedString(std::string key)
{
    if (_localizationDocument.HasMember(key.c_str()))
    {
        return _localizationDocument[key.c_str()].GetString();
    }
#if COCOS2D_DEBUG > 0
    auto message = "Key does not exist: " + key;
    CCASSERT(false, message.c_str());
#endif
    return key;  // Dont crash in production
}

std::vector<SpriteResolutionConfig> GameConfig::getSpriteResolutions()
{
    return _spriteResolutions;
}

// MARK: Fileparser

rapidjson::Document GameConfig::parseJSON(const std::string& filename)
{
    std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(filename);
    std::string json = cocos2d::FileUtils::getInstance()->getStringFromFile(fullPath);

    rapidjson::Document document;
    document.Parse<0>(json.c_str());

    return document;
}

void GameConfig::parseFiles()
{
    parseGameConfigJSON();
    parseLocalizationJSON();
}

void GameConfig::parseGameObjects()
{
    parseWorldJSON();   // TODO Needs to be parsed first as enemy and player need it
    parseBulletJSON();  // Needs to be parsed first as enemy and player need it
    parseBlockJSON();
    parsePlayerJSON();
    parseEnemyJSON();
    parseUpgradeJSON();
}

void GameConfig::parseLocalizationJSON()
{
    std::string language = cocos2d::Application::getInstance()->getCurrentLanguageCode();
    std::string fileName = "en.json";  // fallback to en

    if (std::find(_languages.begin(), _languages.end(), language) != _languages.end())
    {
        fileName = language + ".json";
    }
    else if (!_languages.empty())
    {  // fallback to first language
        fileName = _languages.at(0) + ".json";
    }
    std::string path = LOCALIZATION_PATH + fileName;
    _localizationDocument = parseJSON(path);
}

void GameConfig::parsePlayerJSON()
{
    auto document = parseJSON(PLAYER_JSON_PATH);

    for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
    {
        auto jsonObject = itr->value.GetObject();

        std::string upgrade;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_UPGRADE))
        {
            upgrade = jsonObject[KEY_GAMEOBJECT_UPGRADE].GetString();
        }

        std::vector<std::string> bullets;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_BULLETTYPE))
        {
            auto bulletArray = jsonObject[KEY_GAMEOBJECT_BULLETTYPE].GetArray();
            std::transform(bulletArray.begin(), bulletArray.end(), std::back_inserter(bullets),
                           [&](const auto& v) { return v.GetString(); });
        }
        std::string canKill;  // TODO optional
        if (jsonObject.HasMember(KEY_GAMEOBJECT_CANKILL))
        {
            canKill = jsonObject[KEY_GAMEOBJECT_CANKILL].GetString();
        }
        bool flipAnimationX = false;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_FLIPX))
        {
            flipAnimationX = jsonObject[KEY_GAMEOBJECT_FLIPX].GetBool();
        }

        std::string customButton1;  // TODO optional
        if (jsonObject.HasMember(KEY_GAMEOBJECT_CUSTOMBUTTON1))
        {
            customButton1 = jsonObject[KEY_GAMEOBJECT_CUSTOMBUTTON1].GetString();
        }

        std::string customButton2;  // TODO optional
        if (jsonObject.HasMember(KEY_GAMEOBJECT_CUSTOMBUTTON2))
        {
            customButton2 = jsonObject[KEY_GAMEOBJECT_CUSTOMBUTTON2].GetString();
        }

        // animations
        auto animations = jsonObject[KEY_GAMEOBJECT_ANIMATIONS].GetObject();
        std::map<std::string, std::vector<std::string>> animationMap;

        for (rapidjson::Value::ConstMemberIterator levelItr = animations.MemberBegin();
             levelItr != animations.MemberEnd(); ++levelItr)
        {
            auto animations = levelItr->value.GetArray();

            std::vector<std::string> animationVector;
            for (auto& animation : animations)  // TODO std
            {
                animationVector.push_back(animation.GetString());
            }
            animationMap[levelItr->name.GetString()] = animationVector;
        }

#if COCOS2D_DEBUG > 0
        std::string message = "Animation does not exist: idle_right";
        CCASSERT(animationMap.count(KEY_GAMEOBJECT_ANIMATION_IDLE) > 0, message.c_str());
#endif

        std::string idleIcon = animationMap.at(KEY_GAMEOBJECT_ANIMATION_IDLE).at(0);

        std::string filepath = itr->name.GetString();
        auto animationEnum = AnimationHelper::initAnimations(filepath, animationMap);

        auto player =
            Player::create(itr->name.GetString(), idleIcon, jsonObject[KEY_GAMEOBJECT_ID].GetInt(),
                           jsonObject[KEY_GAMEOBJECT_SPEEDX].GetFloat(), jsonObject[KEY_GAMEOBJECT_SPEEDY].GetFloat(),
                           canKill, customButton1, customButton2, bullets, flipAnimationX, upgrade, animationEnum);
        player->retain();
        _playerGameObjectVector.push_back(player);
    }
}

void GameConfig::parseBulletJSON()
{
    auto document = parseJSON(BULLET_JSON_PATH);

    for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
    {
        auto jsonObject = itr->value.GetObject();

        float xSpeed = 0.0F;
        float ySpeed = 0.0F;

        if (jsonObject.HasMember(KEY_GAMEOBJECT_SPEEDX))
        {
            xSpeed = jsonObject[KEY_GAMEOBJECT_SPEEDX].GetFloat();
        }

        if (jsonObject.HasMember(KEY_GAMEOBJECT_SPEEDY))
        {
            ySpeed = jsonObject[KEY_GAMEOBJECT_SPEEDY].GetFloat();
        }

        int lifeTime = -1;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_LIFETIME))
        {
            lifeTime = jsonObject[KEY_GAMEOBJECT_LIFETIME].GetInt();
        }

        bool rotationAnimation = true;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_ROTATIONANIMATION))
        {
            rotationAnimation = jsonObject[KEY_GAMEOBJECT_ROTATIONANIMATION].GetBool();
        }

        bool flipAnimationX = false;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_FLIPX))
        {
            flipAnimationX = jsonObject[KEY_GAMEOBJECT_FLIPX].GetBool();
        }

        std::string audio;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_AUDIO))
        {
            audio = jsonObject[KEY_GAMEOBJECT_AUDIO].GetString();
        }

        // animations
        auto animations = jsonObject[KEY_GAMEOBJECT_ANIMATIONS].GetObject();
        std::map<std::string, std::vector<std::string>> animationMap;

        for (rapidjson::Value::ConstMemberIterator levelItr = animations.MemberBegin();
             levelItr != animations.MemberEnd(); ++levelItr)
        {
            auto animation = levelItr->value.GetArray();

            std::vector<std::string> animationVector;
            for (rapidjson::SizeType i = 0; i < animation.Size(); i++)
            {
                animationVector.push_back(animation[i].GetString());
            }
            animationMap[levelItr->name.GetString()] = animationVector;
        }

#if COCOS2D_DEBUG > 0
        std::string message = "Animation does not exist: idle_right";
        CCASSERT(animationMap.count(KEY_GAMEOBJECT_ANIMATION_IDLE) > 0, message.c_str());
#endif
        std::string idleIcon = animationMap.at(KEY_GAMEOBJECT_ANIMATION_IDLE).at(0);

        std::string filepath = itr->name.GetString();
        auto animationEnum = AnimationHelper::initAnimations(filepath, animationMap);

        auto bullet =
            Bullet::create(idleIcon, xSpeed, ySpeed, rotationAnimation, lifeTime, flipAnimationX, audio, animationEnum);

        bullet->retain();
        _bulletGameObjectMap[filepath] = bullet;
    }
}

void GameConfig::parseBlockJSON()
{
    auto document = parseJSON(BLOCK_JSON_PATH);

    for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
    {
        auto jsonObject = itr->value.GetObject();

        float xSpeed = 0.0F;
        float ySpeed = 0.0F;

        if (jsonObject.HasMember(KEY_GAMEOBJECT_SPEEDX))
        {
            xSpeed = jsonObject[KEY_GAMEOBJECT_SPEEDX].GetFloat();
        }

        if (jsonObject.HasMember(KEY_GAMEOBJECT_SPEEDY))
        {
            ySpeed = jsonObject[KEY_GAMEOBJECT_SPEEDY].GetFloat();
        }

        bool canAttack = false;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_ATTACK))
        {
            canAttack = jsonObject[KEY_GAMEOBJECT_ATTACK].GetBool();
        }

        bool canMove = true;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_MOVE))
        {
            canMove = jsonObject[KEY_GAMEOBJECT_MOVE].GetBool();
        }

        bool flipAnimationX = false;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_FLIPX))
        {
            flipAnimationX = jsonObject[KEY_GAMEOBJECT_FLIPX].GetBool();
        }

        std::string audio;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_AUDIO))
        {
            audio = jsonObject[KEY_GAMEOBJECT_AUDIO].GetString();
        }

        // animations
        auto animations = jsonObject[KEY_GAMEOBJECT_ANIMATIONS].GetObject();
        std::map<std::string, std::vector<std::string>> animationMap;

        for (rapidjson::Value::ConstMemberIterator levelItr = animations.MemberBegin();
             levelItr != animations.MemberEnd(); ++levelItr)
        {
            auto animation = levelItr->value.GetArray();

            std::vector<std::string> animationVector;
            for (rapidjson::SizeType i = 0; i < animation.Size(); i++)  // TODO
            {
                animationVector.push_back(animation[i].GetString());
            }
            animationMap[levelItr->name.GetString()] = animationVector;
        }

        std::string message = "Animation does not exist: idle_right";
        CCASSERT(animationMap.count(KEY_GAMEOBJECT_ANIMATION_IDLE) > 0, message.c_str());
        std::string idleIcon = animationMap.at(KEY_GAMEOBJECT_ANIMATION_IDLE).at(0);

        std::string filepath = itr->name.GetString();
        auto animationEnum = AnimationHelper::initAnimations(filepath, animationMap);

        auto block = Block::create(idleIcon, xSpeed, ySpeed, canAttack, canMove, flipAnimationX, audio, animationEnum);

        block->retain();
        _blockGameObjectMap[filepath] = block;
    }
}
void GameConfig::parseEnemyJSON()
{
    auto document = parseJSON(ENEMY_JSON_PATH);

    for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
    {
        auto jsonObject = itr->value.GetObject();

        float xSpeed = 0.0F;
        float ySpeed = 0.0f;

        if (jsonObject.HasMember(KEY_GAMEOBJECT_SPEEDX))
        {
            xSpeed = jsonObject[KEY_GAMEOBJECT_SPEEDX].GetFloat();
        }

        if (jsonObject.HasMember(KEY_GAMEOBJECT_SPEEDY))
        {
            ySpeed = jsonObject[KEY_GAMEOBJECT_SPEEDY].GetFloat();
        }

        int life = 1;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_LIFE))
        {
            life = jsonObject[KEY_GAMEOBJECT_LIFE].GetInt();
        }

        std::vector<std::string> bullets;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_BULLETTYPE))
        {
            auto bulletArray = jsonObject[KEY_GAMEOBJECT_BULLETTYPE].GetArray();
            std::transform(bulletArray.begin(), bulletArray.end(), std::back_inserter(bullets),
                           [&](const auto& v) { return v.GetString(); });
        }

        bool gravity = false;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_GRAVITY))
        {
            gravity = jsonObject[KEY_GAMEOBJECT_GRAVITY].GetBool();
        }

        bool canFly = false;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_FLY))
        {
            canFly = jsonObject[KEY_GAMEOBJECT_FLY].GetBool();
        }

        bool canAttack = false;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_ATTACK))
        {
            canAttack = jsonObject[KEY_GAMEOBJECT_ATTACK].GetBool();
        }

        bool canJump = false;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_JUMP))
        {
            canJump = jsonObject[KEY_GAMEOBJECT_JUMP].GetBool();
        }

        bool canMove = true;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_MOVE))
        {
            canMove = jsonObject[KEY_GAMEOBJECT_MOVE].GetBool();
        }

        bool flipAnimationX = false;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_FLIPX))
        {
            flipAnimationX = jsonObject[KEY_GAMEOBJECT_FLIPX].GetBool();
        }

        std::string audio;
        if (jsonObject.HasMember(KEY_GAMEOBJECT_AUDIO))
        {
            audio = jsonObject[KEY_GAMEOBJECT_AUDIO].GetString();
        }

        // animations
        auto animations = jsonObject[KEY_GAMEOBJECT_ANIMATIONS].GetObject();
        std::map<std::string, std::vector<std::string>> animationMap;

        for (rapidjson::Value::ConstMemberIterator levelItr = animations.MemberBegin();
             levelItr != animations.MemberEnd(); ++levelItr)
        {
            auto animation = levelItr->value.GetArray();

            std::vector<std::string> animationVector;
            for (rapidjson::SizeType i = 0; i < animation.Size(); i++)
            {
                animationVector.push_back(animation[i].GetString());
            }
            animationMap[levelItr->name.GetString()] = animationVector;
        }

#if COCOS2D_DEBUG > 0
        std::string message = "Animation does not exist: idle_right";
        CCASSERT(animationMap.count(KEY_GAMEOBJECT_ANIMATION_IDLE) > 0, message.c_str());
#endif
        std::string idleIcon = animationMap.at(KEY_GAMEOBJECT_ANIMATION_IDLE).at(0);

        std::string filepath = itr->name.GetString();
        auto animationEnum = AnimationHelper::initAnimations(filepath, animationMap);

        auto enemy = Enemy::create(idleIcon, xSpeed, ySpeed, life, bullets, gravity, canFly, canAttack, canJump,
                                   canMove, flipAnimationX, audio, animationEnum);

        enemy->retain();
        _enemyGameObjectMap[filepath] = enemy;
    }
}

void GameConfig::parseUpgradeJSON()
{
    auto document = parseJSON(UPGRADE_JSON_PATH);

    for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
    {
        auto jsonObject = itr->value.GetObject();

        // actions
        auto jsonActions = jsonObject[KEY_GAMEOBJECT_ACTION].GetObject();
        UpgradeActions upgradeActions;

        int life = 0;
        if (jsonActions.HasMember(KEY_GAMEOBJECT_LIFE))
        {
            life = jsonActions[KEY_GAMEOBJECT_LIFE].GetInt();
        }

        int money = 0;
        if (jsonActions.HasMember(KEY_GAMEOBJECT_MONEY))
        {
            money = jsonActions[KEY_GAMEOBJECT_MONEY].GetInt();
        }
        int player = -20;  // TODO default value!
        if (jsonActions.HasMember(KEY_GAMEOBJECT_PLAYER))
        {
            player = jsonActions[KEY_GAMEOBJECT_PLAYER].GetInt();
        }

        bool gameOver = false;
        if (jsonActions.HasMember(KEY_GAMEOBJECT_GAMEOVER))
        {
            gameOver = jsonActions[KEY_GAMEOBJECT_GAMEOVER].GetBool();
        }

        std::string audio;  // TODO optional
        if (jsonActions.HasMember(KEY_GAMEOBJECT_AUDIO))
        {
            audio = jsonActions[KEY_GAMEOBJECT_AUDIO].GetString();
        }

        upgradeActions = { life, money, player, gameOver, audio };

        // animations
        auto animations = jsonObject[KEY_GAMEOBJECT_ANIMATIONS].GetObject();
        std::map<std::string, std::vector<std::string>> animationMap;

        for (rapidjson::Value::ConstMemberIterator levelItr = animations.MemberBegin();
             levelItr != animations.MemberEnd(); ++levelItr)
        {
            auto animation = levelItr->value.GetArray();

            std::vector<std::string> animationVector;
            for (rapidjson::SizeType i = 0; i < animation.Size(); i++)
            {
                animationVector.push_back(animation[i].GetString());
            }
            animationMap[levelItr->name.GetString()] = animationVector;
        }

        std::string filepath = itr->name.GetString();
        auto animationEnum = AnimationHelper::initAnimations(filepath, animationMap);

#if COCOS2D_DEBUG > 0
        std::string message = "Animation does not exist: idle_right";
        CCASSERT(animationMap.count(KEY_GAMEOBJECT_ANIMATION_IDLE) > 0, message.c_str());
#endif

        std::string idleIcon = animationMap.at(KEY_GAMEOBJECT_ANIMATION_IDLE).at(0);

        auto upgrade = Upgrade::create(idleIcon, upgradeActions, animationEnum);
        upgrade->retain();
        _upgradeGameObjectMap[filepath] = upgrade;
    }
}

void GameConfig::parseWorldJSON()
{
    auto document = parseJSON(WORLD_JSON_PATH);

    int id = 0;
    for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
    {
        auto world = itr->value.GetObject();

        // music
        std::string audio;  // TODO optional
        if (world.HasMember(KEY_GAMEOBJECT_AUDIO))
        {
            audio = world[KEY_GAMEOBJECT_AUDIO].GetString();
        }

        // background
        auto backgrounds = world[KEY_WORLD_SPRITES].GetObject();

        Background background;

        auto spriteSheets = backgrounds[KEY_WORLD_SPRITESHEETS].GetArray();
        for (auto& spriteSheet : spriteSheets)
        {
            background.spriteSheets.push_back(spriteSheet.GetString());
        }

        auto backgroundSprites = backgrounds[KEY_WORLD_PARALLAXSPRITES].GetArray();
        for (auto& backgroundSprite : backgroundSprites)
        {
            background.parallaxSprites.push_back(backgroundSprite.GetString());
        }

        auto backgroundParallaxRatios = backgrounds[KEY_WORLD_PARALLAXRATIO].GetArray();
        for (auto& backgroundParallaxRatio : backgroundParallaxRatios)
        {
            background.parallaxRatio.push_back(backgroundParallaxRatio.GetFloat());
        }

        if (backgrounds.HasMember(KEY_WORLD_COLORS))
        {  // TODO currently only LRA
            auto colors = backgrounds[KEY_WORLD_COLORS].GetArray();
            for (auto& color : colors)
            {
                background.colors.push_back(color.GetString());
            }
        }

        // levels
        auto levels = world[KEY_WORLD_LEVELS].GetObject();
        std::vector<LevelConfig> vLevel;
        int levelId = 0;
        for (rapidjson::Value::ConstMemberIterator levelItr = levels.MemberBegin(); levelItr != levels.MemberEnd();
             ++levelItr)
        {
            auto level = levelItr->value.GetObject();

            std::string particle;
            if (level.HasMember(KEY_WORLD_PARTICLE))
            {
                particle = level[KEY_WORLD_PARTICLE].GetString();
            }

            LevelConfig levelConfig = { levelId, level[KEY_CONFIG_TILEMAP].GetString(),
                                        level[KEY_WORLD_LEVELSELECTICON].GetString(), levelItr->name.GetString(),
                                        particle };
            vLevel.push_back(levelConfig);
            levelId++;
        }

        WorldConfig worldConfig = {
            id, "", world[KEY_WORLD_LEVELSELECTICON].GetString(), itr->name.GetString(), "", audio, background, vLevel
        };  // TODO add particle

        _worldConfigMap[worldConfig.id] = worldConfig;

        id++;
    }
}

void GameConfig::parseGameConfigJSON()
{
    auto document = parseJSON(GAMECONFIG_JSON_PATH);

    for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
    {
        std::string key = itr->name.GetString();

        if (key == "sharing")
        {
            std::string configDefaultName = "default";
            std::string configPlatformName = "default";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
            configPlatformName = "windows";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
            configPlatformName = "mac";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
            configPlatformName = "linux";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            configPlatformName = "android";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            configPlatformName = "ios";
#endif

            auto jsonObject = itr->value.GetObject();
            if (jsonObject.HasMember(configPlatformName.c_str()))  // TODO duplicate code
            {
                auto configObject = jsonObject[configPlatformName.c_str()].GetObject();
                for (rapidjson::Value::ConstMemberIterator i = configObject.MemberBegin();
                     i != configObject.MemberEnd(); ++i)
                {
                    auto jsonSharing = i->value.GetObject();
                    int reward = -1;
                    if (jsonSharing.HasMember(KEY_CONFIG_REWARD))
                    {
                        reward = jsonSharing[KEY_CONFIG_REWARD].GetInt();
                    }
                    SharingConfig config = { jsonSharing[KEY_CONFIG_TITLE].GetString(),
                                             jsonSharing[KEY_CONFIG_ICON].GetString(),
                                             jsonSharing[KEY_CONFIG_URL].GetString(), reward };
                    _sharingVector.push_back(config);
                }
            }
            else if (jsonObject.HasMember(configDefaultName.c_str()))  // TODO duplicate code
            {
                auto configObject = jsonObject[configDefaultName.c_str()].GetObject();
                for (rapidjson::Value::ConstMemberIterator i = configObject.MemberBegin();
                     i != configObject.MemberEnd(); ++i)
                {
                    auto jsonSharing = i->value.GetObject();
                    int reward = -1;
                    if (jsonSharing.HasMember(KEY_CONFIG_REWARD))
                    {
                        reward = jsonSharing[KEY_CONFIG_REWARD].GetInt();
                    }
                    SharingConfig config = { jsonSharing[KEY_CONFIG_TITLE].GetString(),
                                             jsonSharing[KEY_CONFIG_ICON].GetString(),
                                             jsonSharing[KEY_CONFIG_URL].GetString(), reward };
                    _sharingVector.push_back(config);
                }
            }
        }
        else if (key == KEY_CONFIG_RESSOURCES)
        {
            auto jsonObject = itr->value.GetObject();
            for (rapidjson::Value::ConstMemberIterator it = jsonObject.MemberBegin(); it != jsonObject.MemberEnd();
                 ++it)
            {
                auto jsonRessources = it->value.GetObject();
                std::string value = it->name.GetString();
                if (value == KEY_CONFIG_FONTS)
                {
                    for (rapidjson::Value::ConstMemberIterator i = jsonRessources.MemberBegin();
                         i != jsonRessources.MemberEnd(); ++i)
                    {
                        _fontMap[i->name.GetString()] = i->value.GetString();
                    }
                }
                else if (value == KEY_GAMEOBJECT_AUDIO)
                {
                    for (rapidjson::Value::ConstMemberIterator i = jsonRessources.MemberBegin();
                         i != jsonRessources.MemberEnd(); ++i)
                    {
                        _audioFxMap[i->name.GetString()] = i->value.GetString();
                    }
                }
                else if (value == KEY_WORLD_SPRITES)
                {
                    for (rapidjson::Value::ConstMemberIterator i = jsonRessources.MemberBegin();
                         i != jsonRessources.MemberEnd(); ++i)
                    {
                        _spriteMap[i->name.GetString()] = i->value.GetString();
                    }
                }
            }
        }
        else if (key == KEY_CONFIG_ORIENTATION)
        {
            _deviceOrientation = itr->value.GetString();
        }
        else if (key == KEY_CONFIG_RESOLUTION)
        {
            auto jsonObject = itr->value.GetObject();
            for (rapidjson::Value::ConstMemberIterator it = jsonObject.MemberBegin(); it != jsonObject.MemberEnd();
                 ++it)
            {
                auto jsonObject2 = it->value.GetObject();
                auto width = jsonObject2[KEY_CONFIG_WIDTH].GetInt();
                auto height = jsonObject2[KEY_CONFIG_HEIGHT].GetInt();
                SpriteResolutionConfig spriteConfig = { it->name.GetString(), cocos2d::Size(width, height) };
                _spriteResolutions.push_back(spriteConfig);
            }
        }
        else if (key == KEY_CONFIG_UI)
        {
            auto jsonObject = itr->value.GetObject();
            for (rapidjson::Value::ConstMemberIterator it = jsonObject.MemberBegin(); it != jsonObject.MemberEnd();
                 ++it)
            {
                auto jsonObject2 = it->value.GetObject();

                std::string value = it->name.GetString();
                if (value == KEY_CONFIG_MAINSCENE)
                {
                    if (jsonObject2.HasMember(KEY_CONFIG_TILEMAP))
                    {
                        _mainSceneConfig.tilemap = jsonObject2[KEY_CONFIG_TILEMAP].GetString();
                    }

                    if (jsonObject2.HasMember(KEY_CONFIG_TITLE))
                    {
                        _mainSceneConfig.title = jsonObject2[KEY_CONFIG_TITLE].GetString();
                    }

                    if (jsonObject2.HasMember(KEY_CONFIG_FONTCOLOR))
                    {
                        _mainSceneConfig.fontColor = jsonObject2[KEY_CONFIG_FONTCOLOR].GetString();
                    }

                    // backgrounds
                    auto backgrounds = jsonObject2[KEY_CONFIG_BACKGROUNDS].GetArray();
                    for (rapidjson::SizeType i = 0; i < backgrounds.Size(); i++)
                    {
                        _mainSceneConfig.background.push_back(backgrounds[i].GetString());
                    }
                }
                else if (value == KEY_GAMEOBJECT_ANIMATIONS)
                {
                    if (jsonObject2.HasMember(KEY_CONFIG_LEVELINFO))
                    {
                        _levelInfoPopupType = jsonObject2[KEY_CONFIG_LEVELINFO].GetInt();
                    }
                    if (jsonObject2.HasMember(KEY_CONFIG_MAINSCENE))
                    {
                        _mainSceneAnimation = jsonObject2[KEY_CONFIG_MAINSCENE].GetInt();
                    }
                }
                else if (value == KEY_CONFIG_COLOR)
                {
                    if (jsonObject2.HasMember(KEY_CONFIG_CRASH))
                    {
                        _playerCrashRectangleColor = jsonObject2[KEY_CONFIG_CRASH].GetString();
                    }
                    if (jsonObject2.HasMember(KEY_CONFIG_BACKGROUND))
                    {
                        _backgroundColor = jsonObject2[KEY_CONFIG_BACKGROUND].GetString();
                    }
                    if (jsonObject2.HasMember(KEY_CONFIG_TEXT))
                    {
                        _textColor = jsonObject2[KEY_CONFIG_TEXT].GetString();
                    }
                }
            }
        }
        else if (key == KEY_CONFIG_CONTROLS)
        {
            auto jsonObject = itr->value.GetObject();
            for (rapidjson::Value::ConstMemberIterator it = jsonObject.MemberBegin(); it != jsonObject.MemberEnd();
                 ++it)
            {
                auto jsonObject2 = it->value.GetObject();
                auto type = jsonObject2[KEY_CONFIG_TYPE].GetString();
                std::string sprite;
                if (jsonObject2.HasMember(KEY_CONFIG_SPRITE))
                {
                    sprite = jsonObject2[KEY_CONFIG_SPRITE].GetString();
                }
                ControlConfig controlConf = { type, sprite };
                std::string key = it->name.GetString();
                _controlConfigMap[key] = controlConf;
            }
        }
        else if (key == KEY_CONFIG_LOCALIZATION)
        {
            auto jsonObject = itr->value.GetArray();
            for (rapidjson::SizeType i = 0; i < jsonObject.Size(); i++)
            {
                _languages.push_back(jsonObject[i].GetString());
            }
        }
        else if (key == KEY_CONFIG_GAMEPLAY)
        {
            auto jsonObject = itr->value.GetObject();
            int playerLife = 3;
            if (jsonObject.HasMember(KEY_CONFIG_PLAYERLIFE))
            {
                playerLife = jsonObject[KEY_CONFIG_PLAYERLIFE].GetInt();
            }
            int reviveStartCash = 20;
            if (jsonObject.HasMember(KEY_CONFIG_REVIVESTART))
            {
                reviveStartCash = jsonObject[KEY_CONFIG_REVIVESTART].GetInt();
            }
            int reviveMultiply = 2;
            if (jsonObject.HasMember(KEY_CONFIG_REVIVEMULTI))
            {
                reviveMultiply = jsonObject[KEY_CONFIG_REVIVEMULTI].GetInt();
            }

            _gameplayConfig = { playerLife, reviveStartCash, reviveMultiply };
        }
    }
}
