#include "ConfigLoader.h"


// 解析 config.ini: "UID=MusicName"
/* 默认参数在实现的时候 不需要写默认赋值. */
void ConfigLoader::loadFromFile(const char* path) 
{
    File f = SD.open(path);
    if (!f) return;
    
    playlist.clear();
    while (f.available()) 
    {
        String line = f.readStringUntil('\n');
        line.trim();
        if (line.startsWith("#") || line.length() == 0) 
        {
            continue;
        }

        int sep = line.indexOf('=');
        if (sep > 0) 
        {
            String uid = line.substring(0, sep);
            String file = line.substring(sep + 1);
            uid.trim(); file.trim();
            playlist[uid.c_str()] = file.c_str();
        }
    }
    f.close();
    Serial.println("Config: 映射关系加载完成");
}

String ConfigLoader::getMusicByUID(String uid) 
{
    auto it = playlist.find(uid.c_str());
    return (it != playlist.end()) ? String(it->second.c_str()) : "";
}