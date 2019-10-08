/****************************************************************************
 Copyright (c) 2014-2019 Gabriel Heilig
 fresh-engine
 funkyzooink@gmail.com
 ****************************************************************************/
#ifndef SCENES_MAINSCENE_H_
#define SCENES_MAINSCENE_H_

#include "2d/CCLayer.h"

class BackgroundNode;
class LevelSelectView;
class MainView;
class SettingsView;
namespace cocos2d
{
class Node;
class MenuItemSprite;
}  // namespace cocos2d

class MainScene : public cocos2d::Layer
{
    // MARK: cocos2dx inherited
  public:
    enum MainSceneType
    {
        MAINSCENE,
        SETTINGS,
        LEVELSELECT
    };
    virtual ~MainScene() override;
    static cocos2d::Scene* createScene(MainSceneType type);
    static cocos2d::Scene* createScene(MainSceneType type, int page);
    virtual bool init() override;
    void showPage(MainSceneType type);

  private:
    virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;

    MainScene();
    CREATE_FUNC(MainScene);
    void update(float dt) override;

    // MARK: functions
    void menuExitCallback(cocos2d::Ref* pSender);  // exit Button Callback
    void menuPageCallback(cocos2d::Ref* pSender, int page);
    cocos2d::Node* createTiledBackground(const std::string& tiledMapPath);
    // MARK: variables
    MainSceneType _settingsScenetype;
    cocos2d::MenuItemSprite* _exitButton;

    int _pageId;  // TODO currently needed for level select
    MainView* _mainView;
    SettingsView* _settingsView;
    LevelSelectView* _levelSelectView;
    BackgroundNode* _backgroundNode;
};

#endif  // SCENES_MAINSCENE_H_
