#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include "Config.h"

class DisplayManager {


public:
    void init();

    // 核心改进：只在歌词变化时刷新文字区域，避免全屏闪烁
    void updateLyric(String lyric);

    // 唱片转动动画（每帧只画几条线，不影响播放）
    void drawDiscAnimation(bool isPlaying);

    void showWaiting();

    void showAlbumInfo(String name);

private:
    TFT_eSPI tft = TFT_eSPI();
    String lastLyric = "";
    int discAngle = 0;
};
#endif