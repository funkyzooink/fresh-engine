/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "Player.h"

#include <utility>

#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"
#include "../Helpers/TileHelper.h"
#include "../Helpers/Utility.h"
#include "../Scenes/GameScene.h"
#include "Bullet.h"
#include "Enemy.h"  // TODO new collision
#include "Upgrade.h"

#include "cocos2d.h"

// MARK: - create

Player::Player() : _canKillByJump(false), _attackTime(-10), _hit(0), _id(0), _shoot(0)
{
    _sleeping = false;
    setHeadingState(RIGHT_HEADING);
}

Player* Player::create(const std::string& idleIcon, int id, float xSpeed, float ySpeed, bool canKillByJump,
                       std::string additionalButton, std::vector<std::string> bulletTypes, bool flipAnimationX,
                       std::string upgrade, std::map<AnimationHelper::AnimationTagEnum, std::string> animationEnumMap)
{
    Player* sprite = createWithSpriteFrameName(idleIcon);
    sprite->_animationEnumMap = std::move(animationEnumMap);

    sprite->_idleIcon = idleIcon;
    sprite->_id = id;  // TODO remove
    sprite->_upgrade = std::move(upgrade);
    sprite->_xSpeed = xSpeed;   // 1000.0f;
    sprite->_ySpeed = ySpeed;   // 750.0f;
    sprite->setGravity(true);   // TODO
    sprite->setCanJump(true);   // TODO
    sprite->_canAttack = true;  // TODO

    sprite->_ammoVector.clear();

    sprite->_canKillByJump = canKillByJump;
    sprite->_flipAnimationX = flipAnimationX;
    sprite->setLife(GAMECONFIG.getGameplayConfig().playerMaxLife);

    sprite->_additionalButton = std::move(additionalButton);

    sprite->_bulletTypes = std::move(bulletTypes);
    return sprite;
}
Player* Player::clone(GameScene* gameScene) const
{
    auto player = Player::create(_idleIcon, _id, _xSpeed, _ySpeed, _canKillByJump, _additionalButton, _bulletTypes,
                                 _flipAnimationX, _upgrade, _animationEnumMap);
    for (auto& bulletName : _bulletTypes)
    {
        player->addAmmo(bulletName, gameScene, true);
    }

    player->_gameScene = gameScene;
    return player;
}

Player* Player::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);

#if COCOS2D_DEBUG > 0
    auto message = "Invalid spriteFrameName: " + spriteFrameName;
    CCASSERT(frame != nullptr, message.c_str());
#endif

    auto sprite = new Player();
    if (sprite && frame && sprite->initWithSpriteFrame(frame))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

// MARK: - update
void Player::update(float dt)
{
    // check if player shoot ammo and if it is still alive
    if (!_ammoVector.empty())
    {
        if (_shoot >= 0)
        {
            _shoot--;
        }
    }

    if (_attackTime > 0)
    {
        _attackTime--;
        collisionAttack();
        return;
    }
    else if (_attackTime == 0)
    {
        stopActionByTag(AnimationHelper::AnimationTagEnum::ATTACK_LEFT_ANIMATION);
        stopActionByTag(AnimationHelper::AnimationTagEnum::ATTACK_RIGHT_ANIMATION);
        _attackTime--;
    }
    else if (_attackTime > -10)
    {
        _attackTime--;
    }

    // check if player hit?
    if (_hit >= 1)
    {
        _hit--;
    }

    GameObject::update(dt);

    if (!getAmmoVector().empty())
    {
        for (auto const& ammo : getAmmoVector())
        {
            if (ammo->getLifeTime() < 1 && ammo->isVisible())
            {
                ammo->showDestroyAnimationAndRemove();
            }
            else if (ammo->isVisible())
            {
                ammo->collisionShoot();
            }
        }
    }
    tileCollision();
    collisionHazardTiles();
}

// MARK: - collision

bool Player::tileCollision()
{
    if (!GameObject::tileCollision())
    {
        return false;
    }

    auto tiledMap = _gameScene->getTileMap();
    const auto& mapTileSize = tiledMap->getTileSize();
    const auto& mapSize = tiledMap->getMapSize();

    TileHelper::collisionTile(this, tiledMap->getLayer(CONSTANTS.tilemapTileLayer), mapSize, mapTileSize);
    return false;
}

