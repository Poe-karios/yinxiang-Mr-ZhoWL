#ifndef NFC_READER_H
#define NFC_READER_H

#include <Adafruit_PN532.h>

class NFCReader {
public:
    /* 构造函数 */
    NFCReader(Adafruit_PN532* n);

    // 返回：1-放置，2-移除，0-保持
    int update();

    // 只读接口：获取当前活跃的 UID
    String getActiveUID() const;
private:
    /* 成员变量 */
    String m_activeUID = "";
    int m_missingCount = 0;
    Adafruit_PN532* m_nfc;
};
#endif