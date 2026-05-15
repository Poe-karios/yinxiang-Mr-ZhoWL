#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <SD.h>
#include <unordered_map>

class SystemState {
public:

    // 写入内存
    void setPos(String uid, unsigned long pos);

    // 获取历史进度
    unsigned long getPos(String uid);

    // 将进度保存到 SD 卡 (state.bin)
    void saveToSD();

    // 开机加载进度
    void loadFromSD();
private:
    std::unordered_map<std::string, unsigned long> resumePoints;
    // 数据是否发生改变
    bool isDirty = false; 
};
#endif