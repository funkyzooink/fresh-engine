/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "Enemy.h"

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

Enemy::Enemy() : _canMove(false), _canFly(false), _isFriend(false), _attackCoolDownCounter(0), _counter(0)

{
    setHeadingState(LEFT_HEADING);
    setMovementState(WALK_MOVEMENT);
    _soundIndex = Utility::randomValueBetween(20, 500);
    //  _counter = 0;  // CONF.randomValueBetween(2,50);
    _sleeping = false;
}

Enemy* Enemy::create(const std::string& idleIcon, float xSpeed, float ySpeed, int life,
                     std::vector<std::string> bulletTypes, bool gravity, bool canFly, bool canAttack, bool canJump,
                     bool canMove, bool flipAnimationX, std::string audio,
                     std::map<AnimationHelper::AnimationTagEnum, std::string> animationEnumMap)
{
    Enemy* sprite = createWithSpriteFrameName(idleIcon);
    sprite->setLife(life);  // todo
    sprite->_animationEnumMap = std::move(animationEnumMap);

    sprite->_idleIcon = idleIcon;
    sprite->_xSpeed = xSpeed;
    sprite->_ySpeed = ySpeed;
    sprite->_canMove = canMove;
    sprite->setGravity(gravity || canAttack || canJump);  // TODO document properly for json creation
    sprite->_audio = std::move(audio);

    sprite->_canAttack = canAttack;
    sprite->setCanJump(canJump);
    sprite->_canFly = canFly;
    sprite->_flipAnimationX = flipAnimationX;
    sprite->_bulletTypes = std::move(bulletTypes);

    return sprite;
}

Enemy* Enemy::clone(GameScene* gameScene) const
{
    auto enemy = Enemy::create(_idleIcon, _xSpeed, _ySpeed, _life, _bulletTypes, _gravity, _canFly, _canAttack,
                               _canJump, _canMove, _flipAnimationX, _audio, _animationEnumMap);

    for (auto& bulletName : _bulletTypes)
    {
        enemy->addAmmo(bulletName, gameScene, false);
    }
    enemy->_gameScene = gameScene;

    return enemy;
}

Enemy* Enemy::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);

#if COCOS2D_DEBUG > 0
    auto message = "Invalid spriteFrameName: " + spriteFrameName;
    CCASSERT(frame != nullptr, message.c_str());
#endif

    auto sprite = new Enemy();
    if (sprite && frame && sprite->initWithSpriteFrame(frame))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

// MARK: - update
bool Enemy::checkSleeping()
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

void Enemy::updateFly()
{
    if (_canFly)  // TODO refactor this
    {
        if (_ySpeed > 0.0)
        {
            auto randomNumber = Utility::randomValueBetween(0, 100);
            if (randomNumber > 85)
            {  // down
                auto speed = -_ySpeed * 2.0;
                _velocity.y = _velocity.y + (speed * 0.002);
            }
            else if (randomNumber < 15)
            {  // up
                auto speed = _ySpeed * 2.0;
                _velocity.y = _velocity.y + (speed * 0.002);
            }
        }

        _soundIndex--;
        if (_soundIndex < 1)
        {
            _soundIndex = Utility::randomValueBetween(800, 1200);
            AudioPlayer::playFx(_audio);
        }
    }
}

