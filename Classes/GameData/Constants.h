/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef GAMEDATA_CONSTANTS_H_
#define GAMEDATA_CONSTANTS_H_

#include <string>
#include "math/CCGeometry.h"
#include "platform/CCPlatformMacros.h"

#define CONSTANTS Constants::getInstance()
/**
 * Constants holds constants either hardcoded or from JSON via GameConfig
 * Cannot be static otherwise there are initialization problems on Android - TODO evaluate static vs singleton class
 */

class Constants
{
    // MARK: init
  public:
    enum LocalZOrderEnum
    {
        BACKGROUND_Z_ORDER = 0,
        NO_Z_ORDER = 1,
        MOVABLE_Z_ORDER = 2,
        PLAYER_Z_ORDER = 3,
        CONTROLS_Z_ORDER = 10
    };

    static Constants& getInstance()
    {
        static Constants instance;  // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    int getOffset();
    // MARK: constants

    // font
    const std::string fontNormal;
    const std::string fontButton;
    const std::string fontTitle;
    const std::string fontHud;
    const std::string fontSubtitle;

    const std::string audioFileType;  // mp3 or wav

    // icons
    const std::string hudIconCoin;
    const std::string hudIconHeart;
    const std::string hudIconEnemy;
    const std::string hudIconShoot;

    const std::string iconCloud;
    const std::string iconCrashCloud;
    const std::string iconHit;
    const std::string iconStar;
    const std::string iconLock;
    const std::string iconNoise;  // TODO only LRA color plugin
    const std::string iconArrow;  // TODO used in hud and levelselect

    const std::string buttonIconSettings;

    // button image paths
    const std::string buttonIconPlayNormal;
    const std::string buttonIconPauseNormal;
    const std::string buttonIconMusicNormal;
    const std::string buttonIconMusicDown;
    const std::string buttonIconAudioFXNormal;
    const std::string buttonIconAudioFXDown;

    // TODO save to device - customization

    // MARK: User Input
    float touchArea1;
    float touchArea2;
    float touchArea3;
    float touchArea4;

    const std::string touch1;
    const std::string touch2;
    const std::string touch3;
    const std::string touch4;
    const std::string accelerometer1;
    const std::string accelerometer2;

    // MARK: Audio
    const std::string audioShoot;
    const std::string audioJump;
    const std::string audioEnd;
    const std::string audioShake;
    const std::string audioGameOver;
    const std::string audioHit;
    const std::string audioExplode;
    const std::string audioStart;

    // MARK: Userefaults
    const std::string userDefaultPlayerId;
    const std::string userDefaultPlayerCash;
    const std::string userDefaultPlayerReviveCash;
    const std::string userDefaultSoundFx;
    const std::string userDefaultSoundMusic;
    const std::string userDefaultRewards;
    const std::string userDefaultUnlockedLevel;
    const std::string userDefaultUnlockedWorld;
    const std::string userDefaultStarCount;

    // MARK: ui button type
    const std::string buttonTypeAttack;
    const std::string buttonTypeShoot;
    const std::string buttonTypeDown;
    const std::string buttonTypeJump;
    const std::string buttonTypeSwitch;

    // MARK: Tile map
    const std::string tilemapTileLayer;               // tmx layer with normal tiles
    const std::string tilemapHazardLayer;             // tmx layer with tiles that hurt
    const std::string tilemapInfoGroup;               // tmx group with information that should be displayed
    const std::string tilemapGameObjectsGroup;        // tmx group containing all gameobjects
    const std::string tilemapInteractionObjectGroup;  // tmx group containing all object that can be either destroyed,
                                                      // moved through or climbed upon
    const std::string tilemapInteractionObjectDestroy;
    const std::string tilemapInteractionObjectJump;
    const std::string tilemapInteractionObjectClimb;

    CC_SYNTHESIZE(std::string, _tileSizeIdentifier, TileSizeIdentifier);

  private:
    Constants();
    CC_DISALLOW_COPY_AND_ASSIGN(Constants);

    const int buttonSizeSD;
    const int buttonSizeHD;
    const int buttonSizeHDR;
};

#endif  // GAMEDATA_CONSTANTS_H_
