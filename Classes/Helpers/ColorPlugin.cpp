/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "ColorPlugin.h"
#include "../GameData/Constants.h"
#include "../GameData/GameConfig.h"
#include "Utility.h"

#include "cocos2d.h"

cocos2d::Color4F ColorPlugin::colorHelper(cocos2d::Color4F start, cocos2d::Color4F end, float div)
{
    cocos2d::Color4F mixedColor;
    return mixedColor;
}

void ColorPlugin::initLayerBackground(cocos2d::TMXTiledMap* tiledMap, int worldID, cocos2d::Node* cameraFollowNode)
{
}
void ColorPlugin::initLayerColors(cocos2d::TMXTiledMap* tiledMap, int worldID, cocos2d::Node* cameraFollowNode,
                                  bool showItems)
{
}
void ColorPlugin::createPixelWall(cocos2d::Point initialPosition, cocos2d::Size initialSize,
                                  cocos2d::Node* cameraFollowNode)
{
}

cocos2d::Sprite* ColorPlugin::spriteWithColor(cocos2d::Color4F bgColor, cocos2d::Color4F fgColor, float tWidth,
                                              float tHeight, bool gradient)
{
    return nullptr;
}
