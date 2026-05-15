#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include <Arduino.h>
#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "Config.h"

class PlayerManager {
public:
    /*  初始化 */
    void init();

    // 开始播放，支持传入起始偏移量（断点续播）
    void start(String name, unsigned long resumeMs = 0);

    // 停止播放
    void stop();

    // 核心心跳：放在主循环 loop 中驱动解码
    bool loop();

    // 获取当前播放进度（毫秒）
    unsigned long getElapsed();


    // 提供一个只读的 Getter 函数
    bool isPlaying() const;

private:
    AudioGeneratorMP3 *mp3 = nullptr;
    AudioFileSourceSD *file = nullptr;
    AudioOutputI2S *out = nullptr;
    bool m_isPlaying = false;
    unsigned long playStartTime = 0;
};
#endif