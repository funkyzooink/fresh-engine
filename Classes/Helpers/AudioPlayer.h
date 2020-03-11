/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef HELPERS_AUDIOPLAYER_H_
#define HELPERS_AUDIOPLAYER_H_

#include <string>

class AudioPlayer
{
  public:
    static void playFx(const std::string& audio);
    static void playMenuMusic();
    static void playMusicForWorld(int world);

    static void stopMusic();
    static void pauseMusic();
    static void resumeMusic();

  private:
    static void playMusic(const std::string& audio);

    static int musicAudioID;
};

#endif  // HELPERS_AUDIOPLAYER_H_
