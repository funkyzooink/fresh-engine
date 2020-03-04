/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "GameScene.h"

#include "../3psw/Shake.h"
#include "../GameData/Constants.h"
#include "../GameData/Gamedata.h"
#include "../GameObjects/Block.h"
#include "../GameObjects/Bullet.h"
#include "../GameObjects/Enemy.h"
#include "../GameObjects/Player.h"
#include "../GameObjects/Upgrade.h"
#include "../Helpers/AnimationHelper.h"
#include "../Helpers/AudioPlayer.h"
#include "../Helpers/BackgroundNode.h"
#include "../Helpers/ColorPlugin.h"
#include "../Helpers/Localization.h"
#include "../Helpers/TileHelper.h"
#include "../Helpers/Utility.h"
#include "../Views/HUD.h"
#include "../Views/Popups.h"

#include "cocos2d.h"

// MARK: - create

GameScene::GameScene()
  : _hudLayer(nullptr)
  , _backgroundNode(nullptr)
  , _player(nullptr)
  , _activePopup(false)
  , _playerCrashRectangle(nullptr)
  , _tutorialInfoLabel(nullptr)
  , _cameraFollowNode(nullptr)
  , _crashCloudSprite(nullptr)
  , _enemyHitSprite(nullptr)
  , _tiledMap(nullptr)
  , _gameTime(0)
  , _worldID(0)
  , _enemyCounter(0)
  , _enemyMaxCounter(0)
  , _moneyCounter(0)
  , _moneyMaxCounter(0)
  , _tutorialInfoLabelIndex(0)
  , _allowedPlayerTypes()
  , _levelID(0)
{
}

GameScene::~GameScene() = default;

cocos2d::Scene* GameScene::createScene(int world, int level)
{
    auto scene = cocos2d::Scene::create();
    auto layer = GameScene::create();

    layer->_worldID = world;
    layer->_levelID = level;
    layer->setTag(998);  // needed to set pause when app goes to background
    layer->loadMap();
    scene->addChild(layer);
    return scene;
}
void GameScene::loadMap()
{
    // TODO is this cleanup needed?
    // for (int i = 0; i < 6; ++i)
    // {
    //     if (i != _worldID)
    //     {
    //         std::string background = "bg" + Utility::numberToString(i + 1) + ".plist";
    //         cocos2d::SpriteFrameCache::getInstance()->removeSpriteFrameByName(background);
    //     }
    // }
    auto spriteSheets = GAMECONFIG.getSpriteSheets(_worldID);
    for (auto& spriteSheet : spriteSheets)
    {
        if (!cocos2d::SpriteFrameCache::getInstance()->isSpriteFramesWithFileLoaded(spriteSheet))
        {
            cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(spriteSheet);
        }
    }

    _hudLayer = HUD::createLayer();
    _hudLayer->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER);
    addChild(_hudLayer);

    initParallaxBackground();

    initTiledMap();

    ColorPlugin::initLayerBackground(_tiledMap, _worldID, _cameraFollowNode);
    ColorPlugin::initLayerColors(_tiledMap, _worldID, _cameraFollowNode, true);

    initTmxObjects();

    _cameraFollowNode->addChild(_tiledMap);

    _cameraFollowNode->runAction(cocos2d::Follow::create(_player, _tiledMap->getBoundingBox()));

    // update
    _activePopup = true;
    LevelInfoPopup* popup = LevelInfoPopup::create(this, _worldID, _levelID);
    addChild(popup, CONSTANTS.CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 2);
}

void GameScene::initTiledMap()
{
    // Tile Map
    auto level = GAMECONFIG.getWorldConfig(_worldID).levels.at(_levelID);
    std::string tilename = "img/" + CONSTANTS.getTileSizeIdentifier() + "/" + level.tilemap;
    _tiledMap = cocos2d::TMXTiledMap::create(tilename);
    _tiledMap->setScale(cocos2d::Director::getInstance()->getContentScaleFactor());
    _tiledMap->setLocalZOrder(CONSTANTS.LocalZOrderEnum::NO_Z_ORDER);
}

