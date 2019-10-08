/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef HELPERS_ANIMATIONHELPER_H_
#define HELPERS_ANIMATIONHELPER_H_

//#include <string>
//#include <map>
#include "cocos2d.h"

namespace cocos2d
{
struct Color4B;
struct Color4F;
class Animation;
class FiniteTimeAction;
class Sprite;
class Scene;
class Rect;
class Node;
class MenuItemSprite;
class Label;
class TransitionScene;
}  // namespace cocos2d

// TODO forward declaration cleanup

class AnimationHelper
{
  public:
    enum AnimationTagEnum
    {
        IDLE_LEFT_ANIMATION = 299,
        IDLE_RIGHT_ANIMATION = 300,
        FALL_LEFT_ANIMATION = 301,
        JUMP_LEFT_ANIMATION = 302,
        FALL_RIGHT_ANIMATION = 303,
        JUMP_RIGHT_ANIMATION = 304,
        WALK_LEFT_ANIMATION = 305,
        WALK_RIGHT_ANIMATION = 306,
        ATTACK_LEFT_ANIMATION = 307,
        ATTACK_RIGHT_ANIMATION = 308,
        HIT_LEFT_ANIMATION = 309,
        HIT_RIGHT_ANIMATION = 310
    };

    // MARK: Animation Helper
    /**
     * levelinfoFadeInAnimation
     */
    static void levelinfoFadeInAnimation(cocos2d::Node* node1, cocos2d::Node* node2);
    /**
     * levelInfoFadeOutAnimation
     */
    static void levelInfoFadeOutAnimation(cocos2d::Node* node1, cocos2d::Node* node2);
    /**
     * fadeIn
     */
    static void fadeIn(cocos2d::Node* from, cocos2d::Node* to);
    /**
     * fadeOut
     */
    static void fadeOut(cocos2d::Node* from, cocos2d::Node* to);

    /**
     * sceneTransition
     */
    static cocos2d::TransitionScene* sceneTransition(cocos2d::Scene* scene);
    /**
     * blink animation
     */
    static cocos2d::FiniteTimeAction* blinkAnimation();

    /**
     * getActionForTag
     */
    static cocos2d::Action* getActionForTag(const std::string& tag);

    // MARK: Object Creation
    /**
     * initAnimations
     */
    static std::map<AnimationHelper::AnimationTagEnum, std::string>
    initAnimations(const std::string& type, const std::map<std::string, std::vector<std::string>>& animationMap);

    /**
     * prepare animation Vector
     */
    static cocos2d::Animation* prepareVector(const std::vector<std::string>& filenames, float duration);

};  // class AnimationHelper

#endif  // HELPERS_ANIMATIONHELPER_H_
