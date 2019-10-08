/****************************************************************************
 Copyright (c) 2014-2019 Gabriel Heilig
 fresh-engine
 funkyzooink@gmail.com
 ****************************************************************************/
#include "BackgroundNode.h"

#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../3psw/CCParallaxScrollNode.h"

BackgroundNode::BackgroundNode() : _parallaxNode(nullptr)
{
}

BackgroundNode* BackgroundNode::createWithWorldAndLevelId(int world, int level)
{
    auto node = BackgroundNode::create();
    node->addParallaxBackground(GAMECONFIG.getBackgroundSprites(world),
                                GAMECONFIG.getWorldConfig(world).backgrounds.parallaxRatio);
    node->addParticles(world, level);
    return node;
}

BackgroundNode* BackgroundNode::createWithBackgrounds()
{
    auto node = BackgroundNode::create();
    node->addParallaxBackground(GAMECONFIG.getMainSceneConfig()->background, { 0.05, 0.1, 0.1, 0.05 });  // TODO
    return node;
}

void BackgroundNode::addParallaxBackground(std::vector<std::string> backgrounds, std::vector<float> parallaxRatio)
{
#if COCOS2D_DEBUG > 0
    auto message = "parallaxRatio and background size need to be the same";
    CCASSERT(backgrounds.size() <= parallaxRatio.size(), message);
#endif
    _parallaxNode = CCParallaxScrollNode::create();
    addChild(_parallaxNode);
    for (int i = 0; i < backgrounds.size(); ++i)
    {
        auto sprite1 = cocos2d::Sprite::createWithSpriteFrameName(backgrounds.at(i));
        auto sprite2 = cocos2d::Sprite::createWithSpriteFrameName(backgrounds.at(i));
        _parallaxNode->addInfiniteScrollWithObjects({ sprite1, sprite2 }, i - (int)backgrounds.size() - 2,
                                                    Point(parallaxRatio.at(i), 0), Point(0, 0), Point(1, 0));
    }
}
void BackgroundNode::addParticles(int world, int level)
{
    auto levelConfig = GAMECONFIG.getWorldConfig(world).levels.at(level);
    auto particle = levelConfig.particle;

    if (particle == "rain")
    {
        cocos2d::ParticleSnow* rain = cocos2d::ParticleSnow::create();
        rain->setStartColor(cocos2d::Color4F(238 / 255.0F, 238 / 255.0F, 170 / 255.0F, 1.0F));
        rain->setEndColor(cocos2d::Color4F(255 / 255.0F, 238 / 255.0F, 221 / 255.0F, 1.0F));
        rain->setSpeedVar(50.0F);
        rain->setEmissionRate(255.0F);
        rain->setSpeedVar(40.0F);
        rain->setTangentialAccel(100.0F);
        rain->setAngle(-45);
        rain->setAngleVar(10);
        rain->setStartSize(2.0F);
        rain->setEndSize(2.0F);
        rain->setScale(CONSTANTS.getOffset() / 120.0F);
        //        rain->setDuration(255.0f);
        //        rain->setSourcePosition(Vec2(_visibleSize.width,_visibleSize.height
        //        ));
        addChild(rain);  //, 1, cocos2d::Vec2(1, 1), cocos2d::Vec2(1, 1));  // TODO check if correct
                         // TODO add to parallax?
    }
    else if (particle == "snow")
    {
        cocos2d::ParticleSnow* rain = cocos2d::ParticleSnow::create();
        rain->setSpeedVar(50.0F);
        rain->setSpeed(40.0F);
        rain->setScale(CONSTANTS.getOffset() / 50);
        addChild(rain);  //, 1, cocos2d::Vec2(1, 1), cocos2d::Vec2(1, 1));  // TODO check if correct
        //        addChild(rain,-4,Vec2(0.5,0),Vec2(mBgSky2->getPosition().x+mBgSky2->getContentSize().width,mBgSky2->getContentSize().height/2));
    }
}

void BackgroundNode::update(float dt)
{
    _parallaxNode->updateWithVelocity(cocos2d::Point(-10, 0), dt);
}