void GameScene::initTmxObjects()
{
    // read objects
    cocos2d::TMXObjectGroup* grp = _tiledMap->getObjectGroup(CONSTANTS.tilemapGameObjectsGroup);

    auto objects = grp->getObjects();
    for (auto& object : objects)
    {
        auto properties = object.asValueMap();
        auto position =
            cocos2d::Point(properties.at("x").asInt() * cocos2d::Director::getInstance()->getContentScaleFactor(),
                           properties.at("y").asInt() * cocos2d::Director::getInstance()->getContentScaleFactor());

        std::string name = properties.at("name").asString();

        if (GAMECONFIG.isPlayerEntry(name))  // TODO remove
        {
            // TODO use upgradePlayerForId
            if (_player == nullptr)
            {
                initPlayer(GAMECONFIG.getPlayerObject(GAMEDATA.getPlayerId()), position);
            }
        }
        else if (GAMECONFIG.isPlayerType(name))
        {
            // TODO use upgradePlayerForId
            if (_player == nullptr)
            {
                initPlayer(GAMECONFIG.getPlayerObjectForKey(name), position);
            }
            _allowedPlayerTypes.push_back(name);
        }
        else if (GAMECONFIG.isEnemyType(name))
        {
            Enemy* movableObject = GAMECONFIG.getEnemyObject(name)->clone(this);
            _enemyMaxCounter = _enemyMaxCounter + 1;
            addGameObject(movableObject, position);
        }
        else if (GAMECONFIG.isBlockType(name))
        {
            Block* movableObject = GAMECONFIG.getBlockObject(name)->clone(this);
            addGameObject(movableObject, position);
        }
        else if (GAMECONFIG.isUpgradeType(name))
        {
            Upgrade* movableObject = GAMECONFIG.getUpgradeObject(name)->clone(this);
            _moneyMaxCounter += movableObject->getActions().money;
            addGameObject(movableObject, position);
        }
        else
        {
            //#if COCOS2D_DEBUG > 0
            //      auto message = "Object in tilemap but not in json: " + name;
            //      CCASSERT(false, message.c_str());
            //#endif
            continue;
        }
    }

    // hud
    auto enemies = Utility::numberToString(_enemyCounter) + "/" + Utility::numberToString(_enemyMaxCounter);
    auto money = Utility::numberToString(_moneyCounter) + "/" + Utility::numberToString(_moneyMaxCounter);
    _hudLayer->setEnemies(enemies);
    _hudLayer->setCoins(money);
}

void GameScene::initParallaxBackground()
{
    _backgroundNode = BackgroundNode::createWithWorldAndLevelId(_worldID, _levelID);
    _backgroundNode->setLocalZOrder(-5);
    addChild(_backgroundNode);
}

bool GameScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    _cameraFollowNode = cocos2d::Node::create();
    addChild(_cameraFollowNode);

    /// touch / mouse events
    auto listener = cocos2d::EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(GameScene::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(GameScene::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(GameScene::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) ||                            \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    // keyboard events
    // Currently Desktop only
    auto keyboardListener = cocos2d::EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
#endif
    // accelerometer events
    //    auto accelerometer1 = GAMECONFIG.getControlConfig(CONSTANTS.accelerometer1).type;
    //    auto accelerometer2 = GAMECONFIG.getControlConfig(CONSTANTS.accelerometer2).type;
    //    if (!accelerometer1.empty() || !accelerometer2.empty())
    //    {
    //        setAccelerometerEnabled(true);
    //        auto accListener = cocos2d::EventListenerAcceleration::create(CC_CALLBACK_2(GameScene::onAcceleration,
    //        this)); _eventDispatcher->addEventListenerWithSceneGraphPriority(accListener, this);
    //    }

    //	/////////////////////////////
    //
    auto playpausItem =
        cocos2d::MenuItemSprite::create(cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconPauseNormal),
                                        cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.buttonIconPauseNormal),
                                        nullptr, CC_CALLBACK_1(GameScene::menuPauseCallback, this));

    playpausItem->setPosition(cocos2d::Vec2(visibleSize.width - playpausItem->getContentSize().width * 0.5,
                                            visibleSize.height - playpausItem->getContentSize().height / 2));

    auto playPauseMenu_ = cocos2d::Menu::create(playpausItem, nullptr);
    playPauseMenu_->setPosition(cocos2d::Vec2::ZERO);
    playPauseMenu_->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 1);
    addChild(playPauseMenu_);

    // crash rect
    _playerCrashRectangle = cocos2d::DrawNode::create();
    _playerCrashRectangle->drawSolidRect(cocos2d::Vec2(0, 0), cocos2d::Vec2(visibleSize.width, visibleSize.height),
                                         Utility::colorConverterFloat(GAMECONFIG.getPlayerCrashRectangleColor()));
    _playerCrashRectangle->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    _playerCrashRectangle->setLocalZOrder(3);
    _playerCrashRectangle->setVisible(false);
    addChild(_playerCrashRectangle);

    _tutorialInfoLabel =
        Utility::addLabel("0", CONSTANTS.fontSubtitle, CONSTANTS.getOffset() / 4, cocos2d::Color4B::BLACK);
    _tutorialInfoLabel->setPosition(visibleSize.width / 3 - _tutorialInfoLabel->getContentSize().width / 2,
                                    visibleSize.height * 0.7 - _tutorialInfoLabel->getContentSize().width / 2);
    _tutorialInfoLabel->setLocalZOrder(CONSTANTS.LocalZOrderEnum::MOVABLE_Z_ORDER);
    _tutorialInfoLabel->setVisible(false);
    _cameraFollowNode->addChild(_tutorialInfoLabel);

    _tutorialInfoLabelIndex = 0;

    // effects
    _crashCloudSprite = cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.iconCloud);
    _crashCloudSprite->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER);
    _crashCloudSprite->setVisible(false);

    _enemyHitSprite = cocos2d::Sprite::createWithSpriteFrameName(CONSTANTS.iconHit);
    _enemyHitSprite->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER);
    _enemyHitSprite->setVisible(false);

    _cameraFollowNode->addChild(_crashCloudSprite);
    _cameraFollowNode->addChild(_enemyHitSprite);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) ||                            \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    glview->setCursorVisible(false);
