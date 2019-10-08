/****************************************************************************
 Copyright (c) 2014-2019 Gabriel Heilig
 fresh-engine
 funkyzooink@gmail.com
 ****************************************************************************/
#include "Block.h"

#include <utility>

#include "../GameData/Constants.h"
#include "../Helpers/AudioPlayer.h"
#include "../Helpers/TileHelper.h"
#include "../Helpers/Utility.h"
#include "../Scenes/GameScene.h"
#include "Player.h"  // TODO new collision detection
#include "Bullet.h"

#include "cocos2d.h"

// MARK: - create

Block::Block()
  : _canMove(false), _isFriend(false), _counter(0), _attackCoolDownCounter(0), _range(150)  // TODO from
                                                                                            // json or tmx

{
    setHeadingState(LEFT_HEADING);
    setMovementState(WALK_MOVEMENT);
    //  _counter = 0;  // CONF.randomValueBetween(2,50);
    _sleeping = false;
}

Block* Block::create(const std::string& idleIcon, float xSpeed, float ySpeed, bool canAttack, bool canMove,
                     bool flipAnimationX, std::string audio,
                     std::map<AnimationHelper::AnimationTagEnum, std::string> animationEnumMap)
{
    Block* sprite = createWithSpriteFrameName(idleIcon);
    sprite->_animationEnumMap = std::move(animationEnumMap);

    sprite->_idleIcon = idleIcon;
    sprite->_xSpeed = xSpeed;
    sprite->_ySpeed = ySpeed;
    sprite->_canMove = canMove;
    sprite->_audio = std::move(audio);
    sprite->_flipAnimationX = flipAnimationX;
    sprite->_canAttack = canAttack;
    return sprite;
}

Block* Block::clone(GameScene* gameScene) const
{
    auto Block =
        Block::create(_idleIcon, _xSpeed, _ySpeed, _canAttack, _canMove, _flipAnimationX, _audio, _animationEnumMap);

    for (auto& bulletName : _bulletTypes)
    {
        Block->addAmmo(bulletName, gameScene, false);
    }
    Block->_gameScene = gameScene;

    return Block;
}

Block* Block::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);

#if COCOS2D_DEBUG > 0
    auto message = "Invalid spriteFrameName: " + spriteFrameName;
    CCASSERT(frame != nullptr, message.c_str());
#endif

    Block* sprite = new Block();
    if (sprite && frame && sprite->initWithSpriteFrame(frame))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

// MARK: - update
bool Block::checkSleeping()
{
    if (getSleeping())
    {
        wakeUp();
    }
    else
    {
        goToSleep();
    }
    return getSleeping();
}

void Block::addVelocity(float dt)
{
    cocos2d::Point forwardMove = cocos2d::Point(_xSpeed * CONSTANTS.getOffset(), _ySpeed * CONSTANTS.getOffset());
    cocos2d::Point forwardStep = forwardMove * dt;
    _velocity = _velocity + forwardStep;

    _velocity = cocos2d::Point(_velocity.x * 0.9, _velocity.y * 0.9);  // 2

    _onGround = true;  // always true
    _velocity = Utility::clampMovment(_velocity);
    cocos2d::Point stepVelocity = _velocity * dt;
    if (getHeadingState() == LEFT_HEADING)
    {
        setFlippedX(false);
        stepVelocity.x = -stepVelocity.x;
    }
    else if (getHeadingState() == RIGHT_HEADING)
    {
        setFlippedX(true);
    }

    if (getJumpState() == FALLING_DOWN_JUMP)
    {
        stepVelocity.y = -stepVelocity.y;
    }

    _desiredPosition = _position + stepVelocity;
}

void Block::update(float dt)
{
    if (!isVisible() || getDisabled())  // Block already defeated // todo remove from here
        return;

    if (checkSleeping())
    {
        return;
    }
    _desiredPosition = _position;

    if (!getCanAttack())
    {
        _counter++;

        if (_counter > _range)
        {
            _counter = 0;
            if (_xSpeed > 0)
            {
                if (getHeadingState() == LEFT_HEADING)
                {
                    setFlippedX(true);
                    setHeadingState(RIGHT_HEADING);
                }
                else if (getHeadingState() == RIGHT_HEADING)
                {
                    setFlippedX(false);
                    setHeadingState(LEFT_HEADING);
                }
            }
            if (_ySpeed > 0)
            {
                if (getJumpState() == JUMP)
                {
                    setJumpState(FALLING_DOWN_JUMP);
                }
                else
                {
                    setJumpState(JUMP);
                }
            }
        }

        addVelocity(dt);
    }
    else if (_attackCoolDownCounter > 0)
    {  // spike falling down
        addVelocity(dt);
    }
    else if (getCanAttack() && _gameScene != nullptr &&
             (_gameScene->getPlayerPosition().x > getPosition().x - CONSTANTS.getOffset() * 2 &&
              _gameScene->getPlayerPosition().x < getPosition().x + CONSTANTS.getOffset() * 2))  //&&
    //             (_gameScene->getPlayerPosition().y >
    //                  getPosition().y - cocos2d::Director::getInstance()->getVisibleSize().height / 2 &&
    //              _gameScene->getPlayerPosition().y <
    //                  getPosition().y + cocos2d::Director::getInstance()->getVisibleSize().height / 2) &&
    //             _gameScene->getPlayerPosition().y < getPosition().y)  // TODO spike cleanup
    {
        attack();
    }

    playerCollision();
    tileCollision();
}

