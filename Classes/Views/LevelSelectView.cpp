/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "LevelSelectView.h"

#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"
#include "../Helpers/AnimationHelper.h"
#include "../Helpers/AudioPlayer.h"
#include "../Helpers/Localization.h"
#include "../Helpers/Utility.h"
#include "../Scenes/GameScene.h"
#include "../Scenes/MainScene.h"

#include "cocos2d.h"

const int LevelSelectView::_worldViewId = 20;  // TODO!!

LevelSelectView::LevelSelectView() : _pageIndex(_worldViewId)
{
}

LevelSelectView* LevelSelectView::createLayer()
{
    // 'layer' is an autorelease object
    LevelSelectView* layer = LevelSelectView::create();

    // return the layer
    return layer;
}

bool LevelSelectView::init()
{
    if (!Layer::init())
    {
        return false;
    }

    return true;
}

void LevelSelectView::onEnterTransitionDidFinish()
{
    showPage(_pageIndex);
}

cocos2d::Node* LevelSelectView::createStar()
{
    auto starSprite1 = cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.iconStar);
    starSprite1->setScale(0.7F);
    starSprite1->setLocalZOrder(CONSTANTS.LocalZOrderEnum::NO_Z_ORDER);
    starSprite1->setColor(cocos2d::Color3B::YELLOW);

    return starSprite1;
}

void LevelSelectView::showTiles()
{
    std::vector<std::string> vList;
    if (_pageIndex == _worldViewId)
    {
        auto worlds = GAMECONFIG.getWorldOverview();
        std::transform(worlds.begin(), worlds.end(), std::back_inserter(vList),
                       [](std::pair<int, WorldConfig> const& pair) { return pair.second.levelSelectIcon; });
    }
    else
    {
        auto levels = GAMECONFIG.getWorldConfig(_pageIndex).levels;
        std::transform(levels.begin(), levels.end(), std::back_inserter(vList),
                       [&](const auto& v) { return v.levelSelectIcon; });
    }

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    auto menu = cocos2d::Menu::create();
    menu->setPosition(cocos2d::Vec2::ZERO);
    addChild(menu);

    for (int i = 0; i < vList.size(); ++i)
    {
        cocos2d::MenuItemSprite* sprite;
        cocos2d::Node* starNode = Node::create();
        starNode->setVisible(false);
        addChild(starNode);

        if (_pageIndex == _worldViewId)
        {
            if (GAMEDATA.isWorldUnlocked(i))
            {
                sprite = Utility::addSpriteButton(vList.at(i), cocos2d::Vec2::ZERO,
                                                  CC_CALLBACK_1(LevelSelectView::menuTouchCallback, this, i));
            }
            else
            {
                sprite = Utility::addSpriteButton(CONSTANTS.iconLock, cocos2d::Vec2::ZERO,
                                                  CC_CALLBACK_1(LevelSelectView::menuTouchCallback, this, i));
            }
        }
        else
        {
            if (GAMEDATA.isLevelUnlocked(_pageIndex, i))
            {
                sprite = Utility::addSpriteButton(vList.at(i), cocos2d::Vec2::ZERO,
                                                  CC_CALLBACK_1(LevelSelectView::menuTouchCallback, this, i));

                // stars
                auto starSprite1 = createStar();
                starSprite1->setPosition(
                    cocos2d::Vec2(-starSprite1->getContentSize().width, -starSprite1->getContentSize().height));

                auto starSprite2 = createStar();
                starSprite2->setPosition(
                    cocos2d::Vec2(starSprite1->getContentSize().width * 0, -starSprite1->getContentSize().height));

                auto starSprite3 = createStar();
                starSprite3->setPosition(
                    cocos2d::Vec2(starSprite1->getContentSize().width * 1, -starSprite1->getContentSize().height));

                int money = 0;
                int enemies = 0;
                int heart = 0;
                GAMEDATA.getStarsForLevel(_pageIndex, i, money, enemies, heart);
                if (money == 0)
                {
                    starSprite1->setColor(cocos2d::Color3B::BLACK);
                }
                else
                {
                    starSprite1->setColor(cocos2d::Color3B::YELLOW);
                }
                if (enemies == 0)
                {
                    starSprite2->setColor(cocos2d::Color3B::BLACK);
                }
                else
                {
                    starSprite2->setColor(cocos2d::Color3B::YELLOW);
                }
                if (heart == 0)
                {
                    starSprite3->setColor(cocos2d::Color3B::BLACK);
                }
                else
                {
                    starSprite3->setColor(cocos2d::Color3B::YELLOW);
                }

                starNode->setScale(0.5F);
                starNode->addChild(starSprite1);
                starNode->addChild(starSprite2);
                starNode->addChild(starSprite3);
            }
            else
            {
                sprite = Utility::addSpriteButton(CONSTANTS.iconLock, cocos2d::Vec2::ZERO,
                                                  CC_CALLBACK_1(LevelSelectView::menuTouchCallback, this, i));
            }
        }
        auto positionX = (i + 1) * sprite->getContentSize().width + visibleSize.width / 10;
        sprite->setPosition(cocos2d::Vec2(positionX, visibleSize.height / 2));
        starNode->setPosition(cocos2d::Vec2(positionX, visibleSize.height / 2 - sprite->getContentSize().height / 2));
        starNode->runAction(cocos2d::FadeIn::create(0.15F));
        menu->addChild(sprite);
    }
}

void LevelSelectView::menuTouchCallback(Ref* pSender, int arg)
{
    if (_pageIndex == _worldViewId)
    {
        if (GAMEDATA.isWorldUnlocked(arg))
        {
            showPage(arg);
            _pageIndex = arg;
        }
    }
    else
    {
        if (GAMEDATA.isLevelUnlocked(_pageIndex, arg))
        {
            AudioPlayer::stopMusic();

            auto scene = GameScene::createScene(_pageIndex, arg);
            auto transition = AnimationHelper::sceneTransition(scene);
            cocos2d::Director::getInstance()->replaceScene(transition);
        }
    }
}

void LevelSelectView::showPage(int id)
{
    runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.15F),
                                        cocos2d::CallFuncN::create(CC_CALLBACK_0(LevelSelectView::fadeIn, this)),
                                        nullptr));

    _pageIndex = id;
}

void LevelSelectView::fadeIn()
{
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    removeAllChildren();
    auto titleLabel = Utility::addLabel("0", CONSTANTS.fontSubtitle, CONSTANTS.getOffset() / 2,
                                        GAMECONFIG.getMainSceneConfig()->fontColor);
    titleLabel->setPosition(visibleSize.width / 15, visibleSize.height - titleLabel->getContentSize().height * 1.7);
    titleLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 1);
    addChild(titleLabel);

    if (_pageIndex == _worldViewId)
    {
        titleLabel->setString(Localization::getLocalizedString("worlds"));
    }
    else
    {
        titleLabel->setString(GAMECONFIG.getWorldConfig(_pageIndex).title);
    }

    showTiles();
}
