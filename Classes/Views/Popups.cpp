/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "Popups.h"

#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"
#include "../Helpers/AnimationHelper.h"
#include "../Helpers/AudioPlayer.h"
#include "../Helpers/Localization.h"
#include "../Helpers/UserDefaults.h"
#include "../Helpers/Utility.h"
#include "../Scenes/GameScene.h"
#include "../Scenes/MainScene.h"
#include "cocos2d.h"
// MARK: Popup

Popup::Popup(GameScene* gameScene, int world, int level)
  : _scene(gameScene)
  , _menu(nullptr)
  , _musicMenuItem(nullptr)
  , _effectMenuItem(nullptr)
  , _worldID(world)
  , _levelID(level)
  , _selectedMenuItem(0)
{
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    auto popupFrame = cocos2d::DrawNode::create();
    popupFrame->drawSolidRect(cocos2d::Vec2(0, 0), cocos2d::Vec2(visibleSize.width, visibleSize.height),
                              Utility::colorConverterFloat(GAMECONFIG.getBackgroundColor()));
    popupFrame->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    addChild(popupFrame);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) ||                            \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    glview->setCursorVisible(true);
#endif
}

Popup::~Popup() = default;

cocos2d::Menu* Popup::addMenu()
{
    // return _menu if already created & added
    if (_menu)
    {
        return _menu;
    }

    // create & add the _menu
    _menu = cocos2d::Menu::create();
    _menu->setPosition(cocos2d::Vec2::ZERO);
    addChild(_menu);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) ||                            \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    // keyboard events
    // Currently Desktop only
    auto keyboardListener = cocos2d::EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(Popup::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
#endif

    return _menu;
}
void Popup::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    for (auto& menu : _menuItems)  // TODO only call on up and down
    {
        menu->unselected();
    }
    switch (keyCode)
    {
        case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_S: {
            if (_selectedMenuItem <= _menuItems.size() - 1)
            {
                _selectedMenuItem++;
            }
            else
            {
                _selectedMenuItem = 0;
            }

            _menuItems.at(_selectedMenuItem)->selected();
            break;
        }
        case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_W: {
            if (_selectedMenuItem >= 1)
            {
                _selectedMenuItem--;
            }
            else
            {
                _selectedMenuItem = _menuItems.size() - 1;
            }

            _menuItems.at(_selectedMenuItem)->selected();
            break;
        }
        case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
        case cocos2d::EventKeyboard::KeyCode::KEY_RETURN:
        case cocos2d::EventKeyboard::KeyCode::KEY_ENTER: {
            auto selected = _menuItems.at(_selectedMenuItem);
            selected->activate();
            break;
        }
        default:
            break;
    }
}

void Popup::toggleMusicCallback(cocos2d::Ref* sender)
{
    if (GAMEDATA.getMusicEnabled())
    {
        AudioPlayer::pauseMusic();

        _musicMenuItem->setNormalImage(cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconMusicDown));
        _musicMenuItem->setSelectedImage(cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconMusicNormal));

        GAMEDATA.setMusicEnabled(false);
    }
    else
    {
        GAMEDATA.setMusicEnabled(true);

        AudioPlayer::resumeMusic();

        _musicMenuItem->setNormalImage(cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconMusicNormal));
        _musicMenuItem->setSelectedImage(cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconMusicDown));
    }

    UserDefaults::saveMusicFxToUserdefaults(GAMEDATA.getMusicEnabled());
}

void Popup::toggleFxCallback(cocos2d::Ref* sender)
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

void Popup::restartGameCallback(cocos2d::Ref* sender)
{
    //    removeFromParentAndCleanup(true);
    AudioPlayer::stopMusic();

    auto scene = GameScene::createScene(_worldID, _levelID);
    auto transition = AnimationHelper::sceneTransition(scene);

    cocos2d::Director::getInstance()->replaceScene(transition);
    return;
}

void Popup::exitToLevelSelectViewCallback(cocos2d::Ref* sender)
{
    AudioPlayer::stopMusic();
    AudioPlayer::playMenuMusic();

    auto scene = MainScene::createScene(MainScene::MainSceneType::LEVELSELECT, _worldID);
    auto transition = AnimationHelper::sceneTransition(scene);
    cocos2d::Director::getInstance()->replaceScene(transition);
    return;
}

