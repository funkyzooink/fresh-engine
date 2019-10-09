/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "Utility.h"
#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "../GameData/Gamedata.h"

#include "cocos2d.h"
#include "external/flatbuffers/util.h"

// MARK: int to String

// template <typename T>
std::string Utility::numberToString(int t)
{
    return flatbuffers::NumToString(t);
}

// MARK: color Helper

cocos2d::Color4B Utility::colorConverter(const std::string& hexValue)
{
    unsigned int r = 0;
    unsigned int g = 0;
    unsigned int b = 0;
    unsigned int a = 0;
    sscanf(hexValue.c_str(), "%02x%02x%02x%02x", &r, &g, &b, &a);

    return cocos2d::Color4B(r, g, b, a);
}

cocos2d::Color4F Utility::colorConverterFloat(const std::string& hexValue)
{
    return cocos2d::Color4F(colorConverter(hexValue));
}

// MARK: Node Helper
cocos2d::MenuItem* Utility::addButton(cocos2d::MenuItem* button, const cocos2d::Point& position)
{
    // if button is nullptr, return
    if (button == nullptr)
    {
        return nullptr;
    }

    // position the button & add to _menu
    button->setPosition(position);
    return button;
}
cocos2d::Label* Utility::addLabel(const std::string& text, const std::string& font, float size,
                                  const std::string& color)
{
    return Utility::addLabel(text, font, size, Utility::colorConverter(color), cocos2d::Point::ZERO);
}

cocos2d::Label* Utility::addLabel(const std::string& text, const std::string& font, float size,
                                  const std::string& color, const cocos2d::Point& position)
{
    return Utility::addLabel(text, font, size, Utility::colorConverter(color), position);
}

cocos2d::Label* Utility::addLabel(const std::string& text, const std::string& font, float size,
                                  const cocos2d::Color4B& color)
{
    return Utility::addLabel(text, font, size, color, cocos2d::Point::ZERO);
}

cocos2d::Label* Utility::addLabel(const std::string& text, const std::string& font, float size,
                                  const cocos2d::Color4B& color, const cocos2d::Point& position)
{
    auto label = cocos2d::Label::createWithTTF(text, font, size);
    label->setPosition(position);
    label->setTextColor(color);
    label->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);  // should be default
    return label;
}

cocos2d::MenuItemLabel* Utility::addLabelButton(const std::string& text, const cocos2d::Point& position,
                                                const cocos2d::ccMenuCallback& callback)
{
    return addLabelButton(text, CONSTANTS.fontButton, CONSTANTS.getOffset() * 0.75, position, callback);
}

cocos2d::MenuItemLabel* Utility::addLabelButton(const std::string& text, const std::string& font, float size,
                                                const cocos2d::Point& position, const cocos2d::ccMenuCallback& callback)
{
    // create CCMenuItemLabel with LabelTTF
    auto label = cocos2d::Label::createWithTTF(text, font, size);
    auto menu = cocos2d::MenuItemLabel::create(label, callback);
    auto button = (cocos2d::MenuItemLabel*)addButton(menu, position);
    button->setColor(cocos2d::Color3B(Utility::colorConverter(GAMECONFIG.getTextColor())));
    return button;
}

cocos2d::MenuItemSprite* Utility::addSpriteButton(const std::string& frameName, const cocos2d::Point& position,
                                                  const cocos2d::ccMenuCallback& callback)
{
    // create MenuItemSprite
    return (cocos2d::MenuItemSprite*)addButton(
        cocos2d::MenuItemSprite::create(cocos2d::Sprite::createWithSpriteFrameName(frameName),
                                        cocos2d::Sprite::createWithSpriteFrameName(frameName), callback),
        position);
}

cocos2d::MenuItemSprite* Utility::addMusicButton(const cocos2d::ccMenuCallback& callback)
{
    std::string music1 = CONSTANTS.buttonIconMusicNormal;
    if (!GAMEDATA.getMusicEnabled())
    {
        music1 = CONSTANTS.buttonIconMusicDown;
    }

    auto _musicMenuItem = addSpriteButton(
        music1,
        cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width - CONSTANTS.getOffset() * 1,
                      cocos2d::Director::getInstance()->getVisibleSize().height / 2 - CONSTANTS.getOffset() / 2),
        callback);
    return _musicMenuItem;
}

cocos2d::MenuItemSprite* Utility::addFxButton(const cocos2d::ccMenuCallback& callback)
{
    std::string effect1 = CONSTANTS.buttonIconAudioFXNormal;
    if (!GAMEDATA.getAudioFxEnabled())
    {
        effect1 = CONSTANTS.buttonIconAudioFXDown;
    }

    auto _effectMenuItem = addSpriteButton(
        effect1,
        cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width - CONSTANTS.getOffset() * 1,
                      cocos2d::Director::getInstance()->getVisibleSize().height / 2 + CONSTANTS.getOffset() / 2),
        callback);
    return _effectMenuItem;
}
cocos2d::Sprite* Utility::createCrashCloud()
{
    auto cloudIndex = Utility::randomValueBetween(1, 4);
    std::string file = CONSTANTS.iconCrashCloud + Utility::numberToString(cloudIndex);
    cocos2d::Sprite* cloud = cocos2d::Sprite::createWithSpriteFrameName(file);

    cloud->setLocalZOrder(CONSTANTS.LocalZOrderEnum::CONTROLS_Z_ORDER);
    cloud->setVisible(false);

    return cloud;
}

// MARK: Math Helper

cocos2d::Rect Utility::calculateIntersection(const cocos2d::Rect& source, const cocos2d::Rect& destination)
{
    auto intersection = cocos2d::Rect(std::max(source.getMinX(), destination.getMinX()),
                                      std::max(source.getMinY(), destination.getMinY()), 0, 0);
    intersection.size.width = std::min(source.getMaxX(), destination.getMaxX()) - intersection.getMinX();
    intersection.size.height = std::min(source.getMaxY(), destination.getMaxY()) - intersection.getMinY();

    return intersection;
}

cocos2d::Point Utility::clampMovment(const cocos2d::Point& velocity)
{
    cocos2d::Point minMovement = cocos2d::Point(0.0, -CONSTANTS.getOffset() * 5);
    cocos2d::Point maxMovement = cocos2d::Point(CONSTANTS.getOffset() * 3, CONSTANTS.getOffset() * 5);
    return velocity.getClampPoint(minMovement, maxMovement);
}

cocos2d::Rect Utility::getBoundingBox(cocos2d::Node* node)
{
    auto bbox = node->getBoundingBox();  // change bbox size for better experienece
    bbox.origin.x = bbox.origin.x * 1.2;
    bbox.size.width = bbox.size.width * 0.8;
    return bbox;
}

static unsigned int randX = 123456789, randY = 362436069, randZ = 521288629;

int Utility::randomValueBetween(const int min, const int max)
{
    // TODO platform independent solution
    // TEST Marsaglia's xorshf  better performance than rand?
    int t;
    randX ^= randX << 16;
    randX ^= randX >> 5;
    randX ^= randX << 1;

    t = randX;
    randX = randY;
    randY = randZ;
    randZ = t ^ randX ^ randY;

    return (randZ % (max - min + 1) + min);
}