#endif

    return true;
}

// MARK: - Touch events

void GameScene::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
    if (_activePopup)
        return;
    for (auto& touch : touches)
    {
        if (touch)
        {
            handleTouch(touch, false);
        }
    }
}

void GameScene::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
    if (_activePopup)
        return;
    for (auto& touch : touches)
    {
        if (touch)
        {
            handleTouch(touch, true);
        }
    }
}

void GameScene::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
    if (_activePopup)
        return;

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    for (auto& touch : touches)
    {
        if (touch)
        {
            float touchArea = touch->getLocationInView().x / visibleSize.width;
            float right = CONSTANTS.touchArea2 + ((CONSTANTS.touchArea3 - CONSTANTS.touchArea2) / 2);

            if (touchArea <= right)
            {
                _player->setMovementState(GameObject::MovementStateEnum::IDLE_MOVEMENT);
            }
        }
    }
}

// MARK: - Key events

void GameScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    if (_activePopup)
        return;

    switch (keyCode)
    {
        case cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE:
        {
            menuPauseCallback(this);
            break;
        }
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_A:
        {
            actionLeft();
            break;
        }
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_D:
        {
            actionRight();
            break;
        }
        case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_S:
        {
            actionCustom(false, _player->getCustomButton1());
            break;
        }
        case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_W:
        {
            actionCustom(false, _player->getCustomButton2());
            break;
        }
        default:
            break;
    }
}

void GameScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    switch (keyCode)
    {
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_A:
            if (_player->getHeadingState() == GameObject::HeadingStateEnum::LEFT_HEADING)
            {
                _player->setMovementState(GameObject::MovementStateEnum::IDLE_MOVEMENT);
            }
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_D:
            if (_player->getHeadingState() == GameObject::HeadingStateEnum::RIGHT_HEADING)
            {
                _player->setMovementState(GameObject::MovementStateEnum::IDLE_MOVEMENT);
            }
            break;
        default:
            break;
    }
}

// MARK: - Acceleration events

void GameScene::onAcceleration(cocos2d::Acceleration* acceleration, cocos2d::Event* event)
{
    if (acceleration->x > 0.05)
    {
        actionRight();
    }
    else if (acceleration->x < -0.05)
    {
        actionLeft();
    }
    else
    {
        _player->setMovementState(GameObject::MovementStateEnum::IDLE_MOVEMENT);
    }
}

// MARK: - Event Helpers

void GameScene::actionLeft()
{
    _player->setHeadingState(GameObject::HeadingStateEnum::LEFT_HEADING);
    _player->setMovementState(GameObject::MovementStateEnum::WALK_MOVEMENT);
}

