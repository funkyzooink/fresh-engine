/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "Upgrade.h"

#include <utility>
#include "../GameData/Gamedata.h"
#include "../Helpers/AudioPlayer.h"
#include "../Helpers/Utility.h"
#include "../Scenes/GameScene.h"
#include "Player.h"  // TODO new collision detection

#include "cocos2d.h"

// MARK: - create

Upgrade::Upgrade()
{
}

Upgrade* Upgrade::create(const std::string& filepath, UpgradeActions upgradeActions,
                         std::map<AnimationHelper::AnimationTagEnum, std::string> animationEnumMap)
{
    Upgrade* sprite = createWithSpriteFrameName(filepath);
    sprite->_idleIcon = filepath;
    sprite->_actions = std::move(upgradeActions);
    sprite->_animationEnumMap = std::move(animationEnumMap);
    sprite->startIdleAnimation();
    return sprite;
}

Upgrade* Upgrade::clone(GameScene* gameScene) const
{
    auto upgrade = Upgrade::create(_idleIcon, _actions, _animationEnumMap);
    upgrade->_gameScene = gameScene;
    return upgrade;
}

Upgrade* Upgrade::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);

#if COCOS2D_DEBUG > 0
    auto message = "Invalid spriteFrameName: " + spriteFrameName;
    CCASSERT(frame != nullptr, message.c_str());
#endif

    auto sprite = new Upgrade();
    if (sprite && frame && sprite->initWithSpriteFrame(frame))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}
// MARK: - update

void Upgrade::update(float dt)
{
    // do not move
    _desiredPosition = _position;
    _onGround = true;  // always true

    playerCollision();
}

// MARK: - collision
bool Upgrade::playerCollision()
{
    if (!GameObject::playerCollision())
    {
        return false;
    }
    auto player = _gameScene->getPlayer();
    auto playerBb = Utility::getBoundingBox(player);
    auto gameObjectDestinationBb = Utility::getBoundingBox(this);

    if (playerBb.intersectsRect(gameObjectDestinationBb))
    {
        auto player = _gameScene->getPlayer();
        if (_actions.life > 0)
        {
            if (player->getLife() != GAMECONFIG.getGameplayConfig().playerMaxLife)
            {
                setDisabled(true);
                runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.3F, 2.0F), nullptr));
                runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.3F), cocos2d::Hide::create(), nullptr));
                _gameScene->increaseLife();
                AudioPlayer::playFx(_actions.audio);
            }

            return true;  // was false
        }
        else if (_actions.money > 0)  // coin
        {
            setDisabled(true);
            runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.3F, 2.0F), nullptr));
            runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.3F), cocos2d::Hide::create(), nullptr));
            _gameScene->increaseMoney();

            AudioPlayer::playFx(_actions.audio);
            return true;
        }
        else if (_actions.gameOver)  // exit
        {
            _gameScene->gameOverCallback(true);
            return true;
        }
        else if (_actions.playerId > 0)  // classic upgrade // TODO default value?
        {
            if (player->getId() == _actions.playerId)  // skip if player already has this
            {
                return true;
            }

            setDisabled(true);
            runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.3F, 2.0F), nullptr));
            runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.3F), cocos2d::Hide::create(), nullptr));

            _gameScene->upgradePlayer(_actions.playerId);

            AudioPlayer::playFx(_actions.audio);
            return true;
        }
    }

    return false;
}

void Upgrade::goToSleep()
{
    auto _player = _gameScene->getPlayer();

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    if (!((_player->getPosition().x > getPosition().x - visibleSize.width &&
           _player->getPosition().x < getPosition().x + visibleSize.width) &&
          (_player->getPosition().y > getPosition().y - visibleSize.height &&
           _player->getPosition().y < getPosition().y + visibleSize.height)))
    {
        setSleeping(true);
    }
}

void Upgrade::wakeUp()
{
    auto _player = _gameScene->getPlayer();

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    if ((_player->getPosition().x > getPosition().x - visibleSize.width &&
         _player->getPosition().x < getPosition().x + visibleSize.width) &&
        (_player->getPosition().y > getPosition().y - visibleSize.height &&
         _player->getPosition().y < getPosition().y + visibleSize.height))
    {
        setSleeping(false);
    }
}

// MARK: - getters and setters
UpgradeActions Upgrade::getActions()
{
    return _actions;
}

void Upgrade::setObstacle(ObstacleStateEnum obstacle)
{
    // intentionally do nothing
}