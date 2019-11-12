/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "HUD.h"

#include <utility>

#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"
#include "../Helpers/Utility.h"

#include "cocos2d.h"

HUD::HUD()
  : _moneyLabel(nullptr)
  , _enemyLabel(nullptr)
  , _timerLabel(nullptr)
  , _cashSprite(nullptr)
  , _enemySprite(nullptr)
  , _touchArea3Sprite(nullptr)
  , _touchArea4Sprite(nullptr)
  , _lifeCounter(0)
  , _iconScaleFactor(0.0)
{
}

HUD::~HUD() = default;

HUD* HUD::createLayer(std::string coins, std::string enemies)
{
    HUD* hud = HUD::create();
    hud->addLabels(std::move(coins), std::move(enemies));
    hud->setColor(cocos2d::Color3B(0, 0, 0));

    hud->setContentSize(cocos2d::Size(800, 40));

    return hud;
}

bool HUD::init()
{
    initControls();

    _iconScaleFactor = 0.625F;

    return true;
}

void HUD::addLabels(const std::string& coins, const std::string& enemies)
{
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    ////hud
    // time
    _timerLabel = Utility::addLabel("", CONSTANTS.fontHud, CONSTANTS.getOffset() / 4, GAMECONFIG.getTextColor(),
                                    cocos2d::Point(visibleSize.width - CONSTANTS.getOffset() * 2.0,
                                                   visibleSize.height - CONSTANTS.getOffset() * 0.5));
    _timerLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::PLAYER_Z_ORDER);
    addChild(_timerLabel);

    // life
    _lifeCounter = GAMECONFIG.getGameplayConfig().playerMaxLife;

    for (int j = 1; j < _lifeCounter; ++j)
    {
        cocos2d::Sprite* redHeart = cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.hudIconHeart);
        redHeart->setPosition(_timerLabel->getPosition().x - (_lifeCounter - j) * redHeart->getContentSize().width,
                              _timerLabel->getPosition().y + redHeart->getContentSize().height * 0.5);
        redHeart->setLocalZOrder(CONSTANTS.LocalZOrderEnum::PLAYER_Z_ORDER);
        redHeart->setScale(_iconScaleFactor);
        addChild(redHeart);
        _heartList.push_back(redHeart);
    }

    // txt
    _enemyLabel = Utility::addLabel(enemies, CONSTANTS.fontHud, CONSTANTS.getOffset() / 4, GAMECONFIG.getTextColor());
    _enemyLabel->setPosition(_heartList.at(0)->getPosition().x - _heartList.at(0)->getContentSize().width -
                                 _enemyLabel->getContentSize().width,
                             _timerLabel->getPosition().y);
    _enemyLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::PLAYER_Z_ORDER);
    addChild(_enemyLabel);

    _enemySprite = cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.hudIconEnemy);
    _enemySprite->setPosition(_enemyLabel->getPosition().x - _enemySprite->getContentSize().width * _iconScaleFactor,
                              _timerLabel->getPosition().y +
                                  _enemySprite->getContentSize().height * 0.5 * _iconScaleFactor);
    _enemySprite->setLocalZOrder(CONSTANTS.LocalZOrderEnum::PLAYER_Z_ORDER);
    _enemySprite->setScale(_iconScaleFactor);
    addChild(_enemySprite);

    // money sign
    _moneyLabel = Utility::addLabel(coins, CONSTANTS.fontHud, CONSTANTS.getOffset() / 4, GAMECONFIG.getTextColor());
    _moneyLabel->setPosition(_enemySprite->getPosition().x - _enemySprite->getContentSize().width -
                                 _moneyLabel->getContentSize().width,
                             _timerLabel->getPosition().y);
    _moneyLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::PLAYER_Z_ORDER);
    addChild(_moneyLabel);

    _cashSprite = cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.hudIconCoin);
    _cashSprite->setPosition(_moneyLabel->getPosition().x - _cashSprite->getContentSize().width * _iconScaleFactor,
                             _timerLabel->getPosition().y +
                                 _cashSprite->getContentSize().height * 0.5 * _iconScaleFactor);
    _cashSprite->setLocalZOrder(CONSTANTS.LocalZOrderEnum::PLAYER_Z_ORDER);
    _cashSprite->setScale(_iconScaleFactor);
    addChild(_cashSprite);

    auto rectangleHeight = MAX(_enemySprite->getContentSize().height * _iconScaleFactor * 2,
                               _cashSprite->getContentSize().height * _iconScaleFactor * 2);
    auto rectangleWithBorder = cocos2d::DrawNode::create();
    rectangleWithBorder->drawSolidRect(cocos2d::Vec2(0, visibleSize.height - rectangleHeight),
                                       cocos2d::Vec2(visibleSize), cocos2d::Color4F(0.0F, 0.0F, 0.0F, 0.2f));
    rectangleWithBorder->setLocalZOrder(CONSTANTS.LocalZOrderEnum::NO_Z_ORDER);
    addChild(rectangleWithBorder);
}

