/****************************************************************************
 Copyright (c) 2014-2019 Gabriel Heilig
 fresh-engine
 funkyzooink@gmail.com
 ****************************************************************************/
#ifndef VIEWS_MAINVIEW_H_
#define VIEWS_MAINVIEW_H_

#include "2d/CCLayer.h"

class MainScene;
namespace cocos2d
{
class Menu;
class MenuItemSprite;
}  // namespace cocos2d

class MainView : public cocos2d::Layer
{
    // MARK: cocos2dx inherited
  private:
    CREATE_FUNC(MainView);
    MainView();
    // cocos2dx inherited

    void menuSettingsCallback(Ref* pSender);
    void menuPlayCallback(cocos2d::Ref* pSender);
    void menuShareCallback(Ref* pSender, int arg);

  public:
    static MainView* createLayer(MainScene* mainScene);
    virtual bool init() override;

  private:
    MainScene* _mainScene;
};

#endif  // VIEWS_MAINVIEW_H_
