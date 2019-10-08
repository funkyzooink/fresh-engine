/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef HELPERS_COLORPLUGIN_H_
#define HELPERS_COLORPLUGIN_H_

#include "math/Vec2.h"

namespace cocos2d
{
class TMXTiledMap;
class Node;
class Size;
class Color4F;
class Sprite;
}  // namespace cocos2d

class ColorPlugin
{
  public:
    static void initLayerColors(cocos2d::TMXTiledMap* tiledMap, int worldID, cocos2d::Node* cameraFollowNode,
                                bool showItems);
    static void initLayerBackground(cocos2d::TMXTiledMap* tiledMap, int worldID, cocos2d::Node* cameraFollowNode);

  private:
    static void createPixelWall(cocos2d::Point position, cocos2d::Size size, cocos2d::Node* cameraFollowNode);
    static cocos2d::Color4F colorHelper(cocos2d::Color4F start, cocos2d::Color4F end, float div);
    static cocos2d::Sprite* spriteWithColor(cocos2d::Color4F bgColor, cocos2d::Color4F fgColor, float tWidth,
                                            float tHeight, bool gradient);
};

#endif  // HELPERS_COLORPLUGIN_H_
