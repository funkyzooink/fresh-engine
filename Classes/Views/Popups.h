/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef VIEWS_POPUPS_H_
#define VIEWS_POPUPS_H_

#include "2d/CCLayer.h"
#include "2d/CCMenuItem.h"

class GameScene;

namespace cocos2d
{
class Menu;
class MenuItem;
class MenuItemLabel;
class MenuItemSprite;
class Node;
}  // namespace cocos2d

class Popup : public cocos2d::LayerColor
{
  public:
    Popup(GameScene* gameScene, int world, int level);
    virtual ~Popup() override;

    // create & add an empty cocos2d::Menu
    virtual cocos2d::Menu* addMenu();
    // adds a cocos2d::MenuItem into the cocos2d::Menu

    // button handler functions
    void restartGameCallback(cocos2d::Ref* sender);
    void exitToLevelSelectViewCallback(cocos2d::Ref* sender);

    void toggleMusicCallback(cocos2d::Ref* sender);
    void toggleFxCallback(cocos2d::Ref* sender);

  protected:
    GameScene* _scene;
    cocos2d::Menu* _menu;
    cocos2d::MenuItemSprite* _musicMenuItem;
    cocos2d::MenuItemSprite* _effectMenuItem;

    int _worldID;
    int _levelID;

    // Needed for non touch devices
    std::vector<cocos2d::MenuItem*> _menuItems;
    int _selectedMenuItem;

  private:
    virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;
};

class PausePopup : public Popup
{
  public:
    explicit PausePopup(GameScene* gameScene, int world, int level);
    virtual ~PausePopup() override;

    static PausePopup* create(GameScene* gameScene, int world, int level);

    void resumeGameCallback(cocos2d::Ref* sender);
};

class LevelInfoPopup : public Popup
{
  public:
    LevelInfoPopup(GameScene* gameScene, int world, int level);
    virtual ~LevelInfoPopup() override;

    static LevelInfoPopup* create(GameScene* gameScene, int world, int level);
    void dissmisLevelinfoCallback();
    void levelInfoFadeOutAnimation(cocos2d::Node* node1, cocos2d::Node* node2);
};

class RevivePopup : public Popup
{
  public:
    RevivePopup(GameScene* gameScene, const std::string& text, int world, int level);
    virtual ~RevivePopup() override;

    static RevivePopup* create(GameScene* gameScene, std::string& text, int world, int level);

    void reviveCallback(cocos2d::Ref* sender);
};

class GameOverPopup : public Popup
{
  public:
    GameOverPopup(GameScene* gameScene, bool won, int world, int level);
    virtual ~GameOverPopup() override;

    static GameOverPopup* create(GameScene* gameScene, bool won, int world, int level);

    void showGameoverPopup(const std::string& message, const std::string& cash, int world, int level, bool showStars);
    void nextLevelCallback(cocos2d::Ref* sender);

  private:
    bool playerHasWonTheGame(int world, int level);
};

#endif  // VIEWS_POPUPS_H_
