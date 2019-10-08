/****************************************************************************
 Copyright (c) 2014-2019 Gabriel Heilig
 fresh-engine
 funkyzooink@gmail.com
 ****************************************************************************/
#include "MainView.h"

#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"
#include "../Helpers/Localization.h"
#include "../Helpers/Utility.h"
#include "../Helpers/UserDefaults.h"
#include "../Scenes/MainScene.h"

#include "cocos2d.h"

MainView::MainView() : _mainScene(nullptr)
{
}

MainView* MainView::createLayer(MainScene* mainScene)
{
    // 'layer' is an autorelease object
    MainView* layer = MainView::create();
    layer->_mainScene = mainScene;

    // return the layer
    return layer;
}

bool MainView::init()
{
    if (!Layer::init())
    {
        return false;
    }

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    // text
    auto gameTitleLabel =
        Utility::addLabel(Localization::getLocalizedString(GAMECONFIG.getMainSceneConfig()->title), CONSTANTS.fontTitle,
                          CONSTANTS.getOffset(), GAMECONFIG.getMainSceneConfig()->fontColor);
    if (gameTitleLabel->getContentSize().width > visibleSize.width)
    {
        // scale the title to a maximum of 90% of the screenwidth
        gameTitleLabel->setScale(visibleSize.width / gameTitleLabel->getContentSize().width * 0.9F);
    }
    gameTitleLabel->setPosition(
        cocos2d::Vec2(visibleSize.width / 2 - gameTitleLabel->getContentSize().width / 2 * gameTitleLabel->getScale(),
                      visibleSize.height * 0.60));
    gameTitleLabel->setAlignment(cocos2d::TextHAlignment::CENTER);
    gameTitleLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::MOVABLE_Z_ORDER);
    addChild(gameTitleLabel);

    //    /////////////////////////////
    //
    auto playPauseItem = Utility::addSpriteButton(CONSTANTS.buttonIconPlayNormal,
                                                  cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2),
                                                  CC_CALLBACK_1(MainView::menuPlayCallback, this));

    auto menuPlayPause = cocos2d::Menu::create(playPauseItem, nullptr);
    menuPlayPause->setPosition(cocos2d::Vec2::ZERO);
    addChild(menuPlayPause);

    //    /////////////////////////////
    // rate on mainscreen

    auto sharingConfig = GAMECONFIG.getSharingconfig();
    if (!sharingConfig.empty())
    {
        auto rateMenu = cocos2d::Menu::create();
        rateMenu->setPosition(cocos2d::Vec2::ZERO);
        rateMenu->setVisible(true);
        addChild(rateMenu);

        for (int i = 0; i < sharingConfig.size(); ++i)
        {
            auto share = sharingConfig.at(i);
            auto menuItem = Utility::addSpriteButton(share.icon, cocos2d::Vec2::ZERO,
                                                     CC_CALLBACK_1(MainView::menuShareCallback, this, i));

            menuItem->setPosition(
                cocos2d::Vec2(visibleSize.width - CONSTANTS.getOffset() * 1,
                              CONSTANTS.getOffset() / 2 + menuItem->getContentSize().height * (i + 1)));

            auto rateLabel =
                Utility::addLabel(Localization::getLocalizedString(share.title), CONSTANTS.fontNormal,
                                  CONSTANTS.getOffset() * 0.25, GAMECONFIG.getMainSceneConfig()->fontColor);
            rateLabel->setPosition(cocos2d::Vec2(menuItem->getPosition().x - menuItem->getContentSize().width * 0.6 -
                                                     rateLabel->getContentSize().width,
                                                 menuItem->getPosition().y - menuItem->getContentSize().height / 4));
            rateLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::MOVABLE_Z_ORDER);
            addChild(rateLabel);

            if (GAMEDATA.getSharingReward(share.title))
            {
                rateLabel->setVisible(false);
            }

            rateMenu->setLocalZOrder(CONSTANTS.LocalZOrderEnum::MOVABLE_Z_ORDER);
            rateMenu->addChild(menuItem);
        }
    }

    ////////////
    auto settingsItem = Utility::addSpriteButton(CONSTANTS.buttonIconSettings, cocos2d::Vec2::ZERO,
                                                 CC_CALLBACK_1(MainView::menuSettingsCallback, this));

    settingsItem->setPosition(cocos2d::Vec2(visibleSize.width - CONSTANTS.getOffset() * 1, CONSTANTS.getOffset() / 2));

    auto menuFx = cocos2d::Menu::create(settingsItem, nullptr);
    menuFx->setPosition(cocos2d::Vec2::ZERO);
    menuFx->setLocalZOrder(CONSTANTS.LocalZOrderEnum::MOVABLE_Z_ORDER);
    addChild(menuFx);

    return true;
}

void MainView::menuShareCallback(Ref* pSender, int arg)
{
    auto sharing = GAMECONFIG.getSharingconfig().at(arg);
    cocos2d::Application::getInstance()->openURL(sharing.url);

    if (!GAMEDATA.getSharingReward(sharing.title))
    {
        GAMEDATA.setPlayerTotalCash(GAMEDATA.getPlayerTotalCash() + sharing.reward);

        UserDefaults::saveSharingRewardsToUserdefaults(sharing.title);
    }
}

void MainView::menuSettingsCallback(Ref* pSender)
{
    _mainScene->showPage(MainScene::MainSceneType::SETTINGS);
}

void MainView::menuPlayCallback(Ref* pSender)
{
    _mainScene->showPage(MainScene::MainSceneType::LEVELSELECT);
}