void Player::collisionAttack()
{
    auto gameObjectVector = _gameScene->getGameObjectVector();

    for (auto gameObject : gameObjectVector)
    {
        if (dynamic_cast<GameObject*>(gameObject) == nullptr || dynamic_cast<Player*>(gameObject) != nullptr)
            continue;

        if (dynamic_cast<GameObject*>(gameObject)->shootOrAttackCollision(this))
            _gameScene->destroyGameObject(dynamic_cast<GameObject*>(gameObject));
    }
}

void Player::collisionHazardTiles()
{
    auto tiledMap = _gameScene->getTileMap();
    const auto& mapTileSize = tiledMap->getTileSize();
    const auto& mapSize = tiledMap->getMapSize();

    TileHelper::simpleCollisionTile(this, tiledMap->getLayer(CONSTANTS.tilemapHazardLayer), mapSize, mapTileSize, 0.7F);
}

// MARK: - collision callbacks

void Player::aboveCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{
    auto collisionSource = tile.tileRect.origin;
    float tileX = tile.tilePositionX;
    float tileY = tile.tilePositionY;
    auto tileCoordinate = cocos2d::Point(tileX, tileY);

    if (!checkInteractionObjectCollision(collisionSource, tileCoordinate))  // TODO climb animation
    {
        setDesiredPosition(cocos2d::Point(getDesiredPosition().x, getDesiredPosition().y - intersection.size.height));
        setVelocity(cocos2d::Point(getVelocity().x, 0.0));
    }
}
void Player::belowCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{
    setObstacle(GameObject::ObstacleStateEnum::NO_OBSTACLE);
    if (getJumpState() == GameObject::JumpStateEnum::FALLING_DOWN_JUMP && getOnGround() == false)  // TODO shuffle
    {
        _gameScene->showCrashCloud();

        auto collisionSource = tile.tileRect.origin;
        float tileX = tile.tilePositionX;
        float tileY = tile.tilePositionY;
        auto tileCoordinate = cocos2d::Point(tileX, tileY);
        if (!checkInteractionObjectCollision(collisionSource, tileCoordinate) && canBreakFloor())
        {
            setDesiredPosition(
                cocos2d::Point(getDesiredPosition().x, getDesiredPosition().y + intersection.size.height));
            setVelocity(cocos2d::Point(getVelocity().x, 0.0));
            setJumpState(GameObject::JumpStateEnum::NO_JUMP);
        }
    }
    else if (getDesiredPosition().y < getPosition().y)  // TODO test if this
                                                        // always works
    {
        // tile is directly below character
        setDesiredPosition(cocos2d::Point(getDesiredPosition().x, getDesiredPosition().y + intersection.size.height));
        setVelocity(cocos2d::Point(getVelocity().x, 0.0));
        setOnGround(true);
        // TODO check if code belwo applys to player
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

void Player::defaultCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{
    auto tiledMap = _gameScene->getTileMap();
    auto mapTileSize = tiledMap->getTileSize();
    const auto& mapSize = tiledMap->getMapSize();

    if (intersection.size.width > intersection.size.height && intersection.size.width > mapTileSize.width / 2)
    {
        // TODO recheck but results were not nice
        // tile is diagonal, but resolving collision vertially
        //                setVelocity(cocos2d::Point(getVelocity().x,
        //                0.0)); float resolutionHeight =
        //                -intersection.size.height; if (index > 5)
        //                {
        //                  resolutionHeight = intersection.size.height;
        //                  setOnGround(true);
        //                }
        //                setDesiredPosition(
        //                    cocos2d::Point(getDesiredPosition().x,
        //                    getDesiredPosition().y +
        //                    resolutionHeight));
    }
    else if (intersection.size.height > intersection.size.width && intersection.size.height > mapTileSize.height / 2)
    {
        float resolutionWidth = -intersection.size.width;
        if (tile.tileIdx == TileHelper::tileTopLeft || tile.tileIdx == TileHelper::tileBottomLeft)
        {
            resolutionWidth = intersection.size.width;
        }
        setDesiredPosition(cocos2d::Point(getDesiredPosition().x + resolutionWidth, getDesiredPosition().y));
    }
}

void Player::simpleCollisionCallback()
{
    _gameScene->playerHitByEnemyCallback();
}

void Player::destroyBlock(const cocos2d::Point& screenCoordinate, const cocos2d::Point& tileCoordinate)
{
    auto tiledMap = _gameScene->getTileMap();
    const auto& mapTileSize = tiledMap->getTileSize();
    cocos2d::TMXLayer* layer = tiledMap->getLayer(CONSTANTS.tilemapTileLayer);

    // when player hits through blocks and an enemy is above we need to fake a bullet to kill the enemy
    Bullet* bullet = GAMECONFIG.getBulletObject("bullet")->clone(_gameScene);  // todo this only works when type
    bullet->setIsFriend(true);
    // "bullet"" exists!!!
    bullet->setPosition(screenCoordinate);
    bullet->setContentSize(mapTileSize);
    bullet->setVisible(true);
    bullet->setDisabled(false);
    bullet->setContentSize(bullet->getContentSize() * 2.0F);  // increase collision size
    bullet->collisionShoot();
    layer->removeTileAt(tileCoordinate);
    _gameScene->explodeGameObject(bullet);
}

bool Player::checkInteractionObjectCollision(const cocos2d::Point& screenCoordinate,
                                             const cocos2d::Point& tileCoordinate)
{
    auto tiledMap = _gameScene->getTileMap();
    cocos2d::TMXLayer* layer = tiledMap->getLayer(CONSTANTS.tilemapTileLayer);
    auto tile = layer->getTileAt(tileCoordinate);
    cocos2d::TMXObjectGroup* grp = tiledMap->getObjectGroup(CONSTANTS.tilemapInteractionObjectGroup);

    if (tile != nullptr && grp != nullptr)
    {
        auto objects = grp->getObjects();
        for (auto& object : objects)
        {
            auto properties = object.asValueMap();
            auto position =
                cocos2d::Point(properties.at("x").asInt() * cocos2d::Director::getInstance()->getContentScaleFactor(),
                               properties.at("y").asInt() * cocos2d::Director::getInstance()->getContentScaleFactor());

            if (position == screenCoordinate)
            {
                std::string name = properties.at("name").asString();
                if (name == CONSTANTS.tilemapInteractionObjectDestroy)
                {
                    destroyBlock(screenCoordinate, tileCoordinate);
                    return true;
                }
                else if (name == CONSTANTS.tilemapInteractionObjectJump)
                {
                    return true;
                }
                else if (name == CONSTANTS.tilemapInteractionObjectClimb)
                {
                    return true;  // TODO animation
                }
                return false;
            }
        }
    }
    return false;
}
// MARK: - helpers
void Player::jumpFromObject()
{
    _jumpState = WANTS_TO_OBJECT_JUMP;
    //    _onGround = true;
    runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(0.1f),
                                        cocos2d::CallFuncN::create(CC_CALLBACK_0(Player::resetJumpState, this)),
                                        nullptr));
}

