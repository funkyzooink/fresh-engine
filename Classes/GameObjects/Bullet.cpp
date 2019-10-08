/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "Bullet.h"

#include <utility>

#include "../GameData/Constants.h"
#include "../Helpers/AudioPlayer.h"
#include "../Helpers/TileHelper.h"
#include "../Helpers/Utility.h"
#include "../Scenes/GameScene.h"
#include "Player.h"  // TODO new collision detection
#include "Enemy.h"
#include "Upgrade.h"

#include "cocos2d.h"

// MARK: - create

Bullet::Bullet() : _rotationAnimation(false), _isFriend(false), _lifeTimeMax(0), _lifeTime(0)
{
    setHeadingState(LEFT_HEADING);
    setMovementState(WALK_MOVEMENT);
    //  _lifeTime = 0;  // CONF.randomValueBetween(2,50);
    _sleeping = false;
}

Bullet* Bullet::create(const std::string& idleIcon, float xSpeed, float ySpeed, bool rotationAnimation, int lifeTime,
                       bool flipAnimationX, std::string audio,
                       std::map<AnimationHelper::AnimationTagEnum, std::string> animationEnumMap)
{
    Bullet* sprite = createWithSpriteFrameName(idleIcon);
    sprite->_animationEnumMap = std::move(animationEnumMap);
    sprite->_idleIcon = idleIcon;
    sprite->_xSpeed = xSpeed;  // 1000.0f;
    sprite->_ySpeed = ySpeed;  // 750.0f;
    sprite->_audio = std::move(audio);

    sprite->_lifeTimeMax = lifeTime;
    sprite->_rotationAnimation = rotationAnimation;
    sprite->_flipAnimationX = flipAnimationX;

    return sprite;
}

Bullet* Bullet::clone(GameScene* gameScene) const
{
    auto bullet = Bullet::create(_idleIcon, _xSpeed, _ySpeed, _rotationAnimation, _lifeTimeMax, _flipAnimationX, _audio,
                                 _animationEnumMap);

    bullet->_gameScene = gameScene;
    return bullet;
}

Bullet* Bullet::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);

#if COCOS2D_DEBUG > 0
    auto message = "Invalid spriteFrameName: " + spriteFrameName;
    CCASSERT(frame != nullptr, message.c_str());
#endif

    auto sprite = new Bullet();
    if (sprite && frame && sprite->initWithSpriteFrame(frame))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

// MARK: - update
void Bullet::update(float dt)
{
    if (!isVisible() || getDisabled())  // enemy already defeated // todo remove from here
        return;

    if (!_sleeping)
    {
        _lifeTime--;
        _velocity = cocos2d::Point(_velocity.x * 0.90, _velocity.y);  // 2

        cocos2d::Point forwardMove = cocos2d::Point(_xSpeed * CONSTANTS.getOffset(), _ySpeed * CONSTANTS.getOffset());
        cocos2d::Point forwardStep = forwardMove * dt;
        _velocity = _velocity + forwardStep;

        cocos2d::Point minMovement = cocos2d::Point(0.0, -CONSTANTS.getOffset());
        cocos2d::Point maxMovement = cocos2d::Point(CONSTANTS.getOffset() * 3, CONSTANTS.getOffset() / 2);
        _velocity = _velocity.getClampPoint(minMovement, maxMovement);

        cocos2d::Point stepVelocity = _velocity * dt;

        setFlippedX(true);
        if (getHeadingState() == LEFT_HEADING)
        {
            setFlippedX(false);
            stepVelocity.x = -stepVelocity.x;
        }
        if (_lifeTime <= 0)
        {
            stepVelocity.y -= getContentSize().height / 8;
            stepVelocity.x *= 1.2;
        }

        _desiredPosition = _position + stepVelocity;
        //         GameObject::update(dt); // todo replace above with update
    }

    playerCollision();
    tileCollision();
}

// MARK: - collision

bool Bullet::playerCollision()
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
        if (!getIsFriend())  // enemy shoots player
        {
            // TODO check if this is used
            setDisabled(true);
            setVisible(false);

            _gameScene->explodeGameObject(this);
            _gameScene->playerHitByEnemyCallback();
            return false;
        }
    }
    return false;
}

bool Bullet::tileCollision()
{
    if (!GameObject::tileCollision())
    {
        return false;
    }

    setOnGround(false);

    auto tiledMap = _gameScene->getTileMap();
    const auto& mapTileSize = tiledMap->getTileSize();
    const auto& mapSize = tiledMap->getMapSize();

    TileHelper::simpleCollisionTile(this, tiledMap->getLayer(CONSTANTS.tilemapTileLayer), mapSize, mapTileSize, 1.0F);

    setPosition(getDesiredPosition());  // 8

    return false;
}

void Bullet::collisionShoot()
{
    auto gameObjectVector = _gameScene->getGameObjectVector();

    for (auto gameObject : gameObjectVector)
    {
        if (dynamic_cast<GameObject*>(gameObject) == nullptr || dynamic_cast<Player*>(gameObject) != nullptr)
            continue;

        if (dynamic_cast<GameObject*>(gameObject)->shootOrAttackCollision(this))
        {
            _gameScene->destroyGameObject(dynamic_cast<GameObject*>(gameObject));
            setVisible(false);
            setDisabled(true);  // TODO is this needed
        }
    }
}

// MARK: - collision callbacks
void Bullet::simpleCollisionCallback()
{
    setVisible(false);
    _gameScene->explodeGameObject(this);
}
// MARK: - helpers
void Bullet::resetLifeTime()
{
    _lifeTime = _lifeTimeMax;
}

void Bullet::setObstacle(ObstacleStateEnum obstacle)
{
    // intentionally do nothing
}
