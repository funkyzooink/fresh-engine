/****************************************************************************
 Copyright (c) 2014-2019 Gabriel Heilig
 fresh-engine
 funkyzooink@gmail.com
 ****************************************************************************/
#include "MainScene.h"

#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"
#include "../GameObjects/Player.h"
#include "../Helpers/AnimationHelper.h"
#include "../Helpers/AudioPlayer.h"
#include "../Helpers/BackgroundNode.h"
#include "../Helpers/ColorPlugin.h"
#include "../Helpers/Utility.h"
#include "../Views/LevelSelectView.h"
#include "../Views/MainView.h"
#include "../Views/SettingsView.h"

MainScene::MainScene()
  : _settingsScenetype(MAINSCENE)
  , _exitButton(nullptr)
  , _pageId(LevelSelectView::_worldViewId)  // TODO
  , _mainView(nullptr)
  , _settingsView(nullptr)
  , _levelSelectView(nullptr)
  , _backgroundNode(nullptr)
{
}

MainScene::~MainScene() = default;
cocos2d::Scene* MainScene::createScene(MainSceneType type, int page)
{
    auto scene = cocos2d::Scene::create();
    auto layer = MainScene::create();
    scene->addChild(layer);
    layer->_pageId = page;
    layer->showPage(type);
    return scene;
}

cocos2d::Scene* MainScene::createScene(MainSceneType type)
{
    auto scene = cocos2d::Scene::create();
    auto layer = MainScene::create();
    scene->addChild(layer);
    layer->showPage(type);
    return scene;
}

bool MainScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    if (!(GAMECONFIG.getMainSceneConfig()->tilemap.empty() || GAMECONFIG.getMainSceneConfig()->tilemap.empty()))
    {  // TODO check if path is valid
        _backgroundNode = BackgroundNode::createWithBackgrounds();
        addChild(_backgroundNode);

        std::string tileMapPath =
            "img/" + CONSTANTS.getTileSizeIdentifier() + "/" + GAMECONFIG.getMainSceneConfig()->tilemap;

        auto node = createTiledBackground(tileMapPath);
        addChild(node);
    }

    // exit button
    _exitButton = Utility::addSpriteButton(CONSTANTS.iconArrow, cocos2d::Vec2::ZERO,
                                           CC_CALLBACK_1(MainScene::menuExitCallback, this));
    _exitButton->setRotation(-90);
    _exitButton->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER);
    _exitButton->setPosition(_exitButton->getContentSize().width + CONSTANTS.getOffset() * 0.1F,
                             _exitButton->getContentSize().height / 2);
    _exitButton->setOpacity(128);

    auto exitMenu = cocos2d::Menu::create(_exitButton, nullptr);
    exitMenu->setPosition(cocos2d::Vec2::ZERO);
    exitMenu->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 1);
    addChild(exitMenu);

    _mainView = MainView::createLayer(this);
    _mainView->setLocalZOrder(CONSTANTS.LocalZOrderEnum::MOVABLE_Z_ORDER);
    addChild(_mainView);

    _settingsView = SettingsView::createLayer();
    _settingsView->setVisible(false);
    addChild(_settingsView);

    _levelSelectView = LevelSelectView::createLayer();
    _levelSelectView->setVisible(false);
    addChild(_levelSelectView);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) ||                            \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    // keyboard events
    // Currently Desktop only
    auto keyboardListener = cocos2d::EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(MainScene::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
#endif

    scheduleUpdate();

    return true;
}

cocos2d::Node* MainScene::createTiledBackground(const std::string& tiledMapPath)
{
    auto node = cocos2d::Node::create();
    auto tiledMap = cocos2d::TMXTiledMap::create(tiledMapPath);
    tiledMap->setScale(cocos2d::Director::getInstance()->getContentScaleFactor());
    tiledMap->setLocalZOrder(CONSTANTS.LocalZOrderEnum::NO_Z_ORDER);
    node->addChild(tiledMap);
    node->setLocalZOrder(CONSTANTS.LocalZOrderEnum::MOVABLE_Z_ORDER);

    ColorPlugin::initLayerBackground(tiledMap, 0, this);
    ColorPlugin::initLayerColors(tiledMap, 0, this, false);

    // read objects
    cocos2d::TMXObjectGroup* grp = tiledMap->getObjectGroup(CONSTANTS.tilemapGameObjectsGroup);

    auto objects = grp->getObjects();
    for (auto& object : objects)
    {
        auto properties = object.asValueMap();
        cocos2d::Point position = cocos2d::Point(0, 0);
        position.x = properties.at("x").asInt() * cocos2d::Director::getInstance()->getContentScaleFactor();
        position.y = properties.at("y").asInt() * cocos2d::Director::getInstance()->getContentScaleFactor();

        std::string name = properties.at("name").asString();
        if (GAMECONFIG.isPlayerType(name))
        {
            auto player = GAMECONFIG.getPlayerObject(GAMEDATA.getPlayerId())->clone(nullptr);  // TODO nullptr check
            player->setPosition(position + player->getContentSize() / 2);
            player->setLocalZOrder(CONSTANTS.LocalZOrderEnum::PLAYER_Z_ORDER);
            player->startIdleAnimation();
            node->addChild(player);
        }
    }

    return node;
}

void MainScene::update(float dt)
{
    _backgroundNode->update(dt);
}

void MainScene::menuExitCallback(Ref* pSender)
{
    switch (_settingsScenetype)
    {
        case MAINSCENE:
            break;
        case SETTINGS:
        {
            _settingsScenetype = MAINSCENE;
            _exitButton->setVisible(false);
            AnimationHelper::fadeOut(_settingsView, _mainView);
            break;
        }
        case LEVELSELECT:
        {
            _pageId = _levelSelectView->getPage();
            if (_pageId == LevelSelectView::_worldViewId)
            {
                _settingsScenetype = MAINSCENE;
                _exitButton->setVisible(false);
                AnimationHelper::fadeOut(_levelSelectView, _mainView);
            }
            else
            {
                _levelSelectView->showPage(LevelSelectView::_worldViewId);
            }
            break;
        }
    }
}

void MainScene::showPage(MainSceneType type)
{
    _settingsScenetype = type;
    switch (type)
    {
        case MAINSCENE:
        {
            AudioPlayer::playMenuMusic();
            AnimationHelper::fadeIn(nullptr, _mainView);
            _exitButton->setVisible(false);
            break;
        }
        case SETTINGS:
        {
            _levelSelectView->setVisible(false);
            AnimationHelper::fadeIn(_mainView, _settingsView);
            _settingsView->showPage();
            _exitButton->setVisible(true);
            break;
        }
        case LEVELSELECT:
        {
            _settingsView->setVisible(false);
            _exitButton->setVisible(true);
            _levelSelectView->showPage(_pageId);
            auto fadeInView = _pageId == LevelSelectView::_worldViewId ? _mainView : nullptr;
            _mainView->setVisible(false);
            AnimationHelper::fadeIn(fadeInView, _levelSelectView);
            break;
        }
    }
}

void MainScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    switch (keyCode)
    {
        case cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE:
            // TODO ask if wants to quit
            cocos2d::Director::getInstance()->end();
    }
}