void Player::resetJumpState()
{
    _jumpState = NO_JUMP;
}

void Player::prepareToShoot()
{
    if (_shoot < 1)
    {
        auto result =
            std::find_if_not(_ammoVector.begin(), _ammoVector.end(), [&](const auto& v) { return v->isVisible(); });
        if (result != _ammoVector.end())
        {
            _shoot = 10;  // CONF.randomValueBetween(275,325);
            shootBullet(*result, getHeadingState());
            return;
        }
    }
}

bool Player::increaseLife()  // TODO currently only player!
{
    setLife(_life + 1);
    if (_life > GAMECONFIG.getGameplayConfig().playerMaxLife)
    {
        setLife(GAMECONFIG.getGameplayConfig().playerMaxLife);
        return false;
    }
    return true;
}

void Player::attack()
{
    if (_attackTime <= 0)
    {
        _attackTime = 7;
        // display attack
        if (getHeadingState() == LEFT_HEADING)
        {
            checkAndStartAnimation(AnimationHelper::AnimationTagEnum::ATTACK_LEFT_ANIMATION, false);
        }
        else
        {
            checkAndStartAnimation(AnimationHelper::AnimationTagEnum::ATTACK_RIGHT_ANIMATION, false);  // TODO flipX ?
        }
    }
}

void Player::restore()
{
    _onGround = false;
    _disabled = false;
    _sleeping = false;
    _velocity = cocos2d::Point(0, 0);
    _position = _lastGroundPosition;
    _desiredPosition = _lastGroundPosition;
    setJumpState(NO_JUMP);
    setMovementState(IDLE_MOVEMENT);
    setObstacle(NO_OBSTACLE);
    setHeadingState(RIGHT_HEADING);
    _hit = 40;
}

// MARK: - getters and setters

bool Player::canBreakFloor()
{
    return _additionalButton == CONSTANTS.buttonTypeDown;
}

bool Player::canShoot()
{
    return !_ammoVector.empty();
}

void Player::setObstacle(ObstacleStateEnum dir)
{
    _obstacle = dir;
}

bool Player::isAttacking()
{
    return _attackTime > 0;
}

int Player::getId()
{
    return _id;
}