// MARK: PausePopup

PausePopup::PausePopup(GameScene* gameScene, int world, int level) : Popup(gameScene, world, level)
{
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    // add the title/message of the popup
    auto label = Utility::addLabel(Localization::getLocalizedString("pause_title"), CONSTANTS.fontSubtitle, 36.0F,
                                   GAMECONFIG.getTextColor());
    label->setPosition(visibleSize.width / 2 - label->getContentSize().width / 2, visibleSize.height * 0.7F);
    addChild(label);

    // create menu & buttons
    addMenu();
    auto node1 = Utility::addLabelButton(Localization::getLocalizedString("resume_button"),
                                         cocos2d::Vec2(visibleSize.width * 0.5F, visibleSize.height * 0.55F),
                                         CC_CALLBACK_1(PausePopup::resumeGameCallback, this));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) ||                            \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    // Currently Desktop only
    node1->selected();
#endif
    _menuItems.push_back(node1);
    _menu->addChild(node1);
    auto node2 = Utility::addLabelButton(Localization::getLocalizedString("restart_button"),
                                         cocos2d::Vec2(visibleSize.width * 0.5F, visibleSize.height * 0.45F),
                                         CC_CALLBACK_1(PausePopup::restartGameCallback, this));
    _menuItems.push_back(node2);
    _menu->addChild(node2);
    auto node3 = Utility::addLabelButton(Localization::getLocalizedString("main_menu_button"),
                                         cocos2d::Vec2(visibleSize.width * 0.5F, visibleSize.height * 0.35F),
                                         CC_CALLBACK_1(Popup::exitToLevelSelectViewCallback, this));
    _menuItems.push_back(node3);
    _menu->addChild(node3);

    _musicMenuItem = Utility::addMusicButton(CC_CALLBACK_1(Popup::toggleMusicCallback, this));
    _menuItems.push_back(_musicMenuItem);
    _menu->addChild(_musicMenuItem);
    _effectMenuItem = Utility::addFxButton(CC_CALLBACK_1(Popup::toggleFxCallback, this));
    _menuItems.push_back(_effectMenuItem);
    _menu->addChild(_effectMenuItem);
}

PausePopup::~PausePopup() = default;

