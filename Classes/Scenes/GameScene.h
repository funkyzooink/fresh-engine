/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef SCENES_GAMESCENE_H_
#define SCENES_GAMESCENE_H_

#include "2d/CCLayer.h"

class BackgroundNode;
class Player;
class GameObject;
class HUD;

namespace cocos2d
{
class Ref;
class Touch;
class Event;
class DrawNode;
class Label;
class Node;
class Size;
class Sprite;
class TMXTiledMap;
}  // namespace cocos2d

class GameScene : public cocos2d::Layer
{
  public:
    enum class ResumeTypeEnum
    {
        revive,
        pause,
        gameover,
        dissmissLevelInfo
    };
    // MARK: cocos2dx inherited
  public:
    virtual void applicationDidEnterBackground();

  private:
    virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches,
                                cocos2d::Event* event) override;  // Touch Callback
    virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event) override;
    virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event) override;

    virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;
    virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;

    virtual void onAcceleration(cocos2d::Acceleration* acceleration, cocos2d::Event* event) override;
    // cocos2dx inherited

    // MARK: functions
  public:
    GameScene();
    virtual ~GameScene() override;
    static cocos2d::Scene* createScene(int world, int level);

    /**
     * public for pop up access
     */
    void resumeSceneCallback(ResumeTypeEnum type);

    /**
     * public for game object access
     */
    const cocos2d::Vec2& getPlayerPosition();

    /**
     * Game Over public for game object collision
     *
     * @param gameWon  true if won, false if lost
     *
     */
    void gameOverCallback(bool gameWon);

    /**
     * public for game object collision
     *
     * @param GameObject to destroy
     *
     */
    void explodeGameObject(GameObject* p);

    /**
     * public for game object collision
     */
    void playerHitByEnemyCallback();

    /**
     * public for game object collision
     */
    void destroyGameObject(GameObject* p);

  private:
    virtual bool init() override;
    CREATE_FUNC(GameScene);

    void menuPauseCallback(cocos2d::Ref* pSender);  // Pause Button Callback
    void actionLeft();
    void actionRight();
    void actionCustom(bool move, const std::string& customAction);
    void handleTouchArea(const std::string& touchType, bool move);
    void handleTouch(cocos2d::Touch* touch, bool move);

    /**
     * dissmisLevelinfoCallback and Start Game
     */
    void dissmisLevelinfoCallback();

    void update(float dt) override;
    void loadMap();
    void initParallaxBackground();
    void initTiledMap();
    void initTmxObjects();

    /**
     * helper to add a Gameobject to the camera and _gameObjectVector
     */
    void addGameObject(GameObject* movableObject, const cocos2d::Point& position);

    /**
     * Player just died, check if revive possible and ask user
     */
    void playerLost();

    /**
     * User wants to revive -> resume Game
     */
    void resumeReviveGame();

    /**
     * display tmx info Layer
     */
    void updateTutorial();

    /**
     * disable a game object - needed for a delayed actions
     */
    void disableGameobject(bool dis, GameObject* p);
    // MARK: variables
  private:
    HUD* _hudLayer;
    BackgroundNode* _backgroundNode;
    Player* _player;
    bool _activePopup;
    cocos2d::DrawNode* _playerCrashRectangle;
    cocos2d::Label* _tutorialInfoLabel;
    cocos2d::Node* _cameraFollowNode;
    cocos2d::Sprite* _crashCloudSprite;
    cocos2d::Sprite* _enemyHitSprite;
    cocos2d::TMXTiledMap* _tiledMap;
    double _gameTime;
    int _worldID;
    int _enemyCounter;
    int _enemyMaxCounter;
    int _moneyCounter;
    int _moneyMaxCounter;
    int _tutorialInfoLabelIndex;
    std::vector<std::string> _allowedPlayerTypes;

    CC_SYNTHESIZE_READONLY(int, _levelID, LevelID)

  public:  // TODO for new collision
    void upgradePlayer(int playerId);
    void upgradePlayerForKey(const std::string& playerId);

    void showCrashCloud();

    // MARK: - getters and setters
    void increaseMoney();
    void increaseLife();
    Player* getPlayer();
    cocos2d::TMXTiledMap* getTileMap();
    cocos2d::Vector<Node*> getGameObjectVector();
};

#endif  // SCENES_GAMESCENE_H_