void GameScene::actionRight()
{
    _player->setHeadingState(GameObject::HeadingStateEnum::RIGHT_HEADING);
    _player->setMovementState(GameObject::MovementStateEnum::WALK_MOVEMENT);
}

void GameScene::actionJump(bool move)
{
    if (move)  // only run this action on touch down
        return;

    if (_player->getJumpState() == GameObject::JumpStateEnum::NO_JUMP)  // normal jump
    {
        _player->setJumpState(GameObject::JumpStateEnum::WANTS_TO_JUMP);
        AudioPlayer::playFx(CONSTANTS.audioJump);
    }
    else if (_player->getJumpState() == GameObject::JumpStateEnum::JUMP)  // dbl jump
    {
        _player->setJumpState(GameObject::JumpStateEnum::WANTS_TO_DOUBLE_JUMP);
        AudioPlayer::playFx(CONSTANTS.audioJump);

        _crashCloudSprite->setPosition(_player->getPosition());
        _crashCloudSprite->setVisible(true);
        _crashCloudSprite->runAction(
            cocos2d::Sequence::create(cocos2d::FadeIn::create(0.15F), cocos2d::FadeOut::create(0.15F), nullptr));
    }
}

void GameScene::actionCustom(bool move, const std::string& additionalButton)
{
    if (move)  // only run this action on touch down
        return;

    if (additionalButton == CONSTANTS.buttonTypeAttack)
    {
        _player->attack();
    }
    else if (additionalButton == CONSTANTS.buttonTypeShoot)
    {
        _player->prepareToShoot();
    }
    else if (additionalButton == CONSTANTS.buttonTypeDown)
    {
        _player->setJumpState(GameObject::JumpStateEnum::FALLING_DOWN_JUMP);
    }
    else if (additionalButton == CONSTANTS.buttonTypeJump)
    {
        actionJump(move);
    }
    else if (additionalButton == CONSTANTS.buttonTypeSwitch && _allowedPlayerTypes.size() > 0)
    {  // TODO error handling
        auto result = std::find_if(_allowedPlayerTypes.begin(), _allowedPlayerTypes.end(),
                                   [&](const auto& v) { return v == _player->getName(); });
        auto next = std::next(result, 1);
        if (next != _allowedPlayerTypes.end())
        {
            upgradePlayerForKey(*next);
        }
        else
        {
            upgradePlayerForKey(*_allowedPlayerTypes.begin());
        }
    }
}

void GameScene::handleTouchArea(const std::string& touchType, bool move)
{
    if (touchType == "left")
    {
        actionLeft();
    }
    else if (touchType == "right")
    {
        actionRight();
    }
    else if (touchType == "custom1")
    {
        actionCustom(move, _player->getCustomButton1());
    }
    else if (touchType == "custom2")
    {
        actionCustom(move, _player->getCustomButton2());
    }
    else if (touchType == "jump")
    {
        actionJump(move);
    }
}

void GameScene::handleTouch(cocos2d::Touch* touch, bool move)
{
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    float touchArea = touch->getLocationInView().x / visibleSize.width;
    float area1 = CONSTANTS.touchArea1 + ((CONSTANTS.touchArea2 - CONSTANTS.touchArea1) / 2);
    float area2 = CONSTANTS.touchArea2 + ((CONSTANTS.touchArea3 - CONSTANTS.touchArea2) / 2);
    float area3 = CONSTANTS.touchArea3 + ((CONSTANTS.touchArea4 - CONSTANTS.touchArea3) / 2);
    if (touchArea <= area1)
    {
        auto touchType = GAMECONFIG.getControlConfig(CONSTANTS.touch1);
        handleTouchArea(touchType.type, move);
    }
    else if (touchArea <= area2)
    {
        auto touchType = GAMECONFIG.getControlConfig(CONSTANTS.touch2);
        handleTouchArea(touchType.type, move);
    }
    else if (touchArea <= area3)
    {
        auto touchType = GAMECONFIG.getControlConfig(CONSTANTS.touch3);
        handleTouchArea(touchType.type, move);
    }
    else
    {
        auto touchType = GAMECONFIG.getControlConfig(CONSTANTS.touch4);
        handleTouchArea(touchType.type, move);
    }
}

