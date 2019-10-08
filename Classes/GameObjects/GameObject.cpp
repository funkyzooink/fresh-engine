/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "GameObject.h"

#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"
#include "../Helpers/AnimationHelper.h"
#include "../Helpers/AudioPlayer.h"
#include "../Helpers/TileHelper.h"
#include "../Helpers/Utility.h"
#include "../Scenes/GameScene.h"
#include "Bullet.h"
#include "cocos2d.h"

GameObject::GameObject()
  : _disabled(false)
  , _sleeping(true)
  , _jumpState(NO_JUMP)
  , _movementState(IDLE_MOVEMENT)
  , _lastGroundPosition(cocos2d::Point::ZERO)
  , _desiredPosition(cocos2d::Point::ZERO)
  , _velocity(cocos2d::Point::ZERO)
  , _obstacle(NO_OBSTACLE)
  , _canAttack(false)
  , _onGround(false)
  , _flipAnimationX(false)
  , _xSpeed(0)
  , _ySpeed(0)
  , _heading(LEFT_HEADING)
  , _canJump(false)
  , _gravity(true)
  , _life(0)
  , _gameScene(nullptr)
{
    //	drawBoundingBox();
}

// MARK: - update

void GameObject::update(float dt)
{
    _velocity =
        cocos2d::Point(_velocity.x * 0.90, _velocity.y);  // velocity is constantly increasing -> slow down a little

    addJump();                                     // y
    addXMovement(dt);                              // x
    addGravity(dt);                                // y
    _velocity = Utility::clampMovment(_velocity);  // clamp velocity to a min max
    addDirection(dt);                              // set Direction and Initiate movement

    setAnimation();
}

void GameObject::addJump()
{
    if (_canJump)
    {
        cocos2d::Point jumpForce = cocos2d::Point(0.0, _ySpeed * CONSTANTS.getOffset());

        if (_jumpState == WANTS_TO_JUMP && _onGround)
        {
            _velocity = _velocity + jumpForce;
            _jumpState = JUMP;  // isJumping
            _onGround = false;
        }
        else if (_jumpState == WANTS_TO_DOUBLE_JUMP)  // dbl jump
        {
            _velocity = _velocity + (jumpForce * 0.5);
            _jumpState = DOUBLE_JUMP;
            _onGround = false;
        }
        else if (_jumpState == WANTS_TO_OBJECT_JUMP)  // obj jump
        {
            _velocity = _velocity + (jumpForce * 0.25);
            _jumpState = OBJECT_JUMP;
            _onGround = false;
        }
        else if (_jumpState == FALLING_DOWN_JUMP && !_onGround)
        {
            _velocity = _velocity - (jumpForce * 10);
            _onGround = false;
        }
        else if (_jumpState == BACK_ON_GROUND_JUMP)  // dbl jump
        {
            _jumpState = NO_JUMP;
        }
    }
}

void GameObject::addXMovement(float dt)
{
    if (getMovementState() != IDLE_MOVEMENT && !(_jumpState == FALLING_DOWN_JUMP && !_onGround))
    {
        cocos2d::Point forwardMove = cocos2d::Point(_xSpeed * CONSTANTS.getOffset(), 0.0);
        cocos2d::Point forwardStep = forwardMove * dt;
        _velocity = _velocity + forwardStep;
    }
}

void GameObject::addGravity(float dt)
{
    if (_gravity)
    {
        cocos2d::Point gravity = cocos2d::Point(0.0, -CONSTANTS.getOffset() * 10.0);
        cocos2d::Point gravityStep = gravity * dt;
        _velocity = _velocity + gravityStep;
    }
}

void GameObject::addDirection(float dt)
{
    cocos2d::Point stepVelocity = _velocity * dt;
    if (getHeadingState() == LEFT_HEADING)
    {
        stepVelocity.x = -stepVelocity.x;
    }
    else if (getMovementState() == IDLE_MOVEMENT)
    {
        stepVelocity.x = 0;
    }

    if (_onGround)  // save last known position
        _lastGroundPosition = _position - stepVelocity;

    _desiredPosition = _position + stepVelocity;
}

