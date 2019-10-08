/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef SCENES_BOOTSCENE_H_
#define SCENES_BOOTSCENE_H_

#include "2d/CCLayer.h"

namespace cocos2d
{
class Ref;
class Touch;
class Sprite;
}  // namespace cocos2d

class BootScene : public cocos2d::Layer
{
    // MARK: cocos2dx inherited
  public:
    virtual ~BootScene() override = default;
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches,
                                cocos2d::Event* event) override;  // Touch Callback
  private:
    BootScene();
    CREATE_FUNC(BootScene);
    // MARK: functions
    void menuPlayCallback(Ref* pSender);
    // MARK: variables
    cocos2d::Sprite* _backgroundSprite;
};

#endif  // SCENES_BOOTSCENE_H_