// MARK: - callbacks

void GameScene::dissmisLevelinfoCallback()
{
    _activePopup = false;

    AudioPlayer::playMusicForWorld(_worldID);

    scheduleUpdate();
}

void GameScene::resumeSceneCallback(ResumeTypeEnum type)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) ||                            \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    glview->setCursorVisible(false);
#endif

    switch (type)
    {
        case ResumeTypeEnum::revive:
            resumeReviveGame();
            break;
        case ResumeTypeEnum::dissmissLevelInfo:
            dissmisLevelinfoCallback();
            break;
        case ResumeTypeEnum::gameover:
            gameOverCallback(false);
            break;
        case ResumeTypeEnum::pause:
            _activePopup = false;
            break;
    }
}
void GameScene::menuPauseCallback(Ref* pSender)
{
    if (_activePopup)
        return;

    _activePopup = true;

    PausePopup* popup = PausePopup::create(this, _worldID, _levelID);
    addChild(popup, CONSTANTS.CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 2);
}

void GameScene::gameOverCallback(bool gameWon)
{
    stopAllActions();
    AudioPlayer::stopMusic();
    if (gameWon)
    {
        AudioPlayer::playFx(CONSTANTS.audioEnd);
    }

    GAMEDATA.setPlayerId(_player->getId());

    if (gameWon)
    {
        GAMEDATA.setPlayerTotalCash(GAMEDATA.getPlayerTotalCash() + _moneyCounter);

        GAMEDATA.setStarMap(_worldID, _levelID, _moneyCounter == _moneyMaxCounter, _enemyCounter == _enemyMaxCounter,
                            _player->getLife() == GAMECONFIG.getGameplayConfig().playerMaxLife);
    }

    _activePopup = true;
    GameOverPopup* popup = GameOverPopup::create(this, gameWon, _worldID, _levelID);
    addChild(popup, CONSTANTS.CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 2);
}

void GameScene::playerHitByEnemyCallback()  // TODO move to player class
{
    if (_player->getHit() < 1)
    {
        _player->setHit(55);

        if (!GAMECONFIG.getPlayerObjectForKey(_player->getName())->_upgrade.empty())  // player has an upgrade -> loose
                                                                                      // upgrad
        {
            upgradePlayerForId(0);  // TODO 0!
        }
        else
        {
            if (!_player->decreaseLife())  // if false player just died
            {
                playerLost();
            }
        }

        _hudLayer->setLife(_player->getLife());

        // player touches enemy
        _playerCrashRectangle->setVisible(true);
        _playerCrashRectangle->runAction(cocos2d::Sequence::create(
            cocos2d::FadeIn::create(0.01f), cocos2d::FadeOut::create(0.01f), cocos2d::FadeIn::create(0.01F),
            cocos2d::FadeOut::create(0.01F), cocos2d::Hide::create(), nullptr));
        _player->runAction(AnimationHelper::blinkAnimation());
    }
}

// MARK: - Update

void GameScene::update(float dt)
{
    if (_activePopup)
        return;

    // update info layer
    updateTutorial();

    // increase time
    _gameTime += dt;
    _hudLayer->setTime(_gameTime);

    // move background
    _backgroundNode->update(dt);

    if (_player->getPosition().y <= 0)  // if player falls out of game then stop game
    {
        // game over
        playerLost();
        return;
    }

    for (auto gameObject : getGameObjectVector())
    {
        if (dynamic_cast<GameObject*>(gameObject) == nullptr)
            continue;
        gameObject->update(dt);
    }
}

