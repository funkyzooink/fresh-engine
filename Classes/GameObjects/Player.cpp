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

Player::Player() : _canKill("no"), _attackTime(-10), _hit(0), _id(0), _shoot(0)
{
    _sleeping = false;
    setHeadingState(RIGHT_HEADING);
}

Player* Player::create(const std::string& name, const std::string& idleIcon, int id, float xSpeed, float ySpeed,
                       const std::string& canKill, const std::string& customButton1, const std::string& customButton2,
                       std::vector<std::string> bulletTypes, bool flipAnimationX, std::string upgrade,
                       std::map<AnimationHelper::AnimationTagEnum, std::string> animationEnumMap)
{
    Player* sprite = createWithSpriteFrameName(idleIcon);
    sprite->_animationEnumMap = std::move(animationEnumMap);

    sprite->_name = name;
    sprite->_idleIcon = idleIcon;
    sprite->_id = id;  // TODO remove
    sprite->_upgrade = std::move(upgrade);
    sprite->_xSpeed = xSpeed;   // 1000.0f;
    sprite->_ySpeed = ySpeed;   // 750.0f;
    sprite->setGravity(true);   // TODO
    sprite->setCanJump(true);   // TODO
    sprite->_canAttack = true;  // TODO

    sprite->_ammoVector.clear();

    sprite->_canKill = canKill;
    sprite->_flipAnimationX = flipAnimationX;
    sprite->setLife(GAMECONFIG.getGameplayConfig().playerMaxLife);

    sprite->_customButton1 = std::move(customButton1);
    sprite->_customButton2 = std::move(customButton2);

    sprite->_bulletTypes = std::move(bulletTypes);
    return sprite;
}
Player* Player::clone(GameScene* gameScene) const
{
    auto player = Player::create(_name, _idleIcon, _id, _xSpeed, _ySpeed, _canKill, _customButton1, _customButton2,
                                 _bulletTypes, _flipAnimationX, _upgrade, _animationEnumMap);
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

    cocos2d::Rect collisionSource = getBoundingBox();
    // make rectangle a little bigger in the Heading direction
    if (getHeadingState() == HeadingStateEnum::RIGHT_HEADING) {
        collisionSource.origin.x = collisionSource.origin.x + (CONSTANTS.getOffset() / 100);
        collisionSource.origin.y = collisionSource.origin.y + (CONSTANTS.getOffset() / 100);
        collisionSource.size.width= collisionSource.size.width + (CONSTANTS.getOffset() / 100);
        collisionSource.size.height = collisionSource.size.height + (CONSTANTS.getOffset() / 100);
    }
    else if (getHeadingState() == HeadingStateEnum::LEFT_HEADING) {
        collisionSource.origin.x = collisionSource.origin.x - (CONSTANTS.getOffset() / 100);
        collisionSource.origin.y = collisionSource.origin.y - (CONSTANTS.getOffset() / 100);
        collisionSource.size.width= collisionSource.size.width + (CONSTANTS.getOffset() / 100);
        collisionSource.size.height = collisionSource.size.height + (CONSTANTS.getOffset() / 100);
    }

    auto interaction = checkInteractionObjectCollision(collisionSource);
    if (interaction == InteractionCollisionEnum::DESTROY)
    {
        auto tiledMap = _gameScene->getTileMap();
        const auto& mapTileSize = tiledMap->getTileSize();
        const auto& mapSize = tiledMap->getMapSize();

        if (getHeadingState() == HeadingStateEnum::RIGHT_HEADING) {
            collisionSource.origin.x = collisionSource.origin.x + collisionSource.size.width;
            collisionSource.origin.y = collisionSource.origin.y + collisionSource.size.height;
        }
        else if (getHeadingState() == HeadingStateEnum::LEFT_HEADING) {
            collisionSource.origin.x = collisionSource.origin.x - collisionSource.size.width;
            collisionSource.origin.y = collisionSource.origin.y + collisionSource.size.height;
        }
        cocos2d::Point gameObjectTilePosition = TileHelper::tileCoordinateForPosition(collisionSource.origin, mapSize, mapTileSize);
//        float tileX = tile.tilePositionX;
//        float tileY = tile.tilePositionY;
//        auto tileCoordinate = cocos2d::Point(tileX, tileY);
        destroyBlock(collisionSource.origin, gameObjectTilePosition);
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
    auto collisionSource = tile.tileRect;

    auto interaction = checkInteractionObjectCollision(collisionSource);
    if (interaction == InteractionCollisionEnum::DESTROY)
    {
        float tileX = tile.tilePositionX;
        float tileY = tile.tilePositionY;
        auto tileCoordinate = cocos2d::Point(tileX, tileY);
        destroyBlock(collisionSource.origin, tileCoordinate);
    }
    else if (interaction == InteractionCollisionEnum::NO_INTERACTION)  // nothing
    {
        setDesiredPosition(cocos2d::Point(getDesiredPosition().x, getDesiredPosition().y - intersection.size.height));
        setVelocity(cocos2d::Point(getVelocity().x, 0.0));
    }
    else if (interaction == InteractionCollisionEnum::CLIMB)  // TODOshow climb animation
    {
    }
}
void Player::belowCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{
    setObstacle(GameObject::ObstacleStateEnum::NO_OBSTACLE);
    if (getJumpState() == GameObject::JumpStateEnum::FALLING_DOWN_JUMP && getOnGround() == false)  // TODO shuffle
    {
        _gameScene->showCrashCloud();

        auto collisionSource = tile.tileRect;
        auto interaction = checkInteractionObjectCollision(collisionSource);
        // TODO ladder down
        if (interaction == InteractionCollisionEnum::DESTROY && canBreakFloor())  // break though
        {
            float tileX = tile.tilePositionX;
            float tileY = tile.tilePositionY;
            auto tileCoordinate = cocos2d::Point(tileX, tileY);
            destroyBlock(collisionSource.origin, tileCoordinate);
        }
    }
    else if (getDesiredPosition().y < getPosition().y)  // TODO test if this
                                                        // always works
    {
        auto collisionSource = tile.tileRect;
        auto interaction = checkInteractionObjectCollision(collisionSource);

        if (interaction == InteractionCollisionEnum::NO_INTERACTION ||
            interaction == InteractionCollisionEnum::JUMP)  // in case of jump through do not fall down again
        {
            // tile is directly below character
            setDesiredPosition(
                cocos2d::Point(getDesiredPosition().x, getDesiredPosition().y + intersection.size.height));
            setVelocity(cocos2d::Point(getVelocity().x, 0.0));
            setOnGround(true);
            // TODO check if code below applies to player
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
void Player::leftCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{  // if movableobject walks left stop him
    auto collisionSource = tile.tileRect;
    auto interaction = checkInteractionObjectCollision(collisionSource);
    if (interaction != InteractionCollisionEnum::WALK && interaction != InteractionCollisionEnum::CLIMB)
    {
        setObstacle(GameObject::ObstacleStateEnum::LEFT_OBSTACLE);
        if (getDesiredPosition().x < getPosition().x)
            setDesiredPosition(cocos2d::Point(getPosition().x, getDesiredPosition().y));
    }
}

void Player::rightCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{  // if movableobject walks right stop him
    auto collisionSource = tile.tileRect;
    auto interaction = checkInteractionObjectCollision(collisionSource);
    if (interaction != InteractionCollisionEnum::WALK && interaction != InteractionCollisionEnum::CLIMB)
    {
        setObstacle(GameObject::ObstacleStateEnum::RIGHT_OBSTACLE);
        if (getDesiredPosition().x > getPosition().x)
            setDesiredPosition(cocos2d::Point(getPosition().x, getDesiredPosition().y));
    }
}
void Player::defaultCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{
    auto tiledMap = _gameScene->getTileMap();
    auto mapTileSize = tiledMap->getTileSize();

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
    auto tile = layer->getTileAt(tileCoordinate);

    if (tile != nullptr) {
        // when player hits through blocks and an enemy is above we need to fake a bullet to kill the enemy
        Bullet* bullet = GAMECONFIG.getBulletObject("bullet")->clone(_gameScene);  // todo this only works when type "bullet"" exists!!!
        bullet->setIsFriend(true);
        bullet->setPosition(screenCoordinate);
        bullet->setContentSize(mapTileSize);
        bullet->setVisible(true);
        bullet->setDisabled(false);
        bullet->setContentSize(bullet->getContentSize() * 2.0F);  // increase collision size
        bullet->collisionShoot();
        layer->removeTileAt(tileCoordinate);
        _gameScene->explodeGameObject(bullet);
    }
}

Player::InteractionCollisionEnum Player::checkInteractionObjectCollision(const cocos2d::Rect& screenCoordinate)
{
    auto tiledMap = _gameScene->getTileMap();
    cocos2d::TMXObjectGroup* grp = tiledMap->getObjectGroup(CONSTANTS.tilemapInteractionObjectGroup);

    if (//tile != nullptr && 
    grp != nullptr)
    {
        auto objects = grp->getObjects();
        for (auto& object : objects)
        {

            auto properties = object.asValueMap(); // TODO paint this rectangle for debbuging
            auto position =
                cocos2d::Rect(properties.at("x").asInt() * cocos2d::Director::getInstance()->getContentScaleFactor(),
                               properties.at("y").asInt() * cocos2d::Director::getInstance()->getContentScaleFactor(),
                               properties.at("width").asInt() * cocos2d::Director::getInstance()->getContentScaleFactor(),
                               properties.at("height").asInt() * cocos2d::Director::getInstance()->getContentScaleFactor());

            // make rectangle a little smaller so that tiles touching are not counted
            position.origin.x = position.origin.x + (CONSTANTS.getOffset() / 100);
            position.origin.y = position.origin.y + (CONSTANTS.getOffset() / 100);
            position.size.width= position.size.width - (CONSTANTS.getOffset() / 100);
            position.size.height = position.size.height - (CONSTANTS.getOffset() / 100);

            if (position.intersectsRect(screenCoordinate))
            {
                std::string name = properties.at("name").asString();
                if (name == CONSTANTS.tilemapInteractionObjectDestroy)
                {
                    return InteractionCollisionEnum::DESTROY;
                }
                else if (name == CONSTANTS.tilemapInteractionObjectJump)
                {
                    return InteractionCollisionEnum::JUMP;
                }
                else if (name == CONSTANTS.tilemapInteractionObjectClimb)
                {
                    return InteractionCollisionEnum::CLIMB;  // TODO animation
                }
                else if (name == CONSTANTS.tilemapInteractionObjectWalk)
                {
                    return InteractionCollisionEnum::WALK;
                }
                return InteractionCollisionEnum::NO_INTERACTION;
            }
        }
    }
    return InteractionCollisionEnum::NO_INTERACTION;
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
            checkAndStartAnimation(AnimationHelper::AnimationTagEnum::ATTACK_RIGHT_ANIMATION, true);  // TODO flipX ninja?
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
    return _customButton1 == CONSTANTS.buttonTypeDown || _customButton2 == CONSTANTS.buttonTypeDown;
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
