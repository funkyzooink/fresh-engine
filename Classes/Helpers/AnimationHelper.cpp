/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "AnimationHelper.h"
#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"

#include "cocos2d.h"

// MARK: Animation Helper
void AnimationHelper::levelinfoFadeInAnimation(cocos2d::Node* node1, cocos2d::Node* node2)
{
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    switch (GAMECONFIG.getLevelInfoPopupType())
    {
        case 1:
        {
            node1->runAction(cocos2d::Sequence::create(
                cocos2d::MoveTo::create(
                    1.0F, cocos2d::Vec2(visibleSize.width / 2 - CONSTANTS.getOffset() * 2, node1->getPosition().y)),
                nullptr));
            node2->runAction(cocos2d::Sequence::create(
                cocos2d::MoveTo::create(1.0F, cocos2d::Vec2(visibleSize.width / 2 - node2->getContentSize().width / 2,
                                                            node2->getPosition().y)),
                nullptr));
            break;
        }
        default:
        {
            auto position = node1->getPosition();
            node1->setPosition(position.x, visibleSize.height);
            node2->setVisible(true);
            node1->runAction(cocos2d::Sequence::create(cocos2d::MoveTo::create(1.0F, position), nullptr));
            break;
        }
    }
}

void AnimationHelper::levelInfoFadeOutAnimation(cocos2d::Node* node1, cocos2d::Node* node2)
{
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    switch (GAMECONFIG.getLevelInfoPopupType())
    {
        case 1:
        {
            node1->runAction(cocos2d::Sequence::create(
                cocos2d::MoveTo::create(1.0F, cocos2d::Vec2(visibleSize.width, node1->getPosition().y)), nullptr));
            node2->runAction(cocos2d::Sequence::create(
                cocos2d::MoveTo::create(1.5F, cocos2d::Vec2(-visibleSize.width, node2->getPosition().y)), nullptr));
            break;
        }
        default:
        {
            node2->setVisible(true);
            node1->runAction(cocos2d::Sequence::create(
                cocos2d::MoveTo::create(1.5F, cocos2d::Vec2(node1->getPosition().x, -visibleSize.height)), nullptr));
            break;
        }
    }
}

void AnimationHelper::fadeIn(cocos2d::Node* from, cocos2d::Node* to)
{
    auto duration = 0.8F;
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    if (to != nullptr)
    {
        to->setVisible(true);  // make sure both nodes are visible

        auto toPosition = cocos2d::Vec2(to->getPosition().x, 0.0);
        switch (GAMECONFIG.getMainSceneAnimation())
        {
            case 1:
            {
                to->setPosition(toPosition.x, visibleSize.height);
                to->runAction(cocos2d::Sequence::create(cocos2d::MoveTo::create(duration, toPosition), nullptr));
                break;
            }

            default:
            {
                to->setPosition(toPosition);
                break;
            }
        }
    }

    if (from != nullptr)
    {
        from->setVisible(true);  // make sure both nodes are visible
        auto fromPosition = cocos2d::Vec2(from->getPosition().x, -visibleSize.height);
        switch (GAMECONFIG.getMainSceneAnimation())
        {
            case 1:
            {
                from->setPosition(0.0, 0.0);
                from->runAction(cocos2d::Sequence::create(cocos2d::MoveTo::create(duration, fromPosition), nullptr));
                break;
            }
            default:
            {
                from->setPosition(fromPosition);
                break;
            }
        }
    }
}

void AnimationHelper::fadeOut(cocos2d::Node* from, cocos2d::Node* to)
{
    auto duration = 0.8F;
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    if (to != nullptr)
    {
        to->setVisible(true);  // make sure both nodes are visible

        auto toPosition = cocos2d::Vec2(to->getPosition().x, 0.0F);
        switch (GAMECONFIG.getMainSceneAnimation())
        {
            case 1:
            {
                to->setPosition(toPosition.x, -visibleSize.height);
                to->runAction(cocos2d::Sequence::create(cocos2d::MoveTo::create(duration, toPosition), nullptr));
                break;
            }

            default:
            {
                to->setPosition(toPosition);
                break;
            }
        }
    }

    if (from != nullptr)
    {
        from->setVisible(true);  // make sure both nodes are visible
        auto fromPosition = cocos2d::Vec2(from->getPosition().x, visibleSize.height);
        switch (GAMECONFIG.getMainSceneAnimation())
        {
            case 1:
            {
                from->setPosition(0.0, 0.0);
                from->runAction(cocos2d::Sequence::create(cocos2d::MoveTo::create(duration, fromPosition), nullptr));
                break;
            }

            default:
            {
                from->setPosition(fromPosition);
                break;
            }
        }
    }
}

