/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef HELPERS_TILEHELPER_H_
#define HELPERS_TILEHELPER_H_

#include <vector>
#include "platform/CCPlatformMacros.h"
#include "math/CCGeometry.h"

class GameObject;

namespace cocos2d
{
class TMXLayer;
}  // namespace cocos2d

struct CollisionTile
{
    ssize_t tileIdx;
    int GID;
    cocos2d::Rect tileRect;
    float tilePositionX;
    float tilePositionY;
};

class TileHelper
{
  public:
    /**
     * check if object collides with tile and return where
     */
    static void collisionTile(GameObject* gameObject, cocos2d::TMXLayer* layer, cocos2d::Size mapSize,
                              cocos2d::Size mapTileSize, bool turnAround);

    /**
     * check if object collides with tile and return where
     */
    static void collisionTile(GameObject* gameObject, cocos2d::TMXLayer* layer, cocos2d::Size mapSize,
                              cocos2d::Size mapTileSize);

    /**
     * only check if object collides with any tile, we dont care where
     */
    static void simpleCollisionTile(GameObject* gameObject, cocos2d::TMXLayer* layer, cocos2d::Size mapSize,
                                    cocos2d::Size mapTileSize, float scale);

  private:
    /**
     *  returns a vector containing neighbouring tiles
     */
    static std::vector<CollisionTile> getSurroundingTilesAtPosition(const cocos2d::Point& position,
                                                                    cocos2d::TMXLayer* layer,
                                                                    const cocos2d::Size& mapSize,
                                                                    const cocos2d::Size& mapTileSize);

    static cocos2d::Point tileCoordinateForPosition(const cocos2d::Point& position, const cocos2d::Size& mapSize,
                                                    const cocos2d::Size& mapTileSize);

  public:
    static const int tileBelow;
    static const int tileAbove;
    static const int tileLeft;
    static const int tileRight;
    static const int tileBottomLeft;
    static const int tileBottomRight;
    static const int tileTopLeft;
    static const int tileTopRight;
};

#endif  // HELPERS_TILEHELPER_H_
