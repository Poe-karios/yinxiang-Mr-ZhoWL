#include "DisplayManager.h"

void DisplayManager::init() 
{
    tft.init();
    tft.setRotation(2); // 保持你之前的旋转方向
    showWaiting();
}

// 核心改进：只在歌词变化时刷新文字区域，避免全屏闪烁
void DisplayManager::updateLyric(String lyric) 
{
    if (lyric == lastLyric) return; // 没变化不刷屏
    
    // 只擦除下半部分的歌词区域 (假设坐标 100-160)
    tft.fillRect(0, 100, 128, 60, TFT_BLACK); 
    
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(1);
    tft.setTextDatum(TC_DATUM); // 顶部中心对齐
    
    // 简单的自动换行逻辑
    if (lyric.length() > 20) {
        String line1 = lyric.substring(0, 20);
        String line2 = lyric.substring(20);
        tft.drawString(line1, 64, 110);
        tft.drawString(line2, 64, 125);
    } else {
        tft.drawString(lyric, 64, 115);
    }
    
    lastLyric = lyric;
}

// 唱片转动动画（每帧只画几条线，不影响播放）
void DisplayManager::drawDiscAnimation(bool isPlaying) 
{
    if (!isPlaying) return;

    static unsigned long lastAnim = 0;
    if (millis() - lastAnim < 100) return; // 限制 10fps
    lastAnim = millis();

    int cx = 64, cy = 50, r = 35;
    
    // 绘制唱片外圈
    tft.drawCircle(cx, cy, r, TFT_DARKGREY);
    tft.drawCircle(cx, cy, 5, TFT_WHITE); // 唱片中心孔

    // 计算转动的指针线
    float rad = discAngle * 0.01745; 
    int px = cx + cos(rad) * r;
    int py = cy + sin(rad) * r;
    
    // 擦除旧线 (用黑色画一遍旧的)
    float oldRad = (discAngle - 10) * 0.01745;
    tft.drawLine(cx, cy, cx + cos(oldRad) * r, cy + sin(oldRad) * r, TFT_BLACK);
    
    // 画新线
    tft.drawLine(cx, cy, px, py, TFT_GOLD);

    discAngle = (discAngle + 10) % 360;
}

void DisplayManager::showWaiting() 
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("WAITING NFC...", 64, 80);
    lastLyric = ""; // 重置歌词缓存
}

void DisplayManager::showAlbumInfo(String name) 
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("ALBUM: " + name, 64, 5);
    // 播放开始时画一个基础圆
    tft.drawCircle(64, 50, 35, TFT_WHITE);
}