// MARK: - collision

bool GameObject::playerCollision()
{
    return !(!isVisible() || _disabled);
}

bool GameObject::tileCollision()
{
    return !(!isVisible() || getDisabled() || getSleeping());
}

// MARK: - collision callbacks

void GameObject::aboveCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{
}
void GameObject::belowCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{
}
void GameObject::leftCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{  // if movableobject walks left stop him
    setObstacle(GameObject::ObstacleStateEnum::LEFT_OBSTACLE);
    if (getDesiredPosition().x < getPosition().x)
        setDesiredPosition(cocos2d::Point(getPosition().x, getDesiredPosition().y));
}

void GameObject::rightCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{  // if movableobject walks right stop him
    setObstacle(GameObject::ObstacleStateEnum::RIGHT_OBSTACLE);
    if (getDesiredPosition().x > getPosition().x)
        setDesiredPosition(cocos2d::Point(getPosition().x, getDesiredPosition().y));
}

void GameObject::defaultCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection)
{
}

void GameObject::simpleCollisionCallback()
{
}

// MARK: - helpers
void GameObject::removeObject()
{
    removeFromParent();
}
void GameObject::showDestroyAnimationAndRemove()
{
    float duration = getPosition().y / CONSTANTS.getOffset() / 5;
    float nrRot = getPosition().y / CONSTANTS.getOffset() * 2;
    stopAllActions();
    runAction(cocos2d::Repeat::create(cocos2d::RotateBy::create(0.5F, -360), nrRot));
    auto moveTo = cocos2d::MoveTo::create(duration, cocos2d::Vec2(getPosition().x * 0.99, 0 - getContentSize().height));
    if (dynamic_cast<Bullet*>(this))
    {
        runAction(cocos2d::Sequence::create(moveTo, cocos2d::Hide::create(),
                                            nullptr));  // Hide bullet for reuse
    }
    else
    {
        runAction(cocos2d::Sequence::create(moveTo, cocos2d::Hide::create(),
                                            cocos2d::CallFuncN::create(CC_CALLBACK_0(GameObject::removeObject, this)),
                                            nullptr));
    }
}

void GameObject::addAmmo(std::string bulletType, GameScene* gameScene, bool isFriend)
{
    auto config = GAMECONFIG.getBulletObject(bulletType);
    auto bullet = config->clone(gameScene);
    bullet->setVisible(false);
    bullet->setIsFriend(isFriend);
    _ammoVector.push_back(bullet);
}

bool GameObject::increaseLife()  // TODO currently only player!
{
    return false;
}

bool GameObject::decreaseLife()
{
    setLife(_life - 1);
    if (_life <= 0)
    {
        return false;
    }

    showHitAnimation();
    return true;
}

void GameObject::shootBullet(Bullet* ammo, HeadingStateEnum dir)
{
    ammo->stopAllActions();
    ammo->setRotation(0.0F);
    ammo->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    ammo->setVisible(true);
    ammo->setDisabled(false);
    ammo->setSleeping(false);
    ammo->resetLifeTime();
    ammo->runAction(cocos2d::Sequence::create(cocos2d::FadeIn::create(0.0001), cocos2d::Show::create(), nullptr));
    ammo->setPosition(getPosition().x, getPosition().y);
    ammo->setHeadingState(dir);
    if (dir == LEFT_HEADING)
    {
        if (ammo->getBulletRotation())
        {
            ammo->runAction(cocos2d::RepeatForever::create(cocos2d::RotateBy::create(0.5F, -360.0F)));
        }
    }
    else if (dir == RIGHT_HEADING)
    {
        if (ammo->getBulletRotation())
        {
            ammo->runAction(cocos2d::RepeatForever::create(cocos2d::RotateBy::create(0.5F, 360.0F)));
        }
    }
    AudioPlayer::playFx(CONSTANTS.audioShoot);
}

