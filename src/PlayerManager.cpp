#include "PlayerManager.h"

void PlayerManager::init() 
{
    // 初始化 I2S 输出到外部 DAC (MAX98357A)
    out = new AudioOutputI2S();
    out->SetPinout(Config::I2S_BCLK, Config::I2S_LRC, Config::I2S_DOUT);
    out->SetGain(Config::VOLUME);
}

// 开始播放，支持传入起始偏移量（断点续播）
// 默认参数在实现的时候 不需要写默认赋值
void PlayerManager::start(String name, unsigned long resumeMs) 
{
    stop(); // 启动前先清理旧句柄，防止内存泄漏
    
    String path = "/MP3/" + name + ".mp3";
    file = new AudioFileSourceSD(path.c_str());
    
    // 如果有断点，执行文件指针偏移
    if (resumeMs > 0) 
    {
        file->seek(resumeMs, SEEK_SET); 
    }

    mp3 = new AudioGeneratorMP3();
    if (mp3->begin(file, out)) 
    {
        m_isPlaying = true;
        // 补偿起始时间，使 getElapsed() 返回正确进度
        playStartTime = millis() - resumeMs; 
        Serial.println("Audio: Started " + path);
    }
}

void PlayerManager::stop() 
{
    if (mp3) 
    { 
        mp3->stop(); 
        delete mp3; 
        mp3 = nullptr; 
    }

    if (file) 
    { 
        file->close(); 
        delete file; 
        file = nullptr; 
    }
    m_isPlaying = false;
}

// 核心心跳：放在主循环 loop 中驱动解码
bool PlayerManager::loop() 
{
    if (!m_isPlaying || !mp3) 
    {
           return false;
    }
    if (mp3->isRunning()) 
    {
        if (!mp3->loop()) 
        { 
            // 播放自然结束
            stop();
            return false;
        }
        return true;
    }
    return false;
}

// 获取当前播放进度（毫秒）
unsigned long PlayerManager::getElapsed() 
{
    return m_isPlaying ? (millis() - playStartTime) : 0;
}

// 提供一个只读的 Getter 函数
bool PlayerManager::isPlaying() const
{
    return m_isPlaying;
}