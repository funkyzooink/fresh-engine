/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef GAMEOBJECTS_STATICOBJECT_H_
#define GAMEOBJECTS_STATICOBJECT_H_

#include "GameObject.h"
#include "../GameData/GameConfig.h"

class Upgrade : public GameObject
{
  public:
    virtual ~Upgrade() override = default;

    static Upgrade* create(const std::string& filepath, UpgradeActions upgradeActions,
                           std::map<AnimationHelper::AnimationTagEnum, std::string> animationEnumMap);

    static Upgrade* createWithSpriteFrameName(const std::string& spriteFrameName);
    virtual Upgrade* clone(GameScene* gameScene) const override;
    virtual void update(float delta) override;

    UpgradeActions getActions();

    // MARK: collision functions

    virtual bool shootOrAttackCollision(GameObject* p) override
    {
        return false;
    };  // Do nothing

    virtual void setObstacle(ObstacleStateEnum obstacle) override;

  protected:
    virtual bool playerCollision() override;
    virtual void wakeUp() override;
    virtual void goToSleep() override;

  private:
    Upgrade();
    UpgradeActions _actions;
};

#endif  // GAMEOBJECTS_STATICOBJECT_H_