void Enemy::updateAttackAndShoot()
{
    if (!_ammoVector.empty())
    {
        if (_counter <= 0)
        {
            if (_canMove)
            {
                // stop
                //        cocos2d::Point forwardMove = cocos2d::Point(-_xSpeed * 10, 0.0);
                //        cocos2d::Point forwardStep = forwardMove * dt;
                _velocity = cocos2d::Point(0.0, 0.0);
            }

            runAction(cocos2d::Sequence::create(
                cocos2d::ScaleTo::create(0.025, 0.9), cocos2d::ScaleTo::create(0.025, 1.1),
                cocos2d::ScaleTo::create(0.025, 0.9), cocos2d::ScaleTo::create(0.025, 1.0),
                cocos2d::CallFuncN::create(CC_CALLBACK_0(Enemy::prepareToShoot, this)), nullptr));
        }
        else
        {
            _counter--;
        }
    }
    else if (getCanAttack())
    {
        float jumpCutoff = _ySpeed * 0.15;

        if (_attackCoolDownCounter == 100)
        {
            cocos2d::Point jumpForce = cocos2d::Point(_xSpeed * CONSTANTS.getOffset(), _ySpeed * CONSTANTS.getOffset());
            _counter = 15;
            _velocity = _velocity + jumpForce;
            _onGround = false;
        }
        else if (_counter > 0)
        {
            _counter--;
            _velocity.y = _velocity.y + (_counter * _counter * jumpCutoff / 100);
            _onGround = false;
        }

        _attackCoolDownCounter--;

        if (_gameScene != nullptr)  // TODO check necessary?
        {
            if ((getHeadingState() == RIGHT_HEADING && _onGround &&
                 _gameScene->getPlayerPosition().x < getPosition().x + getContentSize().width * 2 &&
                 _gameScene->getPlayerPosition().x > getPosition().x + getContentSize().width) ||
                (getHeadingState() == LEFT_HEADING && _onGround &&
                 _gameScene->getPlayerPosition().x > getPosition().x - getContentSize().width * 2 &&
                 _gameScene->getPlayerPosition().x < getPosition().x - getContentSize().width))
            {
                attack();
            }
        }
    }
}
void Enemy::updateMove(float dt)
{
    if (_canMove)
    {
        // if an enemy bumps into an obstacle it should turn around // TODO mix up between gameScene collision and here
        if (((getObstacle() == LEFT_OBSTACLE && getHeadingState() == LEFT_HEADING) ||
             (getObstacle() == RIGHT_OBSTACLE && getHeadingState() == RIGHT_HEADING)) &&
            _jumpState <= JUMP)
        {
            setObstacle(NO_OBSTACLE);
            setVelocity(cocos2d::Point(getVelocity().x * 0.5, getVelocity().y));  // slow down on turn around
            if (getHeadingState() == LEFT_HEADING)
            {
                setHeadingState(RIGHT_HEADING);
            }
            else if (getHeadingState() == RIGHT_HEADING)
            {
                setHeadingState(LEFT_HEADING);
            }
        }

        GameObject::update(dt);
    }
    else
    {
        _desiredPosition = _position;
        _onGround = true;  // always true

        if (_gameScene != nullptr)  // TODO check necessary?
        {
            if (getPosition() < _gameScene->getPlayerPosition())
            {
                setFlippedX(true);
                setHeadingState(RIGHT_HEADING);
            }
            else
            {
                setFlippedX(false);
                setHeadingState(LEFT_HEADING);
            }
        }
        auto animationTag = AnimationHelper::AnimationTagEnum::IDLE_LEFT_ANIMATION;
        if (isFlippedX())
            animationTag = AnimationHelper::AnimationTagEnum::IDLE_RIGHT_ANIMATION;

        checkAndStartAnimation(animationTag, _flipAnimationX);
    }
}

void Enemy::update(float dt)
{
    if (!isVisible() || getDisabled())  // enemy already defeated // todo remove from here
        return;

    if (checkSleeping())
    {
        return;
    }

    updateFly();
    updateAttackAndShoot();
    updateMove(dt);

    playerCollision();
    tileCollision();
}

// MARK: - collision

bool Enemy::playerCollision()
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
        cocos2d::Rect intersection = Utility::calculateIntersection(playerBb, gameObjectDestinationBb);

        if ((intersection.size.width * 3.0F) > intersection.size.height &&
            playerBb.origin.y - playerBb.size.height / 10 > gameObjectDestinationBb.origin.y && player->getHit() < 1 &&
            player->getCanKill() == "jump")  // TODO 4f
        {
            _gameScene->destroyGameObject(this);
            player->jumpFromObject();
        }
        else if (player->getCanKill() == "touch")
        {
            _gameScene->destroyGameObject(this);
        }
        else
        {
            _gameScene->playerHitByEnemyCallback();
        }
    }

    return false;
}

bool Enemy::shootOrAttackCollision(GameObject* p)
{
    if (!isVisible() || getDisabled())
        return false;

    cocos2d::Rect bulletBb = p->getBoundingBox();

    return bulletBb.intersectsRect(getBoundingBox());
}

bool Enemy::tileCollision()
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

    bool turnAround = (!getCanJump() && !getCanFly() && !getCanAttack());
    TileHelper::collisionTile(this, tiledMap->getLayer(CONSTANTS.tilemapTileLayer), mapSize, mapTileSize, turnAround);
    return false;
}

void Enemy::goToSleep()
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

void Enemy::wakeUp()
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

void Enemy::aboveCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{
    setDesiredPosition(cocos2d::Point(getDesiredPosition().x, getDesiredPosition().y - intersection.size.height));
    setVelocity(cocos2d::Point(getVelocity().x, 0.0));
}
void Enemy::belowCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{
    setObstacle(GameObject::ObstacleStateEnum::NO_OBSTACLE);
    if (getDesiredPosition().y < getPosition().y)  // TODO test if this
                                                   // always works
    {
        setDesiredPosition(cocos2d::Point(getDesiredPosition().x, getDesiredPosition().y + intersection.size.height));
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

// MARK: - helpers

void Enemy::prepareToShoot()  // TODO
{
    HeadingStateEnum direction = getHeadingState();
    for (auto& ammo : _ammoVector)
    {
        if (!ammo->isVisible())
        {
            _counter = Utility::randomValueBetween(300, 350);  // TODO put in config shooter(275, 325)
            shootBullet(ammo, direction);
            direction = direction == LEFT_HEADING ? RIGHT_HEADING : LEFT_HEADING;
        }
    }
}

void Enemy::attack()
{
    if (_attackCoolDownCounter < 0 && getCanAttack())
    {
        _attackCoolDownCounter = 100;
    }
}

// MARK: - getters and setters

void Enemy::setObstacle(ObstacleStateEnum obstacle)
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
