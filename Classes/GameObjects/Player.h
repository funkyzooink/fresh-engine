/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef GAMEOBJECTS_PLAYER_H_
#define GAMEOBJECTS_PLAYER_H_

#include "GameObject.h"

class Enemy;

class Player : public GameObject
{
  public:
    // MARK: cocos2dx inherited
    static Player* create(const std::string& name, const std::string& filepath, int id, float xSpeed, float ySpeed,
                          const std::string& canKill, const std::string& customButton1,
                          const std::string& customButton2, std::vector<std::string> bulletTypes, bool flipAnimationX,
                          std::string upgrade, std::map<AnimationHelper::AnimationTagEnum, std::string> animationMap);
    static Player* createWithSpriteFrameName(const std::string& spriteFrameName);
    virtual Player* clone(GameScene* gameScene) const override;
    virtual void update(float delta) override;
    // cocos2dx inherited

    // MARK: collision functions
    virtual bool shootOrAttackCollision(GameObject* p) override
    {
        return false;
    };  // TODO should not happen

    /**
     * aboveCollisionCallback is called in case of a tile above the GameObject
     */
    virtual void aboveCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection) override;

    /**
     * belowCollisionCallback is called in case of a tile below the GameObject
     */
    virtual void belowCollisionCallback(const CollisionTile& tile, const cocos2d::Rect& intersection) override;

    /**
     * defaultCollisionCallback is called in case of a diagonal tile
     */
    virtual void defaultCollisionCallback(const CollisionTile& tile,
                                          const cocos2d::Rect& intersection) override;  // TODO remove or diagonal

    /**
     * simpleCollisionCallback is called if we do not care about the whereabouts of the tile
     */
    virtual void simpleCollisionCallback() override;

    virtual bool tileCollision() override;

    void destroyBlock(const cocos2d::Point& screenCoordinate, const cocos2d::Point& tileCoordinate);
    /**
     * check if there is a tile element which can be destroyed
     *
     * @param Point screenCoordinate  coordinate in screenspace
     * @param Point tileCoordinate  coordinate of tile
     */
    bool checkInteractionObjectCollision(const cocos2d::Point& screenCoordinate, const cocos2d::Point& tileCoordinate);
    /**
     * attack
     *
     * @param GameObject p  a bullet or Player
     */
    void collisionAttack();

    /**
     * collision with Hazard Layer
     *
     * @param GameObject p  currently only player
     */
    void collisionHazardTiles();

    // MARK: functions
    int getId();
    virtual bool increaseLife() override;
    void attack();
    void jumpFromObject();
    void restore();
    void prepareToShoot();
    void resetJumpState();
    bool canBreakFloor();
    bool canShoot();
    virtual void setObstacle(ObstacleStateEnum dir) override;
    bool isAttacking();

    std::string _upgrade;  // TODO set get

    // MARK: variables
  private:
    Player();
    CC_SYNTHESIZE(std::string, _customButton1, CustomButton1)
    CC_SYNTHESIZE(std::string, _customButton2, CustomButton2)
    CC_SYNTHESIZE_READONLY(std::string, _canKill, CanKill)
    CC_SYNTHESIZE(int, _hit, Hit)

    int _attackTime;
    int _id;
    int _shoot;
};

#endif  // GAMEOBJECTS_PLAYER_H_