PausePopup* PausePopup::create(GameScene* gameScene, int world, int level)
{
    auto popup = new PausePopup(gameScene, world, level);
    if (popup && popup->initWithColor(cocos2d::Color4B(0, 0, 0, 128)))
    {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

void PausePopup::resumeGameCallback(cocos2d::Ref* sender)
{
    _scene->resumeSceneCallback(GameScene::ResumeTypeEnum::pause);
    //    runAction(Sequence::createWithTwoActions(EaseBackIn::create(ScaleTo::create(0.25f,
    //    0.0f)), RemoveSelf::create(true))); // TODO nice animation
    runAction(cocos2d::RemoveSelf::create(true));
}

// MARK: LevelInfoPopup

LevelInfoPopup::LevelInfoPopup(GameScene* gameScene, int world, int level) : Popup(gameScene, world, level)
{
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    // levelinfo
    auto levelInfoNode = cocos2d::Node::create();
    levelInfoNode->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 1);
    addChild(levelInfoNode);

    auto levelInfoLabel =
        Utility::addLabel(GAMECONFIG.getWorldConfig(world).levels.at(level).title, CONSTANTS.fontNormal,
                          CONSTANTS.getOffset() * 0.4, GAMECONFIG.getTextColor());
    levelInfoLabel->setPosition(visibleSize.width / 2 - CONSTANTS.getOffset(),
                                visibleSize.height / 2 + CONSTANTS.getOffset() -
                                    levelInfoLabel->getContentSize().height / 2);
    levelInfoLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 1);
    levelInfoNode->addChild(levelInfoLabel);

    auto worldInfoLabel = Utility::addLabel(GAMECONFIG.getWorldConfig(world).title, CONSTANTS.fontNormal,
                                            CONSTANTS.getOffset() * 0.7, GAMECONFIG.getTextColor());
    worldInfoLabel->setAlignment(cocos2d::TextHAlignment::CENTER);
    worldInfoLabel->setPosition(visibleSize.width / 2 - worldInfoLabel->getContentSize().width / 2,
                                levelInfoLabel->getPosition().y + CONSTANTS.getOffset() * 0.75);
    worldInfoLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 1);
    levelInfoNode->addChild(worldInfoLabel);

    auto blueRectangle = cocos2d::DrawNode::create();
    blueRectangle->drawSolidRect(cocos2d::Vec2(0, visibleSize.height / 2 + CONSTANTS.getOffset() / 2),
                                 cocos2d::Vec2(visibleSize.width, visibleSize.height / 2 + CONSTANTS.getOffset() * 1.5),
                                 cocos2d::Color4F(0.0F, 0.0F, 0.3f, 1.0F));
    blueRectangle->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    blueRectangle->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER);
    blueRectangle->setVisible(false);
    levelInfoNode->addChild(blueRectangle);

    // start animations
    switch (GAMECONFIG.getLevelInfoPopupType())
    {
        case 1:
            AnimationHelper::levelinfoFadeInAnimation(levelInfoLabel, worldInfoLabel);
            runAction(cocos2d::Sequence::create(
                cocos2d::DelayTime::create(2.5F),
                cocos2d::CallFuncN::create(
                    CC_CALLBACK_0(LevelInfoPopup::levelInfoFadeOutAnimation, this, levelInfoLabel, worldInfoLabel)),
                nullptr));
            break;
        case 2:
        default:
            AnimationHelper::levelinfoFadeInAnimation(levelInfoNode, blueRectangle);
            runAction(cocos2d::Sequence::create(
                cocos2d::DelayTime::create(2.5F),
                cocos2d::CallFuncN::create(
                    CC_CALLBACK_0(LevelInfoPopup::levelInfoFadeOutAnimation, this, levelInfoNode, blueRectangle)),
                nullptr));
            break;
    }

    runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(4.0F),
        cocos2d::CallFuncN::create(CC_CALLBACK_0(LevelInfoPopup::dissmisLevelinfoCallback, this)), nullptr));

    AudioPlayer::playFx(CONSTANTS.audioStart);
}
void LevelInfoPopup::levelInfoFadeOutAnimation(cocos2d::Node* node1, cocos2d::Node* node2)
{
    AnimationHelper::levelInfoFadeOutAnimation(node1, node2);
}
LevelInfoPopup::~LevelInfoPopup()
{
}

void LevelInfoPopup::dissmisLevelinfoCallback()
{
    _scene->resumeSceneCallback(GameScene::ResumeTypeEnum::dissmissLevelInfo);
    runAction(cocos2d::RemoveSelf::create(true));
}

LevelInfoPopup* LevelInfoPopup::create(GameScene* gameScene, int world, int level)
{
    auto popup = new LevelInfoPopup(gameScene, world, level);
    if (popup && popup->initWithColor(cocos2d::Color4B(0, 0, 0, 128)))
    {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

// MARK: RevivePopup

RevivePopup::RevivePopup(GameScene* gameScene, const std::string& text, int world, int level)
  : Popup(gameScene, world, level)
{
    auto reviveLabel = Utility::addLabel(Localization::getLocalizedString("revive_title"), CONSTANTS.fontNormal,
                                         CONSTANTS.getOffset() / 2, GAMECONFIG.getTextColor());
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    auto infoLabel =
        Utility::addLabel(text, CONSTANTS.fontSubtitle, CONSTANTS.getOffset() / 4, GAMECONFIG.getTextColor());
    infoLabel->setPosition(visibleSize.width / 2 - infoLabel->getContentSize().width / 2, visibleSize.height * 0.7F);
    infoLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 1);
    addChild(infoLabel);

    reviveLabel->setAlignment(cocos2d::TextHAlignment::CENTER);
    reviveLabel->setPosition(visibleSize.width / 2 - reviveLabel->getContentSize().width / 2,
                             infoLabel->getPosition().y + reviveLabel->getContentSize().height * 1.0);
    reviveLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 1);
    addChild(reviveLabel);

    addMenu();
    auto node1 = Utility::addLabelButton(Localization::getLocalizedString("revive_button"),
                                         cocos2d::Vec2(visibleSize.width * 0.5F, visibleSize.height * 0.55F),
                                         CC_CALLBACK_1(RevivePopup::reviveCallback, this));
    _menu->addChild(node1);
    auto node2 = Utility::addLabelButton(Localization::getLocalizedString("restart_button"),
                                         cocos2d::Vec2(visibleSize.width * 0.5F, visibleSize.height * 0.45f),
                                         CC_CALLBACK_1(PausePopup::restartGameCallback, this));
    _menu->addChild(node2);
    auto node3 = Utility::addLabelButton(Localization::getLocalizedString("main_menu_button"),
                                         cocos2d::Vec2(visibleSize.width * 0.5F, visibleSize.height * 0.35f),
                                         CC_CALLBACK_1(Popup::exitToLevelSelectViewCallback, this));
    _menu->addChild(node3);
}
RevivePopup::~RevivePopup() = default;

