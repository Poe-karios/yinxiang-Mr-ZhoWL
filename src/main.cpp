#include <Arduino.h>
#include "Config.h"
#include "ConfigLoader.h"
#include "SystemState.h"
#include "NFCReader.h"
#include "PlayerManager.h"
#include "DisplayManager.h"

// --- 全局实例 ---
Adafruit_PN532 nfc_hw(-1, -1);
NFCReader nfc_logic(&nfc_hw);
ConfigLoader cfg;
SystemState state;
PlayerManager player;
DisplayManager ui;

// --- 歌词处理相关变量 (沿用你之前的逻辑) ---
struct Lyric {
    unsigned long time;
    String text;
};
Lyric lyrics[30]; // 稍微加大一点容量
int lyricCount = 0;
int lastShowIdx = -1;

// 解析 LRC 时间戳
unsigned long parseTime(String s) {
    if (!s.startsWith("[")) return 0;
    s.remove(0, 1);
    s.remove(s.length() - 1, 1);
    int m = s.substring(0, s.indexOf(':')).toInt();
    float s_val = s.substring(s.indexOf(':') + 1).toFloat();
    return m * 60000 + (unsigned long)(s_val * 1000);
}

// 加载 LRC 到内存
void loadLRC(String name) {
    lyricCount = 0;
    lastShowIdx = -1;
    File f = SD.open("/LRC/" + name + ".lrc");
    if (!f) return;

    while (f.available() && lyricCount < 30) {
        String line = f.readStringUntil('\n');
        line.trim();
        if (line.startsWith("[")) {
            int close = line.indexOf(']');
            if (close < 0) continue;
            lyrics[lyricCount].time = parseTime(line.substring(0, close + 1));
            lyrics[lyricCount].text = line.substring(close + 1);
            lyrics[lyricCount].text.trim();
            if (lyrics[lyricCount].text.length() > 0) lyricCount++;
        }
    }
    f.close();
}

// 歌词显示逻辑
void handleLyricDisplay() {
    if (!player.isPlaying() || lyricCount == 0) return;

    unsigned long currentPos = player.getElapsed();
    int idx = -1;
    for (int i = 0; i < lyricCount; i++) {
        if (currentPos >= lyrics[i].time) idx = i;
    }

    if (idx != -1 && idx != lastShowIdx) {
        ui.updateLyric(lyrics[idx].text);
        lastShowIdx = idx;
    }
}

// --- 初始化 ---
void setup() {
    Serial.begin(115200);
    
    // 1. 初始化硬件
    if (!SD.begin(Config::SD_CS)) {
        Serial.println("SD Card Failed!");
    }
    nfc_hw.begin();
    nfc_hw.SAMConfig();

    // 2. 初始化各管理器
    ui.init();
    player.init();
    cfg.loadFromFile();   // 加载 UID=歌曲 映射
    state.loadFromSD();    // 加载历史断点

    ui.showWaiting();
}

// --- 主循环 ---
void loop() {
    // 1. NFC 扫描状态机 (0:无变化, 1:放置, 2:拿起)
    int nfcAction = nfc_logic.update();

    if (nfcAction == 1) { 
        // 【动作：放下唱片】
        String musicName = cfg.getMusicByUID(nfc_logic.getActiveUID());
        if (musicName != "") {
            unsigned long resumeMs = state.getPos(nfc_logic.getActiveUID());
            loadLRC(musicName);      // 加载歌词
            player.start(musicName, resumeMs); // 带断点播放
            ui.showAlbumInfo(musicName);
        }
    } 
    else if (nfcAction == 2) {
        // 【动作：拿起唱片】
        if (player.isPlaying()) {
            // 保存当前进度
            state.setPos(nfc_logic.getActiveUID(), player.getElapsed());
            state.saveToSD(); 
            player.stop();
        }
        ui.showWaiting();
    }

    // 2. 播放状态处理
    if (player.isPlaying()) {
        if (player.loop()) { // 驱动解码引擎
            handleLyricDisplay();    // 更新歌词显示
            ui.drawDiscAnimation(true); // 更新唱片旋转动画
            
            // 每 5 秒自动落盘一次进度，防止意外断电丢失
            static unsigned long autoSaveTimer = 0;
            if (millis() - autoSaveTimer > 5000) {
                state.setPos(nfc_logic.getActiveUID(), player.getElapsed());
                state.saveToSD();
                autoSaveTimer = millis();
            }
        } else {
            // 歌曲播放完毕
            state.setPos(nfc_logic.getActiveUID(), 0); // 重置进度
            state.saveToSD();
            ui.showWaiting();
        }
    }
}