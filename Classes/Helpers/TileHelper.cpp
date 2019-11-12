/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "TileHelper.h"
#include "../GameData/Constants.h"
#include "../GameObjects/GameObject.h"
#include "Utility.h"
#include "cocos2d.h"

// original order
// 0 1 2
// 3 4 5
// 6 7 8

// sorted order
// 4 1 5
// 2   3
// 6 0 7

const int TileHelper::tileBelow = 0;
const int TileHelper::tileAbove = 1;
const int TileHelper::tileLeft = 2;
const int TileHelper::tileRight = 3;
const int TileHelper::tileTopLeft = 4;
const int TileHelper::tileTopRight = 5;
const int TileHelper::tileBottomLeft = 6;
const int TileHelper::tileBottomRight = 7;

// MARK: Tile Helpers
/// new framework

// get a tile coordinate for a position
cocos2d::Point TileHelper::tileCoordinateForPosition(const cocos2d::Point& position, const cocos2d::Size& mapSize,
                                                     const cocos2d::Size& mapTileSize)
{
    float x = floor(position.x / mapTileSize.width);
    float levelHeightInPixels = mapSize.height * mapTileSize.height;
    float y = floor((levelHeightInPixels - position.y) / mapTileSize.height);
    return cocos2d::Point(x, y);
}

std::vector<CollisionTile> TileHelper::getSurroundingTilesAtPosition(const cocos2d::Point& position,
                                                                     cocos2d::TMXLayer* layer,
                                                                     const cocos2d::Size& mapSize,
                                                                     const cocos2d::Size& mapTileSize)
{
    cocos2d::Point gameObjectTilePosition = tileCoordinateForPosition(position, mapSize, mapTileSize);
    std::vector<CollisionTile> tileVector;

    for (int i = 0; i < 9; i++)
    {  // 3
        int c = i % 3;
        int r = (int)(i / 3);
        cocos2d::Point tilePos = cocos2d::Point(gameObjectTilePosition.x + (c - 1), gameObjectTilePosition.y + (r - 1));

        int GID = 0;  // set to -1 in case there is no tile
        cocos2d::Rect tileRect;
        if ((tilePos.x >= 0 && tilePos.y >= 0) &&
            (tilePos.x < mapSize.width && tilePos.y < mapSize.height))  // check if position is on screen
        {
            GID = layer->getTileGIDAt(tilePos);
            auto sprite = layer->getTileAt(tilePos);
            if (sprite != nullptr)
            {
                tileRect = sprite->getBoundingBox();
            }
        }

        // even if for any reason there is no tile, save a dummy // TODO replace with optional
        CollisionTile tile = { 0, GID, tileRect, tilePos.x,
                               tilePos.y };  // we are setting the tileIDx later as the vector will be sorted

        tileVector.push_back(tile);
    }

    // 0 1 2
    // 3 4 5
    // 6 7 8
    // sorting the vector accoding to importance, first below, above, left right, etc
    // currently needed, removing this will break collision detection

    tileVector.erase(tileVector.begin() + 4);  // tile in the middle -> not
                                               // needed
    tileVector.insert(tileVector.begin() + 6, tileVector.at(2));
    tileVector.erase(tileVector.begin() + 2);
    std::iter_swap(tileVector.begin() + 4, tileVector.begin() + 6);
    std::iter_swap(tileVector.begin() + 0, tileVector.begin() + 4);

    // 4 1 5
    // 2   3
    // 6 0 7
    return tileVector;
}