// MARK: - collision

bool Block::playerCollision()
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
        _gameScene->playerHitByEnemyCallback();
    }

    return false;
}

bool Block::shootOrAttackCollision(GameObject* p)
{
    return false;
}

bool Block::tileCollision()
{
    if (!GameObject::tileCollision())
    {
        return false;
    }

    if (!getCanMove())
        return false;

    auto tiledMap = _gameScene->getTileMap();
    const auto& mapTileSize = tiledMap->getTileSize();
    const auto& mapSize = tiledMap->getMapSize();

    TileHelper::collisionTile(this, tiledMap->getLayer(CONSTANTS.tilemapTileLayer), mapSize, mapTileSize);
    return false;
}

void Block::goToSleep()
{
    auto _player = _gameScene->getPlayer();

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    if (!((_player->getPosition().x > getPosition().x - visibleSize.width &&
           _player->getPosition().x < getPosition().x + visibleSize.width) &&
          (_player->getPosition().y > getPosition().y - visibleSize.height &&
           _player->getPosition().y < getPosition().y + visibleSize.height)))
    {
        for (auto const& ammo : getAmmoVector())
        {
            ammo->setSleeping(true);
        }

        setSleeping(true);
    }
}

void Block::wakeUp()
{
    auto _player = _gameScene->getPlayer();

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    if ((_player->getPosition().x > getPosition().x - visibleSize.width &&
         _player->getPosition().x < getPosition().x + visibleSize.width) &&
        (_player->getPosition().y > getPosition().y - visibleSize.height &&
         _player->getPosition().y < getPosition().y + visibleSize.height))
    {
        for (auto const& ammo : getAmmoVector())
        {
            ammo->setSleeping(false);
        }
        setSleeping(false);
    }
}

// MARK: - collision callbacks
void Block::aboveCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{
    setDesiredPosition(cocos2d::Point(getDesiredPosition().x, getDesiredPosition().y - intersection.size.height));
    setVelocity(cocos2d::Point(getVelocity().x, 0.0));
}
void Block::belowCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{
    if (getCanAttack())
    {
        showDestroyAnimationAndRemove();
    }
    else
    {  // TODO chek else block
        setObstacle(GameObject::ObstacleStateEnum::NO_OBSTACLE);
        if (getDesiredPosition().y < getPosition().y)  // TODO test if this
                                                       // always works
        {
            setDesiredPosition(
                cocos2d::Point(getDesiredPosition().x, getDesiredPosition().y + intersection.size.height));
            setVelocity(cocos2d::Point(getVelocity().x, 0.0));
            setOnGround(true);
            if (getJumpState() != GameObject::JumpStateEnum::NO_JUMP)
            {
                setJumpState(GameObject::JumpStateEnum::BACK_ON_GROUND_JUMP);
            }
            else
            {
                setJumpState(GameObject::JumpStateEnum::NO_JUMP);
            }
        }
    }
}
// MARK: - helpers

void Block::attack()
{
    if (_attackCoolDownCounter <= 0 && getCanAttack())
    {
        _attackCoolDownCounter = 100;
        setJumpState(FALLING_DOWN_JUMP);
    }
}
// MARK: - getters and setters

void Block::setObstacle(ObstacleStateEnum obstacle)
{
    if (getJumpState() == GameObject::JumpStateEnum::BACK_ON_GROUND_JUMP)
    {
        _obstacle = obstacle;
        setJumpState(GameObject::JumpStateEnum::NO_JUMP);
    }
    else if (getCanJump() && obstacle != GameObject::ObstacleStateEnum::NO_OBSTACLE && obstacle != _obstacle)  // TODO
    {
        setJumpState(GameObject::JumpStateEnum::WANTS_TO_JUMP);
    }
    else
    {
        _obstacle = obstacle;
    }
}
