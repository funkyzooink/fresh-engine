/****************************************************************************
 Copyright (c) 2014-2019 Gabriel Heilig
 fresh-engine
 funkyzooink@gmail.com
 ****************************************************************************/
#ifndef HELPERS_BACKGROUNDNODE_H_
#define HELPERS_BACKGROUNDNODE_H_

#include "cocos2d.h"

class CCParallaxScrollNode;

class BackgroundNode : public cocos2d::Layer
{
  public:
    BackgroundNode();

    /**
     * create Background Node for World nad Level
     */
    static BackgroundNode* createWithWorldAndLevelId(int world, int level);

    /**
     * create Background Node for MainScene
     */
    static BackgroundNode* createWithBackgrounds();

    void update(float dt);

  private:
    CREATE_FUNC(BackgroundNode);
    void addParallaxBackground(std::vector<std::string> backgrounds, std::vector<float> parallaxRatio);
    void addParticles(int world, int level);

    CCParallaxScrollNode* _parallaxNode;
};

#endif  // HELPERS_BACKGROUNDNODE_H_
