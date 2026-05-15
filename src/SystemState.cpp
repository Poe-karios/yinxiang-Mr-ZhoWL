#include "SystemState.h"



// 写入内存
void SystemState::setPos(String uid, unsigned long pos) 
{
    if (resumePoints[uid.c_str()] != pos) {
        resumePoints[uid.c_str()] = pos;
        isDirty = true;
    }
}

// 获取历史进度
unsigned long SystemState::getPos(String uid) 
{
    return resumePoints[uid.c_str()];
}

// 将进度保存到 SD 卡 (state.bin)
void SystemState::saveToSD() 
{
    if (!isDirty) return;
    File f = SD.open("/state.bin", FILE_WRITE);
    if (f) 
    {
        for (auto const& [uid, pos] : resumePoints) 
        {
            f.printf("%s=%lu\n", uid.c_str(), pos);
        }
        f.close();
        isDirty = false;
    }
}

// 开机加载进度
void SystemState::loadFromSD() 
{
    if (!SD.exists("/state.bin")) return;
    File f = SD.open("/state.bin", FILE_READ);
    while (f.available()) 
    {
        String line = f.readStringUntil('\n');
        int sep = line.indexOf('=');
        if (sep > 0) 
        {
            resumePoints[line.substring(0, sep).c_str()] = line.substring(sep + 1).toInt();
        }
    }
    f.close();
}