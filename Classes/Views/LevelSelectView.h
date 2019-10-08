/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef VIEWS_LEVELSELECTVIEW_H_
#define VIEWS_LEVELSELECTVIEW_H_

#include "2d/CCLayer.h"

// forward declarations
struct WorldParentConfig;

class LevelSelectView : public cocos2d::Layer
{
    // MARK: cocos2dx inherited
  private:
    CREATE_FUNC(LevelSelectView);

    LevelSelectView();

    // cocos2dx inherited

  public:
    static LevelSelectView* createLayer();
    virtual bool init() override;
    void showPage(int id);
    virtual void onEnterTransitionDidFinish() override;

    static const int _worldViewId;

  private:
    void showTiles();
    void fadeIn();
    void menuTouchCallback(Ref* pSender, int arg);
    cocos2d::Node* createStar();

    CC_SYNTHESIZE_READONLY(int, _pageIndex, Page)
};

#endif  // VIEWS_LEVELSELECTVIEW_H_
