/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef VIEWS_HUD_H_
#define VIEWS_HUD_H_

#include "../GameObjects/Player.h"
#include "2d/CCLayer.h"

namespace cocos2d
{
class Label;
class Sprite;
}  // namespace cocos2d

class HUD : public cocos2d::Layer
{
  public:
    virtual ~HUD() override;

    virtual bool init() override;  // init
    static HUD* createLayer();

    // set Labels
    void setEnemies(const std::string& message);
    void setCoins(const std::string& message);
    void setTime(double time);
    void setLife(const int life);
    void initControls();  // init arrow up/down signs

    void setCustomButton1(const std::string& customButton);
    void setCustomButton2(const std::string& customButton);

  private:
    HUD();
    CREATE_FUNC(HUD);
    void addLabels();

    // Labels
    cocos2d::Label* _moneyLabel;
    cocos2d::Label* _enemyLabel;
    cocos2d::Label* _timerLabel;

    cocos2d::Sprite* _cashSprite;
    cocos2d::Sprite* _enemySprite;

    // Touch control
    cocos2d::Sprite* _touchArea3Sprite;
    cocos2d::Sprite* _touchArea4Sprite;

    std::vector<cocos2d::Sprite*> _heartList;
    int _lifeCounter;
    float _iconScaleFactor;
};

#endif  // VIEWS_HUD_H_
