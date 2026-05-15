#include "NFCReader.h"


NFCReader::NFCReader(Adafruit_PN532* n) : m_nfc(n) {}

// 返回：1-放置，2-移除，0-保持
int NFCReader::update() 
{
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t uidLen;

    // 非阻塞读取 (超时100ms)
    if (m_nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 100)) 
    {
        String currentUID = "";
        for (uint8_t i = 0; i < uidLen; i++) 
        {
            if (uid[i] < 16) currentUID += "0";
            currentUID += String(uid[i], HEX);
        }
        currentUID.toUpperCase();
        m_missingCount = 0;

        if (m_activeUID == "") 
        {
            m_activeUID = currentUID;
            return 1; // 放置新卡
        }
        return 0;
    } 
    else 
    {
        if (m_activeUID != "") 
        {
            m_missingCount++;
            if (m_missingCount >= 3) 
            { 
                // 连续3次读不到，判定为移除
                m_activeUID = "";
                return 2;
            }
        }
        return 0;
    }
}


// 只读接口：获取当前活跃的 UID
String NFCReader::getActiveUID() const
{
    return m_activeUID;
}
    