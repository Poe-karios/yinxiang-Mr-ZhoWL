#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace Config {
    // SD卡 SPI 引脚 (ESP32 默认 VSPI)
    const int SD_CS = 5;

    // I2S 音频引脚 (输出至 MAX98357A 或同类 DAC)
    const int I2S_BCLK = 26; // 位时钟
    const int I2S_LRC = 27;  // 左右声道切换
    const int I2S_DOUT = 25; // 数据信号

    // NFC I2C 引脚 (PN532)
    const int SDA_PIN = 21;
    const int SCL_PIN = 22;

    // 屏幕参数
    const int SCREEN_WIDTH = 128;
    const int SCREEN_HEIGHT = 160;

    // 业务参数
    const float VOLUME = 0.15;              // 初始音量 (0.0~1.0)
    const unsigned long SAVE_INTERVAL = 3000; // 每3秒检查一次是否需要保存进度
}
#endif