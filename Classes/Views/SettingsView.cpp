/****************************************************************************
 Copyright (c) 2014-2019 Gabriel Heilig
 fresh-engine
 funkyzooink@gmail.com
 ****************************************************************************/
#include "SettingsView.h"

#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"
#include "../Helpers/AudioPlayer.h"
#include "../Helpers/Localization.h"
#include "../Helpers/Utility.h"
#include "../Helpers/UserDefaults.h"
#include "../Scenes/MainScene.h"

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "ui/UIRichText.h"

SettingsView::SettingsView()
  : _musicMenuItem(nullptr), _effectMenuItem(nullptr), _containerHeight(0.0F), _scrollView(nullptr)
{
}

SettingsView* SettingsView::createLayer()
{
    // 'layer' is an autorelease object
    SettingsView* layer = SettingsView::create();

    // return the layer
    return layer;
}

bool SettingsView::init()
{
    if (!Layer::init())
    {
        return false;
    }

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    // TODO xml credits
    //  std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename("json/credits.xml");
    //  std::string xml = cocos2d::FileUtils::getInstance()->getStringFromFile(fullPath.c_str());
    //  auto textNode = cocos2d::ui::RichText::createWithXML(xml);
    auto textNode = Utility::addLabel(Localization::getLocalizedString("credits_text"), CONSTANTS.fontSubtitle,
                                      CONSTANTS.getOffset() / 2, GAMECONFIG.getMainSceneConfig()->fontColor);
    textNode->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    textNode->setDimensions(visibleSize.width * 7 / 10, 0.0F);
    _containerHeight = visibleSize.height * 2 + textNode->getContentSize().height * 1.5F;
    textNode->setPosition(cocos2d::Vec2(0.0F, _containerHeight - visibleSize.height));
    textNode->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 1);

    _scrollView = cocos2d::ui::ScrollView::create();
    _scrollView->setContentSize(cocos2d::Size(visibleSize.width * 7 / 10, visibleSize.height));
    _scrollView->setInnerContainerSize(cocos2d::Size(visibleSize.width * 8 / 10, _containerHeight));
    _scrollView->setBounceEnabled(true);
    _scrollView->setTouchEnabled(true);
    _scrollView->setScrollBarEnabled(false);

    _scrollView->setDirection(cocos2d::ui::ScrollView::Direction::VERTICAL);
    _scrollView->setPosition(cocos2d::Vec2(visibleSize.width / 10, 0.0f));
    addChild(_scrollView);
    _scrollView->addChild(textNode);

    auto menu = cocos2d::Menu::create();
    menu->setPosition(cocos2d::Vec2::ZERO);
    addChild(menu);
    _musicMenuItem = Utility::addMusicButton(CC_CALLBACK_1(SettingsView::toggleMusicCallback, this));
    menu->addChild(_musicMenuItem);
    _effectMenuItem = Utility::addFxButton(CC_CALLBACK_1(SettingsView::toggleFxCallback, this));
    menu->addChild(_effectMenuItem);
    return true;
}

void SettingsView::showPage()
{
    _scrollView->jumpToTop();
    _scrollView->scrollToBottom(_containerHeight / 50, true);
}

// TODO same as popup
void SettingsView::toggleMusicCallback(cocos2d::Ref* sender)
{
    if (GAMEDATA.getMusicEnabled())
    {
        AudioPlayer::stopMusic();

        _musicMenuItem->setNormalImage(cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconMusicDown));
        _musicMenuItem->setSelectedImage(cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconMusicNormal));

        GAMEDATA.setMusicEnabled(false);
    }
    else
    {
        GAMEDATA.setMusicEnabled(true);
        AudioPlayer::playMenuMusic();

        _musicMenuItem->setNormalImage(cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconMusicNormal));
        _musicMenuItem->setSelectedImage(cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconMusicDown));
    }

    UserDefaults::saveMusicFxToUserdefaults(GAMEDATA.getMusicEnabled());
}

void SettingsView::toggleFxCallback(cocos2d::Ref* sender)
{
    if (!GAMEDATA.getAudioFxEnabled())
    {
        _effectMenuItem->setNormalImage(cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconAudioFXNormal));
        _effectMenuItem->setSelectedImage(cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconAudioFXDown));
    }
    else
    {
        _effectMenuItem->setNormalImage(cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconAudioFXDown));
        _effectMenuItem->setSelectedImage(
            cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconAudioFXNormal));
    }

    GAMEDATA.setAudioFxEnabled(!GAMEDATA.getAudioFxEnabled());
    UserDefaults::saveAudioFxToUserdefaults(GAMEDATA.getAudioFxEnabled());
}
