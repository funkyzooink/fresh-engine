/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "BootScene.h"

#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"
#include "../Helpers/AnimationHelper.h"
#include "../Helpers/AudioPlayer.h"
#include "../Helpers/Utility.h"
#include "MainScene.h"

#include "cocos2d.h"

BootScene::BootScene() : _backgroundSprite(nullptr)
{
}
cocos2d::Scene* BootScene::createScene()
{
    auto scene = cocos2d::Scene::create();
    auto layer = BootScene::create();
    scene->addChild(layer);
    return scene;
}

bool BootScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    auto listener = cocos2d::EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(BootScene::onTouchesBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    _backgroundSprite = cocos2d::Sprite::create("LaunchScreenBackground.png");
    _backgroundSprite->setScale((visibleSize.height / _backgroundSprite->getContentSize().height) * 1);
    _backgroundSprite->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2));
    addChild(_backgroundSprite);

    // Load background // TODO get from json
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sprite.plist");
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile("characters.plist");
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile("world1.plist");
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile("bg1.plist");

    //
    // read data - if any value is not written to device write it
    GAMECONFIG.parseGameObjects();
    GAMEDATA.loadData();

    runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(3.0F),
                                        cocos2d::CallFuncN::create(CC_CALLBACK_1(BootScene::menuPlayCallback, this)),
                                        nullptr));

    return true;
}

void BootScene::menuPlayCallback(Ref* pSender)
{
    auto scene = MainScene::createScene(MainScene::MainSceneType::MAINSCENE);
    auto transition = AnimationHelper::sceneTransition(scene);
    cocos2d::Director::getInstance()->replaceScene(transition);
}

void BootScene::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
    menuPlayCallback(this);
}