// MARK: - animations
void GameObject::stopAllMovmentAnimations()
{
    runAction(cocos2d::FlipX::create(false));
    for (int i = AnimationHelper::AnimationTagEnum::IDLE_LEFT_ANIMATION;
         i <= AnimationHelper::AnimationTagEnum::HIT_RIGHT_ANIMATION; ++i)
    {
        stopActionByTag(i);
    }
}

void GameObject::startIdleAnimation()
{
    checkAndStartAnimation(AnimationHelper::AnimationTagEnum::IDLE_RIGHT_ANIMATION, _flipAnimationX);
}

void GameObject::showHitAnimation()
{
    // display hit animation
    if (getHeadingState() == LEFT_HEADING)  // left
    {
        checkAndStartAnimation(AnimationHelper::AnimationTagEnum::HIT_LEFT_ANIMATION, false);
    }
    else
    {
        checkAndStartAnimation(AnimationHelper::AnimationTagEnum::HIT_RIGHT_ANIMATION, _flipAnimationX);
    }
}

void GameObject::checkAndStartAnimation(AnimationHelper::AnimationTagEnum animation, bool flipX)
{
    if (!getActionByTag(animation))
    {
        stopAllMovmentAnimations();
        if (flipX)
        {
            runAction(cocos2d::FlipX::create(flipX));
        }
        auto action = AnimationHelper::getActionForTag(_animationEnumMap[animation]);
        if (action != nullptr)
        {
            action->setTag(animation);
            runAction(action);
        }
        else
        {
#if COCOS2D_DEBUG > 0
            auto message = "Wrong Animation key: " + Utility::numberToString(animation);
            cocos2d::log("%s", message.c_str());
#endif
        }
    }
}

void GameObject::setAnimation()
{
    if (_onGround || !_gravity)
    {
        if (getMovementState() == IDLE_MOVEMENT)
        {
            if (getHeadingState() == LEFT_HEADING)
            {
                checkAndStartAnimation(AnimationHelper::AnimationTagEnum::IDLE_LEFT_ANIMATION, false);
            }
            else
            {
                checkAndStartAnimation(AnimationHelper::AnimationTagEnum::IDLE_RIGHT_ANIMATION, _flipAnimationX);
            }
        }
        else if (getMovementState() == WALK_MOVEMENT)
        {
            if (getHeadingState() == LEFT_HEADING)
            {
                checkAndStartAnimation(AnimationHelper::AnimationTagEnum::WALK_LEFT_ANIMATION, false);
            }
            else if (getHeadingState() == RIGHT_HEADING)
            {
                checkAndStartAnimation(AnimationHelper::AnimationTagEnum::WALK_RIGHT_ANIMATION, _flipAnimationX);
            }
        }
    }
    else
    {
        if (_desiredPosition.y < _position.y && (_canJump || getCanAttack()))
        {
            if (getHeadingState() == LEFT_HEADING)
            {
                checkAndStartAnimation(AnimationHelper::AnimationTagEnum::FALL_LEFT_ANIMATION, false);
            }
            else  // default always right
            {
                checkAndStartAnimation(AnimationHelper::AnimationTagEnum::FALL_RIGHT_ANIMATION, _flipAnimationX);
            }
        }
        else if (_desiredPosition.y > _position.y)  // up
        {
            if (getHeadingState() == LEFT_HEADING)
            {
                checkAndStartAnimation(AnimationHelper::AnimationTagEnum::JUMP_LEFT_ANIMATION, false);
            }
            else  // default always right
            {
                checkAndStartAnimation(AnimationHelper::AnimationTagEnum::JUMP_RIGHT_ANIMATION, _flipAnimationX);
            }
        }
    }
}