void GameScene::updateTutorial()
{
    // read objects
    cocos2d::TMXObjectGroup* grp = _tiledMap->getObjectGroup(CONSTANTS.tilemapInfoGroup);
    if (grp == nullptr || _tutorialInfoLabel->isVisible())
    {
        return;
    }

    auto objects = grp->getObjects();
    for (auto& object : objects)
    {
        auto properties = object.asValueMap();
        auto position =
            cocos2d::Point(properties.at("x").asInt() * cocos2d::Director::getInstance()->getContentScaleFactor(),
                           properties.at("y").asInt() * cocos2d::Director::getInstance()->getContentScaleFactor());
        std::string infoKey = properties.at("name").asString();

        if (_player->getPosition().x - 100 < position.x && _player->getPosition().x + 100 > position.x)
        {
            _tutorialInfoLabel->stopAllActions();
            _tutorialInfoLabel->setVisible(true);
            _tutorialInfoLabel->setPosition(position);
            _tutorialInfoLabel->runAction(AnimationHelper::blinkAnimation());
            _tutorialInfoLabel->setString(Localization::getLocalizedString(infoKey));
            _tutorialInfoLabel->runAction(cocos2d::Sequence::create(
                cocos2d::DelayTime::create(3.0F), cocos2d::FadeIn::create(0.05F), cocos2d::FadeOut::create(0.05F),
                cocos2d::FadeIn::create(0.05F), cocos2d::FadeOut::create(0.05f), cocos2d::Hide::create(), nullptr));
        }
    }
}
void GameScene::upgradePlayerForKey(const std::string& playerKey)
{
    cocos2d::Point pPosition = _player->getPosition() - _player->getContentSize() / 2;  // TODO remove this workaround
    _cameraFollowNode->removeChild(_player);

    _player->setDisabled(true);

    auto player = GAMECONFIG.getPlayerObjectForKey(playerKey);
    initPlayer(player, pPosition);
}

void GameScene::upgradePlayerForId(int playerId)
{
    cocos2d::Point pPosition = _player->getPosition() - _player->getContentSize() / 2;  // TODO remove this workaround
    _cameraFollowNode->removeChild(_player);

    _player->setDisabled(true);

    auto player = GAMECONFIG.getPlayerObject(playerId);
    initPlayer(player, pPosition);
}

void GameScene::initPlayer(Player* player, const cocos2d::Point& pPosition)
{
    // TODO is there a better way

    // TODO 4friends check if works with other games
    auto life = GAMECONFIG.getGameplayConfig().playerMaxLife;
    if (_player != nullptr)
    {
        life = _player->getLife();
    }
    _player = player->clone(this);
    _player->setLife(life);
    _player->setPosition(pPosition + _player->getContentSize() / 2);
    _player->setLocalZOrder(CONSTANTS.LocalZOrderEnum::PLAYER_Z_ORDER);
    for (auto const& ammo : _player->getAmmoVector())
    {
        _cameraFollowNode->addChild(ammo);
    }
    _cameraFollowNode->addChild(_player);
    _cameraFollowNode->stopAllActions();
    _cameraFollowNode->runAction(Shake::create(0.05F, 4.0F, 4.0F));
    _cameraFollowNode->runAction(cocos2d::Follow::create(_player, _tiledMap->getBoundingBox()));
    _player->runAction(AnimationHelper::blinkAnimation());

    _hudLayer->setCustomButton1(_player->getCustomButton1());
    _hudLayer->setCustomButton2(_player->getCustomButton2());
}
// TODO new collision
void GameScene::showCrashCloud()
{
    _cameraFollowNode->runAction(Shake::create(0.1f, 3.0f, 3.0F));
    _crashCloudSprite->setPosition(_player->getPosition().x,
                                   _player->getPosition().y - _tiledMap->getTileSize().height / 8);
    _crashCloudSprite->setVisible(true);
    _crashCloudSprite->runAction(
        cocos2d::Sequence::create(cocos2d::FadeIn::create(0.15F), cocos2d::FadeOut::create(0.15F), nullptr));

    AudioPlayer::playFx(CONSTANTS.audioShake);
}

// MARK: - Helpers
void GameScene::addGameObject(GameObject* movableObject, const cocos2d::Point& position)
{
    movableObject->setLocalZOrder(CONSTANTS.LocalZOrderEnum::MOVABLE_Z_ORDER);

    for (auto const& ammo : movableObject->getAmmoVector())
    {
        _cameraFollowNode->addChild(ammo);
    }
    movableObject->setPosition(position + movableObject->getContentSize() / 2);
    _cameraFollowNode->addChild(movableObject);
}