void HUD::setEnemies(const std::string& message)
{
    _enemyLabel->setString(message);
    _enemySprite->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05F, 1.5F * _iconScaleFactor),
                                                      cocos2d::ScaleTo::create(0.05f, _iconScaleFactor), nullptr));
}

void HUD::setCoins(const std::string& message)
{
    _moneyLabel->setString(message);
    _cashSprite->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05F, 1.5f * _iconScaleFactor),
                                                     cocos2d::ScaleTo::create(0.05F, _iconScaleFactor), nullptr));
}

void HUD::setTime(double time)
{
    std::string seconds = Utility::numberToString((int)(time) % 60);
    std::string minutes = Utility::numberToString((int)(time / 60) % 60);
    std::string message =
        std::string(2 - minutes.length(), '0') + minutes + ":" + std::string(2 - seconds.length(), '0') + seconds;

    _timerLabel->setString(message);
}

void HUD::setLife(const int life)
{
    if (life <= 0)
    {
        return;
    }

    if (_lifeCounter > life)
    {
        for (int i = _lifeCounter - 1; i > life - 1; i--)
        {
            auto* newObject = (cocos2d::Sprite*)_heartList.at(i - 1);
            newObject->setColor(cocos2d::Color3B(112, 79, 79));  // TODO color when hit to json
            newObject->setScale(_iconScaleFactor);
            _lifeCounter--;
        }
    }
    else
    {
        for (int i = _lifeCounter - 1; i < life - 1; i++)
        {
            auto* newObject = (cocos2d::Sprite*)_heartList.at(i);
            newObject->setColor(cocos2d::Color3B::WHITE);  // todo color when full to json
            newObject->setScale(_iconScaleFactor);
            _lifeCounter++;
        }
    }
    for (int i = 0; i < _lifeCounter - 1; ++i)
    {
        auto* newObject = (cocos2d::Sprite*)_heartList.at(i);
        newObject->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.05F, 1.5F * _iconScaleFactor),
                                                       cocos2d::ScaleTo::create(0.05F, _iconScaleFactor), nullptr));
    }
}
void HUD::setCustomButton1(const std::string&)
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32) && (CC_TARGET_PLATFORM != CC_PLATFORM_MAC) &&                            \
    (CC_TARGET_PLATFORM != CC_PLATFORM_LINUX)

    // TODO get from json
    if (== CONSTANTS.buttonTypeAttack)
    {
        _touchArea3Sprite->setSpriteFrame(CONSTANTS.hudIconShoot);
        _touchArea3Sprite->setVisible(true);
    }
    else if (== CONSTANTS.buttonTypeShoot)
    {
        _touchArea3Sprite->setSpriteFrame(CONSTANTS.hudIconShoot);
        _touchArea3Sprite->setVisible(true);
    }
    else if (== CONSTANTS.buttonTypeDown)
    {
        _touchArea3Sprite->setSpriteFrame(CONSTANTS.iconArrow);
        _touchArea3Sprite->setVisible(true);
    }
    else
    {
        _touchArea3Sprite->setVisible(false);
    }
