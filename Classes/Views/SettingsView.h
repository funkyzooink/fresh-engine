/****************************************************************************
 Copyright (c) 2014-2019 Gabriel Heilig
 fresh-engine
 funkyzooink@gmail.com
 ****************************************************************************/
#ifndef VIEWS_SETTINGSVIEW_H_
#define VIEWS_SETTINGSVIEW_H_

#include "2d/CCLayer.h"

namespace cocos2d
{
namespace ui
{
class ScrollView;
}
class Menu;
class MenuItemSprite;
}  // namespace cocos2d

class SettingsView : public cocos2d::Layer
{
    // MARK: cocos2dx inherited
  private:
    CREATE_FUNC(SettingsView);

    SettingsView();
    // cocos2dx inherited

    void toggleMusicCallback(cocos2d::Ref* sender);
    void toggleFxCallback(cocos2d::Ref* sender);

  public:
    static SettingsView* createLayer();
    virtual bool init() override;
    void showPage();

  private:
    cocos2d::MenuItemSprite* _musicMenuItem;
    cocos2d::MenuItemSprite* _effectMenuItem;
    float _containerHeight;
    cocos2d::ui::ScrollView* _scrollView;
};

#endif  // VIEWS_SETTINGSVIEW_H_
