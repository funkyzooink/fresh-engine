/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef HELPERS_UTILITY_H_
#define HELPERS_UTILITY_H_

#include <string>
#include "math/Vec2.h"
#include "2d/CCMenuItem.h"

namespace cocos2d
{
struct Color4B;
struct Color4F;
class Animation;
class FiniteTimeAction;
class Sprite;
class Rect;
class Node;
class MenuItemSprite;
class Label;
class TransitionScene;
}  // namespace cocos2d

class Utility
{
    // MARK: int to String
  public:
    // template <typename T>
    static std::string numberToString(int t);

    // MARK: color Helper

    static cocos2d::Color4B colorConverter(const std::string& hexValue);
    static cocos2d::Color4F colorConverterFloat(const std::string& hexValue);

    // MARK: Node Helper
    static cocos2d::MenuItemSprite* addMusicButton(const cocos2d::ccMenuCallback& callback);
    static cocos2d::MenuItemSprite* addFxButton(const cocos2d::ccMenuCallback& callback);
    static cocos2d::MenuItem* addButton(cocos2d::MenuItem* button, const cocos2d::Point& position);

    // Creates & adds a cocos2d::LabelTTF
    static cocos2d::Label* addLabel(const std::string& text, const std::string& font, float size,
                                    const cocos2d::Color4B& color);
    static cocos2d::Label* addLabel(const std::string& text, const std::string& font, float size,
                                    const cocos2d::Color4B& color, const cocos2d::Point& position);

    static cocos2d::Label* addLabel(const std::string& text, const std::string& font, float size,
                                    const std::string& color);
    static cocos2d::Label* addLabel(const std::string& text, const std::string& font, float size,
                                    const std::string& color, const cocos2d::Point& position);

    // Creates & adds a cocos2d::MenuItemLabel using a cocos2d::LabelTTF
    static cocos2d::MenuItemLabel* addLabelButton(const std::string& text, const cocos2d::Point& position,
                                                  const cocos2d::ccMenuCallback& callback);

    static cocos2d::MenuItemLabel* addLabelButton(const std::string& text, const std::string& font, float size,
                                                  const cocos2d::Point& position,
                                                  const cocos2d::ccMenuCallback& callback);
    // Creates & adds a cocos2d::MenuItemSprite with specified frameName
    static cocos2d::MenuItemSprite* addSpriteButton(const std::string& frameName, const cocos2d::Point& position,
                                                    const cocos2d::ccMenuCallback& callback);
    static cocos2d::Sprite* createCrashCloud();

    // MARK: Math Helper
    static cocos2d::Rect calculateIntersection(const cocos2d::Rect& source, const cocos2d::Rect& destination);
    static cocos2d::Point clampMovment(const cocos2d::Point& velocity);
    static cocos2d::Rect getBoundingBox(cocos2d::Node* node);
    static int randomValueBetween(const int min, const int max);

};  // class utility

#endif  // HELPERS_UTILITY_H_