#endif
}

void HUD::setCustomButton2(const std::string& customButton)
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32) && (CC_TARGET_PLATFORM != CC_PLATFORM_MAC) &&                            \
    (CC_TARGET_PLATFORM != CC_PLATFORM_LINUX)

    // TODO get from json
    if (== CONSTANTS.buttonTypeAttack)
    {
        _touchArea4Sprite->setSpriteFrame(CONSTANTS.hudIconShoot);
        _touchArea4Sprite->setVisible(true);
    }
    else if (== CONSTANTS.buttonTypeShoot)
    {
        _touchArea4Sprite->setSpriteFrame(CONSTANTS.hudIconShoot);
        _touchArea4Sprite->setVisible(true);
    }
    else if (== CONSTANTS.buttonTypeDown)
    {
        _touchArea4Sprite->setSpriteFrame(CONSTANTS.iconArrow);
        _touchArea4Sprite->setVisible(true);
    }
    else
    {
        _touchArea4Sprite->setVisible(false);
    }
#endif
}

void HUD::initControls()
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32) && (CC_TARGET_PLATFORM != CC_PLATFORM_MAC) &&                            \
    (CC_TARGET_PLATFORM != CC_PLATFORM_LINUX)
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    auto touchArea1 = GAMECONFIG.getControlConfig(CONSTANTS.touch1).sprite;
    if (!touchArea1.empty())
    {
        auto _touchArea1Sprite = cocos2d::Sprite::createWithSpriteFrameName(touchArea1);
        _touchArea1Sprite->setPosition(visibleSize.width * CONSTANTS.touchArea1 -
                                           _touchArea1Sprite->getContentSize().width / 2,
                                       _touchArea1Sprite->getContentSize().height / 2);
        _touchArea1Sprite->setLocalZOrder(CONSTANTS.LocalZOrderEnum::PLAYER_Z_ORDER);
        _touchArea1Sprite->setOpacity(128);
        addChild(_touchArea1Sprite);
    }

    auto touchArea2 = GAMECONFIG.getControlConfig(CONSTANTS.touch2).sprite;
    if (!touchArea2.empty())
    {
        auto _touchArea2Sprite = cocos2d::Sprite::createWithSpriteFrameName(touchArea2);
        _touchArea2Sprite->setPosition(visibleSize.width * CONSTANTS.touchArea2 +
                                           _touchArea2Sprite->getContentSize().width / 2,
                                       _touchArea2Sprite->getContentSize().width / 2);
        _touchArea2Sprite->setLocalZOrder(CONSTANTS.LocalZOrderEnum::PLAYER_Z_ORDER);
        _touchArea2Sprite->setOpacity(128);
        addChild(_touchArea2Sprite);
    }

    // TODO currently touch Area 3 and 4 are reserved for Custom buttons - and must be created
    _touchArea3Sprite = cocos2d::Sprite::create();
    _touchArea3Sprite->setPosition(visibleSize.width * CONSTANTS.touchArea3 - CONSTANTS.getOffset() / 2,
                                   CONSTANTS.getOffset() / 2);
    _touchArea3Sprite->setLocalZOrder(CONSTANTS.LocalZOrderEnum::PLAYER_Z_ORDER);
    _touchArea3Sprite->setOpacity(128);
    addChild(_touchArea3Sprite);

    _touchArea4Sprite = cocos2d::Sprite::create();
    _touchArea4Sprite->setPosition(visibleSize.width * CONSTANTS.touchArea4 - CONSTANTS.getOffset() / 2,
                                   CONSTANTS.getOffset() / 2);
    _touchArea4Sprite->setLocalZOrder(CONSTANTS.LocalZOrderEnum::PLAYER_Z_ORDER);
    _touchArea4Sprite->setOpacity(128);
    addChild(_touchArea4Sprite);

#endif
}
