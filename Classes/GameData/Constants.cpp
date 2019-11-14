/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "Constants.h"
#include "GameConfig.h"
#include "cocos2d.h"

Constants::Constants()
  : fontNormal(GAMECONFIG.getFont("font1"))
  , fontButton(GAMECONFIG.getFont("buttonfont"))
  , fontTitle(GAMECONFIG.getFont("titlefont"))
  , fontHud(GAMECONFIG.getFont("hudfont"))
  , fontSubtitle(GAMECONFIG.getFont("subtitlefont"))
  , audioFileType(".mp3")
  , hudIconCoin(GAMECONFIG.getSprite("hud_money"))
  , hudIconHeart(GAMECONFIG.getSprite("hud_power"))
  , hudIconEnemy(GAMECONFIG.getSprite("hud_enemy"))
  , hudIconShoot(GAMECONFIG.getSprite("arrow_shoot"))

  , iconCloud(GAMECONFIG.getSprite("cloud"))
  , iconCrashCloud("ig_crashcloud")
  // TODO array
  , iconHit(GAMECONFIG.getSprite("hit"))
  , iconStar(GAMECONFIG.getSprite("star"))
  , iconLock(GAMECONFIG.getSprite("lock"))
  , iconNoise(GAMECONFIG.getSprite("noise2"))
  , iconArrow(GAMECONFIG.getSprite("arrow_up"))

  // Buttons
  , buttonIconSettings(GAMECONFIG.getSprite("settings_button"))
  , buttonIconPlayNormal(GAMECONFIG.getSprite("play_button"))
  , buttonIconPauseNormal(GAMECONFIG.getSprite("pause_button"))
  , buttonIconMusicNormal(GAMECONFIG.getSprite("music_on_button"))
  , buttonIconMusicDown(GAMECONFIG.getSprite("music_off_button"))
  , buttonIconAudioFXNormal(GAMECONFIG.getSprite("fx_on_button"))
  , buttonIconAudioFXDown(GAMECONFIG.getSprite("fx_off_button"))

  // touch control area
  , touchArea1(0.1F)
  , touchArea2(0.2F)
  , touchArea3(0.7F)
  , touchArea4(0.9F)
  , touch1("touch1")
  , touch2("touch2")
  , touch3("touch3")
  , touch4("touch4")
  , accelerometer1("accelerometer1")
  , accelerometer2("accelerometer2")

  // audio
  , audioShoot("shoot")
  , audioJump("jump")
  , audioEnd("end")
  , audioShake("shake")
  , audioGameOver("gameover")
  , audioHit("hit")
  , audioExplode("explode")
  , audioStart("start")

  , userDefaultPlayerId("player_id")
  , userDefaultPlayerCash(" player_cash")
  , userDefaultPlayerReviveCash("player_revive_cash")
  , userDefaultSoundFx("sound_fx")
  , userDefaultSoundMusic("sound_music")
  , userDefaultRewards("rewards")
  , userDefaultUnlockedLevel("unlocked_level")
  , userDefaultUnlockedWorld("unlocked_world")
  , userDefaultStarCount("stars")
  , buttonTypeAttack("attack")
  , buttonTypeShoot("shoot")
  , buttonTypeDown("down")
  , buttonTypeJump("jump")
  , buttonTypeSwitch("switch")

  , tilemapTileLayer("tileLayer")
  , tilemapHazardLayer("hazardLayer")
  , tilemapInfoGroup("infoObjects")
  , tilemapGameObjectsGroup("gameObjects")
  , tilemapInteractionObjectGroup("interactionObjects")
  , tilemapInteractionObjectDestroy("destroy")
  , tilemapInteractionObjectJump("jump")
  , tilemapInteractionObjectClimb("climb")
  , tilemapInteractionObjectWalk("walk")

  // Button Size in according resolution
  , buttonSizeSD(32)
  , buttonSizeHD(64)
  , buttonSizeHDR(128)
{
}

int Constants::getOffset()
{
    auto kContentScaleFactor = cocos2d::Director::getInstance()->getContentScaleFactor();
    auto buttonSize = buttonSizeHDR;

    if (cocos2d::Director::getInstance()->getVisibleSize().width <= 400 * 1.2)
    {
        buttonSize = buttonSizeSD;
    }
    else if (cocos2d::Director::getInstance()->getVisibleSize().width <= 800 * 1.2)
    {
        buttonSize = buttonSizeHD;
    }

    return buttonSize / kContentScaleFactor;
}
