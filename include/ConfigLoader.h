#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <SD.h>
#include <unordered_map>
#include <string>

class ConfigLoader {
public:
    // 解析 config.ini: "UID=MusicName"
    void loadFromFile(const char* path = "/config.ini");

    // 根据UID获取音乐文件名
    String getMusicByUID(String uid); 

private:
    // 使用哈希表存储映射，查询效率 O(1)
    std::unordered_map<std::string, std::string> playlist;
};
#endif