/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef GAMEOBJECTS_BULLET_H_
#define GAMEOBJECTS_BULLET_H_

#include "../GameData/GameConfig.h"
#include "GameObject.h"

class Bullet;
class GameScene;

class Bullet : public GameObject
{
  public:
    static Bullet* create(const std::string& filepath, float xSpeed, float ySpeed, bool rotationAnimation, int lifeTime,
                          bool flipAnimationX, std::string audio,
                          std::map<AnimationHelper::AnimationTagEnum, std::string> animationEnumMap);

    static Bullet* createWithSpriteFrameName(const std::string& spriteFrameName);
    virtual Bullet* clone(GameScene* gameScene) const override;
    virtual void update(float delta) override;

    // MARK: functions

    /**
     * simpleCollisionCallback is called if we do not care about the whereabouts of the tile
     */
    virtual void simpleCollisionCallback() override;

    virtual bool shootOrAttackCollision(GameObject* p) override
    {
        return false;
    };

    virtual void setObstacle(ObstacleStateEnum obstacle) override;

    void resetLifeTime();

    /**
     * shoot: only friendly fire: bullet collides with game object
     */
    void collisionShoot();

  protected:
    virtual bool playerCollision() override;

    virtual bool tileCollision() override;

  private:
    Bullet();
    // MARK: variables
  private:
    CC_SYNTHESIZE_READONLY(bool, _rotationAnimation, BulletRotation)  // TODO
    CC_SYNTHESIZE_READONLY(int, _lifeTime, LifeTime)
    CC_SYNTHESIZE(bool, _isFriend, IsFriend)

  private:
    int _lifeTimeMax;
    std::string _audio;
};

#endif  // GAMEOBJECTS_BULLET_H_