void TileHelper::collisionTile(GameObject* gameObject, cocos2d::TMXLayer* layer, cocos2d::Size mapSize,
                               cocos2d::Size mapTileSize, bool turnAround)
{
    auto tileVector = TileHelper::getSurroundingTilesAtPosition(gameObject->getPosition(), layer, mapSize, mapTileSize);

    if (turnAround)
    {
        // if gameObject cannot jump or attack turn
        // around when
        // reaching empty tile
        auto leftTileGid = tileVector.at(TileHelper::tileBottomLeft).GID;
        auto rightTileGid = tileVector.at(TileHelper::tileBottomRight).GID;
        auto belowTile = tileVector.at(TileHelper::tileBelow);
        auto leftTileCoordinateX =
            belowTile.tileRect.origin.x + CONSTANTS.getOffset() * 0.1f;  // tile coordinate + width -
        auto rightTileCoordinateX =
            belowTile.tileRect.origin.x + belowTile.tileRect.size.width - CONSTANTS.getOffset() * 0.1f;

        if ((leftTileGid == 0 && gameObject->getHeadingState() == GameObject::HeadingStateEnum::LEFT_HEADING &&
             gameObject->getDesiredPosition().x <= leftTileCoordinateX) ||
            (gameObject->getDesiredPosition().x <= 0))
        {
            // no tile is left of enemy or start of tmx file
            // if enemy walks left tell him there is an obstacle
            gameObject->setObstacle(GameObject::ObstacleStateEnum::LEFT_OBSTACLE);
            gameObject->setPosition(gameObject->getDesiredPosition());  // 8
            return;
        }
        else if ((rightTileGid == 0 && gameObject->getHeadingState() == GameObject::HeadingStateEnum::RIGHT_HEADING &&
                  gameObject->getDesiredPosition().x >= rightTileCoordinateX) ||
                 (gameObject->getDesiredPosition().x >= mapSize.width * mapTileSize.width))
        {
            // no tile is right of enemy or end of tmx file
            // if enemy walks right tell him there is an obstacle
            gameObject->setObstacle(GameObject::ObstacleStateEnum::RIGHT_OBSTACLE);
            gameObject->setPosition(gameObject->getDesiredPosition());  // 8
            return;
        }
    }

    gameObject->setOnGround(false);
    for (ssize_t tileIndx = 0; tileIndx < tileVector.size(); ++tileIndx)
    {
        auto tile = tileVector.at(tileIndx);
        tile.tileIdx = tileIndx;  // TODO remove -> replace default callback with diagonal, then not needed
        cocos2d::Rect gameObjectBb = gameObject->getBoundingBox();

        int tileGID = tile.GID;

        if (tileGID > 0)
        {
            auto tileRect = tile.tileRect;
            if (gameObjectBb.intersectsRect(tileRect))
            {
                cocos2d::Rect intersection = Utility::calculateIntersection(gameObjectBb, tileRect);
                switch (tileIndx)
                {
                    case TileHelper::tileBelow: {
                        gameObject->belowCollisionCallback(tile, intersection);
                        break;
                    }
                    case TileHelper::tileAbove: {
                        gameObject->aboveCollisionCallback(tile, intersection);
                        break;
                    }
                    case TileHelper::tileLeft:  // || tileIndx==4)
                    {
                        gameObject->leftCollisionCallback(tile, intersection);
                        break;
                    }
                    case TileHelper::tileRight:  // || tileIndx==5)
                    {
                        gameObject->rightCollisionCallback(tile, intersection);
                        break;
                    }
                    default: {
                        gameObject->defaultCollisionCallback(tile, intersection);
                        break;
                    }
                }  // switch
            }      // intersect
        }          // git
        if (gameObject->getDesiredPosition().y + gameObject->getContentSize().height / 2 <= 0)
        {
            gameObject->setVisible(false);
        }
        else if (gameObject->getDesiredPosition().x <= 0)
        {
            gameObject->setDesiredPosition(
                cocos2d::Point(gameObject->getPosition().x, gameObject->getDesiredPosition().y));
            gameObject->setObstacle(GameObject::ObstacleStateEnum::LEFT_OBSTACLE);
            gameObject->setOnGround(true);

            // gameObject->setDesiredPosition(
            //     cocos2d::Point(gameObject->getDesiredPosition().x + gameObject->getContentSize().width / 4,
            //     gameObject->getDesiredPosition().y));
        }
        else if (gameObject->getDesiredPosition().x >= mapSize.width * mapTileSize.width)
        {
            gameObject->setDesiredPosition(
                cocos2d::Point(gameObject->getPosition().x, gameObject->getDesiredPosition().y));
            gameObject->setObstacle(GameObject::ObstacleStateEnum::RIGHT_OBSTACLE);
            gameObject->setOnGround(true);

            // gameObject->setDesiredPosition(
            //     cocos2d::Point(gameObject->getDesiredPosition().x - gameObject->getContentSize().width / 4,
            //     gameObject->getDesiredPosition().y));
        }
    }  // for

    gameObject->setPosition(gameObject->getDesiredPosition());  // 8
}

void TileHelper::collisionTile(GameObject* gameObject, cocos2d::TMXLayer* layer, cocos2d::Size mapSize,
                               cocos2d::Size mapTileSize)
{
    TileHelper::collisionTile(gameObject, layer, mapSize, mapTileSize, false);
}

void TileHelper::simpleCollisionTile(GameObject* gameObject, cocos2d::TMXLayer* layer, cocos2d::Size mapSize,
                                     cocos2d::Size mapTileSize, float scale)
{
    if (layer == nullptr)
    {  // if ther is e.g. no hazard layer
        return;
    }

    auto tileVector = TileHelper::getSurroundingTilesAtPosition(gameObject->getPosition(), layer, mapSize, mapTileSize);
    for (auto tile : tileVector)
    {
        auto gameObjectBb = gameObject->getBoundingBox();
        int tileGID = tile.GID;
        if (tileGID > 0)
        {
            auto tileRect = cocos2d::Rect(tile.tileRect.origin.x, tile.tileRect.origin.y, mapTileSize.width * scale,
                                          mapTileSize.height * scale);
            if (gameObjectBb.intersectsRect(tileRect))
            {
                gameObject->simpleCollisionCallback();
                return;
            }
        }
    }
}
