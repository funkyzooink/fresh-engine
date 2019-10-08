/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef GAMEOBJECTS_GAMEOBJECT_H_
#define GAMEOBJECTS_GAMEOBJECT_H_

#include "2d/CCSprite.h"
#include "platform/CCPlatformMacros.h"
#include "../Helpers/AnimationHelper.h"

class GameScene;
class Bullet;
struct CollisionTile;

class GameObject : public cocos2d::Sprite
{
    // MARK: enums
  public:
    /**
     * HeadingStateEnum - Gameobject can look left or right
     */
    enum HeadingStateEnum
    {
        LEFT_HEADING,
        RIGHT_HEADING
    };

    /**
     * ObstacleStateEnum - Gameobject can have no obstacle, left or right
     */
    enum ObstacleStateEnum
    {
        NO_OBSTACLE,
        LEFT_OBSTACLE,
        RIGHT_OBSTACLE
    };

    /**
     * ObstacleStateEnum - Gameobject can move or not // TODO possible to combine with jump?
     */
    enum MovementStateEnum
    {
        IDLE_MOVEMENT,
        WALK_MOVEMENT
    };

    /**
     * JumpStateEnum
     */
    enum JumpStateEnum
    {
        NO_JUMP,
        WANTS_TO_JUMP,
        JUMP,
        WANTS_TO_DOUBLE_JUMP,
        DOUBLE_JUMP,
        WANTS_TO_OBJECT_JUMP,
        OBJECT_JUMP,
        FALLING_DOWN_JUMP,
        BACK_ON_GROUND_JUMP
    };

    // MARK: functions
    GameObject();
    virtual ~GameObject() override = default;

    virtual void update(float dt) override;

    virtual GameObject* clone(GameScene* gameScene) const = 0;

    /**
     * aboveCollisionCallback is called in case of a tile above the GameObject
     */
    virtual void aboveCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection);

    /**
     * belowCollisionCallback is called in case of a tile below the GameObject
     */
    virtual void belowCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection);

    /**
     * leftCollisionCallback is called in case of a tile left of the GameObject
     */
    virtual void leftCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection);

    /**
     * rightCollisionCallback is called in case of a tile right of the GameObject
     */
    virtual void rightCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection);

    /**
     * defaultCollisionCallback is called in case of a diagonal tile
     */
    virtual void defaultCollisionCallback(const CollisionTile& tile,
                                          const cocos2d::Rect& intersection);  // TODO remove or diagonal

    /**
     * simpleCollisionCallback is called if we do not care about the whereabouts of the tile
     */
    virtual void simpleCollisionCallback();

    /**
     * set an approaching obstacle
     */
    virtual void setObstacle(ObstacleStateEnum dir) = 0;

    /**
     *  return true when life can be decreased, false if object has to die
     */
    virtual bool decreaseLife();
    /**
     * true when life can be increased, fals if not supported by object
     */
    virtual bool increaseLife();
    /**
     * call removeFromParent
     */
    virtual void removeObject();
    /**
     * show animation and call removeObjects when it is finished
     */
    virtual void showDestroyAnimationAndRemove();

    /**
     * collision with bullet or attacking gameobject
     */
    virtual bool shootOrAttackCollision(GameObject* p) = 0;

    /**
     * Helper class to start Idle Animation
     */
    virtual void startIdleAnimation();

  protected:
    /**
     * collision with player
     */
    virtual bool playerCollision();

    /**
     * collision with Tile Layer
     */
    virtual bool tileCollision();  // TODO

    /**
     * wake up if close to player
     */
    virtual void wakeUp(){};
    /**
     * sleep if far away
     */
    virtual void goToSleep(){};

    /**
     * Helper class to stop all movment Animation
     */
    void stopAllMovmentAnimations();

    /**
     * Helper class to start Animation
     */
    void checkAndStartAnimation(AnimationHelper::AnimationTagEnum animation, bool flipX);
    void shootBullet(Bullet* ammo, HeadingStateEnum dir);
    virtual void addAmmo(std::string bulletType, GameScene* gameScene, bool isFriend);

  private:
    void showHitAnimation();
    void setAnimation();
    void addJump();
    void addGravity(float dt);
    void addXMovement(float dt);
    void addDirection(float dt);

    // MARK: variables
  protected:
    CC_SYNTHESIZE(JumpStateEnum, _jumpState, JumpState);
    CC_SYNTHESIZE(MovementStateEnum, _movementState, MovementState);
    CC_SYNTHESIZE_READONLY(cocos2d::Point, _lastGroundPosition, LastGroundPosition);
    CC_SYNTHESIZE(cocos2d::Point, _desiredPosition, DesiredPosition);
    CC_SYNTHESIZE(cocos2d::Point, _velocity, Velocity);

    CC_SYNTHESIZE_READONLY(std::vector<Bullet*>, _ammoVector, AmmoVector);
    CC_SYNTHESIZE_READONLY(ObstacleStateEnum, _obstacle, Obstacle);
    CC_SYNTHESIZE(bool, _onGround, OnGround);
    CC_SYNTHESIZE(bool, _canAttack, CanAttack);

    CC_SYNTHESIZE_READONLY(std::string, _idleIcon, Type)
    CC_SYNTHESIZE(bool, _disabled, Disabled);
    CC_SYNTHESIZE(bool, _sleeping, Sleeping);

    CC_SYNTHESIZE(bool, _flipAnimationX, FlipAnimationX);
    CC_SYNTHESIZE(float, _xSpeed, XSpeed);
    CC_SYNTHESIZE(float, _ySpeed, YSpeed);
    CC_SYNTHESIZE(std::vector<std::string>, _bulletTypes, BulletTypes);
    std::map<AnimationHelper::AnimationTagEnum, std::string> _animationEnumMap;

    /**
     * each gameobject needs a reference to a gamescene for callbacks
     */
    GameScene* _gameScene;

  private:
    CC_SYNTHESIZE(HeadingStateEnum, _heading, HeadingState);
    CC_SYNTHESIZE(bool, _canJump, CanJump);
    CC_SYNTHESIZE(bool, _gravity, Gravity);
    CC_SYNTHESIZE(int, _life, Life);
};

#endif  // GAMEOBJECTS_GAMEOBJECT_H_
