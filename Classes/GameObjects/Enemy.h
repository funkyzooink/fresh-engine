/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef GAMEOBJECTS_ENEMY_H_
#define GAMEOBJECTS_ENEMY_H_

#include "../GameData/GameConfig.h"
#include "GameObject.h"

class Bullet;
class GameScene;

class Enemy : public GameObject
{
  public:
    static Enemy* create(const std::string& idleIcon, float xSpeed, float ySpeed, int life,
                         std::vector<std::string> bulletTypes, bool gravity, bool canFly, bool canAttack, bool canJump,
                         bool canMove, bool flipAnimationX, std::string audio,
                         std::map<AnimationHelper::AnimationTagEnum, std::string> animationEnumMap);
    static Enemy* createWithSpriteFrameName(const std::string& spriteFrameName);

    virtual Enemy* clone(GameScene* gameScene) const override;
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
    void prepareToShoot();

  protected:
    virtual bool playerCollision() override;
    virtual bool tileCollision() override;
    virtual void wakeUp() override;
    virtual void goToSleep() override;

  private:
    Enemy();

    // update helpers
    void updateFly();
    void updateAttackAndShoot();
    void updateMove(float dt);
    // MARK: variables
  private:
    CC_SYNTHESIZE_READONLY(bool, _canMove, CanMove)
    CC_SYNTHESIZE_READONLY(bool, _canFly, CanFly)  // TODO a lot of those are only called once
    CC_SYNTHESIZE(bool, _isFriend, IsFriend)

  private:
    int _attackCoolDownCounter;
    int _counter;  // block range, attacker attack time, shooter next shoot time
    int _soundIndex;
    std::string _audio;
};

#endif  // GAMEOBJECTS_ENEMY_H_