void RevivePopup::reviveCallback(cocos2d::Ref* sender)
{
    _scene->resumeSceneCallback(GameScene::ResumeTypeEnum::revive);
    //    runAction(Sequence::createWithTwoActions(EaseBackIn::create(ScaleTo::create(0.25f,
    //    0.0f)), RemoveSelf::create(true))); // TODO nice animation
    runAction(cocos2d::RemoveSelf::create(true));
}

RevivePopup* RevivePopup::create(GameScene* gameScene, std::string& text, int world, int level)
{
    auto popup = new RevivePopup(gameScene, text, world, level);
    if (popup && popup->initWithColor(cocos2d::Color4B(0, 0, 0, 128)))
    {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

// MARK: GameOverPopup

GameOverPopup::GameOverPopup(GameScene* gameScene, bool gameWon, int world, int level) : Popup(gameScene, world, level)
{
    addMenu();
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    if (!gameWon)
    {
        auto node1 = Utility::addLabelButton(Localization::getLocalizedString("restart_button"),
                                             cocos2d::Vec2(visibleSize.width * 0.5F, visibleSize.height * 0.45F),
                                             CC_CALLBACK_1(PausePopup::restartGameCallback, this));
        _menu->addChild(node1);

        showGameoverPopup(Localization::getLocalizedString("level_failed"), "", world, level, true);
    }
    else
    {
        if ((world == GAMECONFIG.getWorldCount() - 1 && level == GAMECONFIG.getLevelCountForWorld(world) - 1))
        {
            showGameoverPopup(Localization::getLocalizedString("victory"),
                              Localization::getLocalizedString("last_level"), world, level, false);
        }
        else if (playerHasWonTheGame(world, level))
        {
            showGameoverPopup(Localization::getLocalizedString("victory"), "", world, level, true);
            auto node1 = Utility::addLabelButton(Localization::getLocalizedString("next_button"),
                                                 cocos2d::Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.45F),
                                                 CC_CALLBACK_1(GameOverPopup::nextLevelCallback, this));
            _menu->addChild(node1);
        }
    }
    auto node2 = Utility::addLabelButton(Localization::getLocalizedString("main_menu_button"),
                                         cocos2d::Vec2(visibleSize.width * 0.5F, visibleSize.height * 0.35F),
                                         CC_CALLBACK_1(Popup::exitToLevelSelectViewCallback, this));
    _menu->addChild(node2);
}

bool GameOverPopup::playerHasWonTheGame(int world, int level)
{
    if (world + 1 > GAMECONFIG.getWorldCount())
    {  // might happen when you remove a world
        return false;
    }

    if (level < GAMECONFIG.getLevelCountForWorld(world) - 1)
    {
        GAMEDATA.unlockLevel(world, level + 1, 1);
        return true;
    }
    else if (level == GAMECONFIG.getLevelCountForWorld(world) - 1)
    {
        GAMEDATA.unlockWorld(world + 1, 1);
        return true;
    }

    return false;
}

void GameOverPopup::showGameoverPopup(const std::string& message, const std::string& cash, int world, int level,
                                      bool showStars)
{
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    // txt
    // game over
    cocos2d::Label* gameOverLabel =
        Utility::addLabel(message, CONSTANTS.fontNormal, CONSTANTS.getOffset() / 2, GAMECONFIG.getTextColor());
    gameOverLabel->setAlignment(cocos2d::TextHAlignment::CENTER);
    gameOverLabel->setPosition(
        cocos2d::Vec2(visibleSize.width / 2 - gameOverLabel->getContentSize().width / 2, visibleSize.height * 0.7F));
    gameOverLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 1);
    addChild(gameOverLabel);

    cocos2d::Label* cashLabel =
        Utility::addLabel(cash, CONSTANTS.fontNormal, CONSTANTS.getOffset() / 4, GAMECONFIG.getTextColor());
    cashLabel->setPosition(visibleSize.width / 2 - cashLabel->getContentSize().width / 2,
                           gameOverLabel->getPosition().y - cashLabel->getContentSize().height * 4);
    cashLabel->setDimensions(visibleSize.width / 2, cashLabel->getContentSize().height * 4);
    cashLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 1);
    addChild(cashLabel);

    // TODO stars also in pause menu!!
    if (showStars)
    {
        auto star_1 = cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.iconStar);
        star_1->setScale(0.7f);
        star_1->setLocalZOrder(CONSTANTS.LocalZOrderEnum::NO_Z_ORDER);
        star_1->setPosition(star_1->getPosition().x - star_1->getContentSize().width, star_1->getPosition().y);
        star_1->setColor(cocos2d::Color3B::YELLOW);

        auto star_2 = cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.iconStar);
        star_2->setScale(0.7F);
        star_2->setLocalZOrder(CONSTANTS.LocalZOrderEnum::NO_Z_ORDER);
        star_2->setPosition(star_1->getPosition().x + star_1->getContentSize().width, star_1->getPosition().y);
        star_2->setColor(cocos2d::Color3B::YELLOW);

        auto star_3 = cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.iconStar);
        star_3->setScale(0.7F);
        star_3->setLocalZOrder(CONSTANTS.LocalZOrderEnum::NO_Z_ORDER);
        star_3->setPosition(star_2->getPosition().x + star_1->getContentSize().width, star_1->getPosition().y);
        star_3->setColor(cocos2d::Color3B::YELLOW);

        auto starNode = Node::create();

        starNode->addChild(star_1);
        starNode->addChild(star_2);
        starNode->addChild(star_3);
        starNode->setPosition(visibleSize.width / 2 - starNode->getContentSize().width / 2, visibleSize.height * 0.6);
        starNode->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER);
        starNode->setVisible(false);
        addChild(starNode);

        // also check if unlocked before
        int money = 0;
        int enemies = 0;
        int lifePower = 0;
        GAMEDATA.getStarsForLevel(world, level, money, enemies, lifePower);
        if (money == 0)
        {
            star_1->setColor(cocos2d::Color3B::BLACK);
        }
        else
        {
            star_1->setColor(cocos2d::Color3B::YELLOW);
        }
        if (enemies == 0)
        {
            star_2->setColor(cocos2d::Color3B::BLACK);
        }
        else
        {
            star_2->setColor(cocos2d::Color3B::YELLOW);
        }
        if (lifePower == 0)
        {
            star_3->setColor(cocos2d::Color3B::BLACK);
        }
        else
        {
            star_3->setColor(cocos2d::Color3B::YELLOW);
        }

        starNode->setVisible(true);
    }
}

void GameOverPopup::nextLevelCallback(cocos2d::Ref* sender)
{
    if (_levelID < GAMECONFIG.getLevelCountForWorld(_worldID) - 1)
    {
        _levelID += 1;
    }
    else
    {
        _levelID = 0;
        _worldID++;
    }
    auto scene = GameScene::createScene(_worldID, _levelID);
    auto transition = AnimationHelper::sceneTransition(scene);
    cocos2d::Director::getInstance()->replaceScene(transition);
}

GameOverPopup::~GameOverPopup() = default;
GameOverPopup* GameOverPopup::create(GameScene* gameScene, bool won, int world, int level)
{
    auto popup = new GameOverPopup(gameScene, won, world, level);
    if (popup && popup->initWithColor(cocos2d::Color4B(0, 0, 0, 128)))
    {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}