cocos2d::TransitionScene* AnimationHelper::sceneTransition(cocos2d::Scene* scene)
{
    return cocos2d::TransitionFade::create(0.2f, scene);
}

cocos2d::FiniteTimeAction* AnimationHelper::blinkAnimation()
{
    float blinkDuration = 0.05F;
    return cocos2d::Sequence::create(cocos2d::FadeOut::create(blinkDuration), cocos2d::FadeIn::create(blinkDuration),
                                     cocos2d::FadeOut::create(blinkDuration), cocos2d::FadeIn::create(blinkDuration),
                                     nullptr);
}

cocos2d::Action* AnimationHelper::getActionForTag(const std::string& tag)
{
    auto animationCache = cocos2d::AnimationCache::getInstance()->getAnimation(tag);

    if (animationCache == nullptr)
    {
        return nullptr;
    }
    auto animation = cocos2d::Animate::create(animationCache);
    auto repeatForever = cocos2d::RepeatForever::create(animation);
    return repeatForever;
}

std::map<AnimationHelper::AnimationTagEnum, std::string> AnimationHelper::initAnimations(
    const std::string& type, const std::map<std::string, std::vector<std::string>>& animationMap)
{
    std::map<AnimationHelper::AnimationTagEnum, std::string> _animationEnumMap;
    _animationEnumMap[AnimationHelper::AnimationTagEnum::ATTACK_LEFT_ANIMATION] = type + "_attack_left";
    _animationEnumMap[AnimationHelper::AnimationTagEnum::FALL_LEFT_ANIMATION] = type + "_jump_left_down";
    _animationEnumMap[AnimationHelper::AnimationTagEnum::HIT_LEFT_ANIMATION] = type + "_hit_left";
    _animationEnumMap[AnimationHelper::AnimationTagEnum::IDLE_LEFT_ANIMATION] = type + "_idle_left";
    _animationEnumMap[AnimationHelper::AnimationTagEnum::JUMP_LEFT_ANIMATION] = type + "_jump_left_up";
    _animationEnumMap[AnimationHelper::AnimationTagEnum::WALK_LEFT_ANIMATION] = type + "_walk_left";
    _animationEnumMap[AnimationHelper::AnimationTagEnum::ATTACK_RIGHT_ANIMATION] = type + "_attack_right";
    _animationEnumMap[AnimationHelper::AnimationTagEnum::FALL_RIGHT_ANIMATION] = type + "_jump_right_down";
    _animationEnumMap[AnimationHelper::AnimationTagEnum::HIT_RIGHT_ANIMATION] = type + "_hit_right";
    _animationEnumMap[AnimationHelper::AnimationTagEnum::IDLE_RIGHT_ANIMATION] = type + "_idle_right";
    _animationEnumMap[AnimationHelper::AnimationTagEnum::JUMP_RIGHT_ANIMATION] = type + "_jump_right_up";
    _animationEnumMap[AnimationHelper::AnimationTagEnum::WALK_RIGHT_ANIMATION] = type + "_walk_right";

    cocos2d::Animation* animation = nullptr;

    for (auto const& entry : animationMap)
    {
        auto animationTag = type + "_" + entry.first;  // TODO this needs to be the same as in animationEnumMap
        // TODO check if created tag is part of animationEnum otherwise abort
        animation = prepareVector(entry.second, 0.2F);
        // check times! 0.5 for staticshooter idle
        // check times! 0.2 for jumps falls and walking animation
        cocos2d::AnimationCache::getInstance()->addAnimation(animation, animationTag);
    }

    return _animationEnumMap;
}

cocos2d::Animation* AnimationHelper::prepareVector(const std::vector<std::string>& filenames, float duration)
{
    cocos2d::Vector<cocos2d::SpriteFrame*> spriteVector(filenames.size());
    for (const std::string& file : filenames)
    {
        cocos2d::SpriteFrame* spriteFrame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(file);
        spriteVector.pushBack(spriteFrame);
    }

    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(spriteVector, duration, 1);
    return animation;
};
