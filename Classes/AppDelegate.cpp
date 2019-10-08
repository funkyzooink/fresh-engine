#include "AppDelegate.h"

#include "GameData/Constants.h"
#include "GameData/GameConfig.h"
#include "Helpers/AudioPlayer.h"
#include "Helpers/Localization.h"
#include "Scenes/BootScene.h"
#include "Scenes/GameScene.h"

USING_NS_CC;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes:
    // red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8, 0 };

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director = Director::getInstance();

    auto title = Localization::getLocalizedString(GAMECONFIG.getMainSceneConfig()->title);
    auto glview = director->getOpenGLView();
    if (!glview)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) ||                            \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithFullScreen(title);  //, cocos2d::Rect(0, 0, designResolution.width,
                                                           // designResolution.height));
#else
        glview = GLViewImpl::create(title);
#endif
        director->setOpenGLView(glview);
    }

    auto resolutions = GAMECONFIG.getSpriteResolutions();
    cocos2d::Size designResolution;
    std::string tileSizeIdentifier = "";

    auto orientation = GAMECONFIG.getDeviceOrientation();

    ResolutionPolicy policy = ResolutionPolicy::FIXED_HEIGHT;
    auto visibleSize = Director::getInstance()->getVisibleSize().width;
    if (orientation == "portrait")
    {
        visibleSize = Director::getInstance()->getVisibleSize().height;
        policy = ResolutionPolicy::FIXED_WIDTH;
    }

    for (auto const& resolution : resolutions)
    {
        // set to current resolution
        designResolution = resolution.size;
        tileSizeIdentifier = resolution.tileMapIdentifier;

        auto resolutionCompareSize = resolution.size.width;
        if (orientation == "portrait")
        {
            resolutionCompareSize = resolution.size.height;
        }

        if (visibleSize > resolutionCompareSize * 1.2)  // if bigger than
                                                        // current resolution then
                                                        // continue loop to use next
        {
            continue;
        }
        else  // otherwise abort loop
        {
            break;
        }
    }

    // turn on display FPS
    //	director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0F / 60);

    glview->setDesignResolutionSize(designResolution.width * 0.9, designResolution.height * 0.9, policy);

    FileUtils::getInstance()->addSearchResolutionsOrder("img/" + tileSizeIdentifier);
    CONSTANTS.setTileSizeIdentifier(tileSizeIdentifier);
    // create a scene. it's an autorelease object
    auto scene = BootScene::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a
// phone call it is invoked.
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();
    Scene* scene = Director::getInstance()->getRunningScene();
    if (dynamic_cast<GameScene*>(scene->getChildByTag(998)) != nullptr)
    {
        GameScene* gameScene = dynamic_cast<GameScene*>(scene->getChildByTag(998));
        gameScene->applicationDidEnterBackground();
    }

    AudioPlayer::pauseMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

    AudioPlayer::resumeMusic();
}

void AppDelegate::applicationScreenSizeChanged(int newWidth, int newHeight)
{
    // TODO do we want apps that have both, landscape and portrait?
    //    auto director = Director::getInstance();
    //    auto glview = director->getOpenGLView();
    //    if (glview)
    //    {
    //        auto size = glview->getFrameSize();
    //        if (size.equals(Size(newWidth, newHeight)))
    //            return;
    //
    //        glview->setFrameSize(newWidth, newHeight);
    //
    //        ResolutionPolicy resolutionPolicy = glview->getResolutionPolicy();
    //        if (size.width < newWidth)
    //        {  // switch to landscape
    //            resolutionPolicy = ResolutionPolicy::FIXED_HEIGHT;
    //        }
    //        else
    //        {  // switch to portrait
    //            resolutionPolicy = ResolutionPolicy::FIXED_WIDTH;
    //        }
    //
    //        glview->setDesignResolutionSize(newWidth, newHeight, resolutionPolicy);
    //    }
}
