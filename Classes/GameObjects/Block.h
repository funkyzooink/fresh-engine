/****************************************************************************
 Copyright (c) 2014-2019 Gabriel Heilig
 fresh-engine
 funkyzooink@gmail.com
 ****************************************************************************/
#ifndef GAMEOBJECTS_BLOCK_H_
#define GAMEOBJECTS_BLOCK_H_

#include "../GameData/GameConfig.h"
#include "GameObject.h"

class Bullet;
class GameScene;

class Block : public GameObject
{
  public:
    static Block* create(const std::string& idleIcon, float xSpeed, float ySpeed, bool canAttack, bool canMove,
                         bool flipAnimationX, std::string audio,
                         std::map<AnimationHelper::AnimationTagEnum, std::string> animationEnumMap);
    static Block* createWithSpriteFrameName(const std::string& spriteFrameName);

    virtual Block* clone(GameScene* gameScene) const override;
    virtual void update(float delta) override;

    // MARK: functions
    virtual bool shootOrAttackCollision(GameObject* p) override;
    /**
     * aboveCollisionCallback is called in case of a tile above the GameObject
     */
    virtual void aboveCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection) override;

    /**
     * belowCollisionCallback is called in case of a tile below the GameObject
     */
    virtual void belowCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection) override;

    virtual void setObstacle(ObstacleStateEnum obstacle) override;
    bool checkSleeping();
    void attack();

  protected:
    virtual bool playerCollision() override;
    virtual bool tileCollision() override;
    virtual void wakeUp() override;
    virtual void goToSleep() override;

  private:
    Block();
    void addVelocity(float delta);
    // MARK: variables
  private:
    CC_SYNTHESIZE_READONLY(bool, _canMove, CanMove)
    CC_SYNTHESIZE(bool, _isFriend, IsFriend)

  private:
    int _counter;  // block range, attacker attack time, shooter next shoot time
    std::string _audio;
    int _attackCoolDownCounter;
    int _range;
};

#endif  // GAMEOBJECTS_BLOCK_H_