void GameScene::playerLost()
{
    _player->stopAllActions();

    AudioPlayer::stopMusic();  // Todo pause here and continue on revive
    AudioPlayer::playFx(CONSTANTS.audioGameOver);

    if (_activePopup)
        return;

    _activePopup = true;

    // ask revive
    if (GAMEDATA.getPlayerTotalCash() >= GAMEDATA.getPlayerReviveCash())
    {
        stopAllActions();
        auto localized1 =
            Localization::getLocalizedString("revive_text_1", Utility::numberToString(GAMEDATA.getPlayerReviveCash()));
        auto localized2 =
            Localization::getLocalizedString("revive_text_2", Utility::numberToString(GAMEDATA.getPlayerTotalCash()));
        auto message = localized1 + localized2;
        _activePopup = true;
        RevivePopup* popup = RevivePopup::create(this, message, _worldID, _levelID);
        addChild(popup, CONSTANTS.CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER + 2);
    }
    else  // not enough revive coin left
    {
        gameOverCallback(false);
    }
}

void GameScene::resumeReviveGame()
{
    AudioPlayer::playMusicForWorld(_worldID);  // TODO pause music and resume in case of revive

    _activePopup = false;

    _player->restore();
    _player->runAction(AnimationHelper::blinkAnimation());
    _player->setLife(GAMECONFIG.getGameplayConfig().playerMaxLife);
    _hudLayer->setLife(_player->getLife());

    GAMEDATA.setRevive();
}

void GameScene::destroyGameObject(GameObject* gameObject)
{
    gameObject->setDisabled(true);
    _enemyHitSprite->setPosition(gameObject->getPosition());
    _enemyHitSprite->setVisible(true);
    _enemyHitSprite->runAction(
        cocos2d::Sequence::create(cocos2d::FadeIn::create(0.1F), cocos2d::FadeOut::create(0.1F), nullptr));

    if (gameObject->decreaseLife())  // if the object has enough life then reduce it and do not die
    {
        gameObject->setDisabled(true);
        runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(2.0F),
            cocos2d::CallFuncN::create(CC_CALLBACK_0(GameScene::disableGameobject, this, false, gameObject)), nullptr));
        return;
    }

    gameObject->showDestroyAnimationAndRemove();

    _enemyCounter++;
    std::string enemies = Utility::numberToString(_enemyCounter) + "/" + Utility::numberToString(_enemyMaxCounter);
    _hudLayer->setEnemies(enemies);

    AudioPlayer::playFx(CONSTANTS.audioHit);
}

void GameScene::explodeGameObject(GameObject* gameObject)
{
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    // check if bullet is on screen, otherwise no reason to show explosion
    if ((_player->getPosition().x > gameObject->getPosition().x - visibleSize.width &&
         _player->getPosition().x < gameObject->getPosition().x + visibleSize.width) &&
        (_player->getPosition().y > gameObject->getPosition().y - visibleSize.height &&
         _player->getPosition().y < gameObject->getPosition().y + visibleSize.height))
    {
        cocos2d::Sprite* cloud = Utility::createCrashCloud();
        cloud->setPosition(gameObject->getPosition());
        cloud->setVisible(true);
        _cameraFollowNode->addChild(cloud);
        cloud->runAction(cocos2d::Sequence::create(cocos2d::FadeIn::create(0.1F), cocos2d::FadeOut::create(0.4F),
                                                   cocos2d::Hide::create(), nullptr));

        AudioPlayer::playFx(CONSTANTS.audioExplode);
    }
}

void GameScene::disableGameobject(bool dis, GameObject* gameObject)
{
    gameObject->setDisabled(dis);
}

const cocos2d::Vec2& GameScene::getPlayerPosition()
{
    return _player->getPosition();
}

void GameScene::applicationDidEnterBackground()
{
    if (_activePopup)  // TODO does not work?
    {
        dissmisLevelinfoCallback();
    }
    else
    {
        menuPauseCallback(this);
    }
}

// MARK: - getters and setters

void GameScene::increaseMoney()
{
    _moneyCounter += 1;
    std::string money = Utility::numberToString(_moneyCounter) + "/" + Utility::numberToString(_moneyMaxCounter);
    _hudLayer->setCoins(money);
}

void GameScene::increaseLife()
{
    _player->increaseLife();
    _hudLayer->setLife(_player->getLife());
}

cocos2d::Vector<cocos2d::Node*> GameScene::getGameObjectVector()
{
    return _cameraFollowNode->getChildren();  // This is a little ugly because we need casts to gameobject
}

Player* GameScene::getPlayer()
{
    return _player;
}

cocos2d::TMXTiledMap* GameScene::getTileMap()
{
    return _tiledMap;
}
