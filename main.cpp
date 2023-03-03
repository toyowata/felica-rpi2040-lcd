/* Mbed Microcontroller Library
 * Copyright (c) 2019-2023 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "ST7735S.h"
#include "RCS620S.h"
#include "Arial12x12.h"
#include "ArialR16x17.h"
#include "ArialR20x20.h"
#include "ArialR32x34.h"

// RCS620S
#define COMMAND_TIMEOUT               400
#define POLLING_INTERVAL              500ms
#define RCS620S_MAX_RESPONSE_LEN      30
 
// FeliCa Service/System Code
#define CYBERNE_SYSTEM_CODE           0x0003
#define COMMON_SYSTEM_CODE            0xFE00
#define PASSNET_SERVICE_CODE          0x090F
#define FELICA_ATTRIBUTE_CODE         0x008B
#define KITACA_SERVICE_CODE           0x208B
#define TOICA_SERVICE_CODE            0x1E8B
#define GATE_SERVICE_CODE             0x184B
#define PASMO_SERVICE_CODE            0x1cc8
#define SUGOCA_SERVICE_CODE           0x21c8
#define PITAPA_SERVICE_CODE           0x1b88
#define EDY_ATTRIBUTE_CODE            0x110B
#define EDY_SERVICE_CODE              0x1317
#define NANACO_ID_CODE                0x558B
#define NANACO_BALANCE_CODE           0x5597
#define WAON_SERVICE_CODE1            0x6817
#define WAON_SERVICE_ID               0x684F

int requestService(uint16_t serviceCode);
int readEncryption(uint16_t serviceCode, uint8_t blockNumber, uint8_t *buf);
void draw_JPY_yen_symbol(int x, int y, int dx, int dy);

DigitalOut back_light(p25);
ST7735S tft(ST7735S_MOSI, ST7735S_MISO, ST7735S_SCLK, ST7735S_CS, ST7735S_RS, ST7735S_RESET);
RCS620S rcs620s(RCS620S_TX, RCS620S_RX);

void printBalanceTFT(const char * name, uint32_t balance)
{
    tft.fillrect(0, 0, 159, 24, DarkCyan);
    
    tft.set_font((unsigned char*) ArialR16x17);
    tft.foreground(Yellow);
    tft.background(DarkCyan);
    tft.locate(12, 4);
    tft.printf(name);

    tft.set_font((unsigned char*) ArialR32x34);
    tft.foreground(White);
    tft.background(Navy);
    tft.locate(12+14, 40);
    char buf[10];
    sprintf(buf, "%5d", (int)balance);
    tft.printf(buf);

    draw_JPY_yen_symbol(128, 50, 14, 15);
}

void draw_JPY_yen_symbol(int x, int y, int dx, int dy)
{
    tft.rect(x, y, x + 1, y + dy, Yellow);
    tft.rect(x, y, x + dx, y + 1, Yellow);
    tft.rect(x + dx, y, x + dx + 1, y + dy, Yellow);
    tft.rect(x, y + (dy / 2), x + dx, y + (dy / 2) + 1, Yellow);
    tft.rect(x + (dx / 2), y, x + (dx / 2) + 1, y + (dy / 2), Yellow);
    tft.rect(x + dx - (dx / 4), y + dy - 1, x + dx, y + dy, Yellow);
}

int main()
{
    uint8_t idm[8];
    uint8_t attr[RCS620S_MAX_RESPONSE_LEN];
    uint8_t buf[RCS620S_MAX_RESPONSE_LEN];
    int isCaptured = 0;
    uint32_t balance = 0;

    back_light = 0;
    tft.background(Navy);
    tft.cls();
    back_light = 1;

    tft.claim(stdout);
    tft.background(Navy);
    tft.cls();
    tft.rect(0, 0, 159, 79, DarkCyan);
    tft.rect(1, 1, 158, 78, DarkCyan);
    tft.fillrect(0, 0, 159, 24, DarkCyan);
    
    tft.set_font((unsigned char*) ArialR16x17);
    tft.foreground(Yellow);
    tft.background(DarkCyan);
    tft.locate(12, 4);
    tft.printf("FeliCa reader");

    rcs620s.initDevice();
    rcs620s.timeout = COMMAND_TIMEOUT;

    while(1) {
#if 0
        // Draw rect
        tft.background(Navy);
        tft.cls();
        tft.rect(0, 0, 159, 79, DarkCyan);
        tft.rect(1, 1, 158, 78, DarkCyan);
        tft.fillrect(0, 0, 159, 24, DarkCyan);

        printBalanceTFT("Suica", 12345);
        
        tft.set_font((unsigned char*) Arial12x12);
        tft.foreground(Yellow);
        tft.locate(12+28+(7*12)+4, 46+12);
        tft.printf("yen");

        while(1);
        ThisThread::sleep_for(4000ms);
#else
        
        // サイバネ領域
        if (rcs620s.polling(CYBERNE_SYSTEM_CODE)) {
            // Suica, PASMO等の交通系ICカード
            if (requestService(PASSNET_SERVICE_CODE)) {
                readEncryption(PASSNET_SERVICE_CODE, 0, buf);
                if (memcmp(idm, buf + 1, 8) != 0) {
                    // カード変更
                    memcpy(idm, buf + 1, 8);
                    isCaptured = 1;
                }
                else {
                    // 前と同じカード
                    isCaptured = 0;
                }
            }
            if (isCaptured) {
                if (requestService(FELICA_ATTRIBUTE_CODE)) {
                    readEncryption(FELICA_ATTRIBUTE_CODE, 0, attr);
                }

                // 残高取得
                balance = attr[12+12];                  // 12 byte目
                balance = (balance << 8) + attr[12+11]; // 11 byte目

                // カード種別判定
                char card[8];
                if ((attr[12+8] & 0xF0) == 0x30) {
                    strcpy(card, "ICOCA");
                }
                else {
                    if (requestService(KITACA_SERVICE_CODE)) {
                        strcpy(card, "Kitaca");
                    }
                    else if (requestService(TOICA_SERVICE_CODE)) {
                        strcpy(card, "toica");
                    }
                    else if (requestService(SUGOCA_SERVICE_CODE)) {
                        strcpy(card, "SUGOCA");
                    }
                    else if (requestService(PITAPA_SERVICE_CODE)) {
                        strcpy(card, "PiTaPa");
                    }
                    else if (requestService(PASMO_SERVICE_CODE)) {
                        strcpy(card, "PASMO");
                    }                    
                    else {
                        strcpy(card, "Suica");
                    }
                }

                // 残高表示
                printBalanceTFT(card , balance);
            }
        }
        
        // 共通領域
        else if (rcs620s.polling(COMMON_SYSTEM_CODE)){

            // Edy
            if (requestService(EDY_ATTRIBUTE_CODE) && readEncryption(EDY_ATTRIBUTE_CODE, 0, buf)) {                    
                if (memcmp(idm, &buf[12 + 2], 8) != 0) {
                    memcpy(idm, &buf[12 + 2], 8);
                    isCaptured = 1;
                }
                else {
                    isCaptured = 0;
                }
                if (readEncryption(EDY_SERVICE_CODE, 0, buf) && isCaptured) {
                    balance = buf[12 + 0];
                    balance += (buf[12 + 1] << 8);
                    balance += (buf[12 + 2] << 8);
                    balance += (buf[12 + 3] << 8);
                    printBalanceTFT("Edy", balance);
                }
            }
            
            // nanaco
            if (requestService(NANACO_ID_CODE) && readEncryption(NANACO_ID_CODE, 0, buf)) {
                if (memcmp(idm, &buf[12], 8) != 0) {
                    memcpy(idm, &buf[12], 8);
                    isCaptured = 1;
                }
                else {
                    isCaptured = 0;
                }
                if ( readEncryption(NANACO_BALANCE_CODE, 0, buf) && isCaptured) {
                    balance = buf[12 + 0];
                    balance += (buf[12 + 1] << 8);
                    balance += (buf[12 + 2] << 8);
                    balance += (buf[12 + 3] << 8);
                    printBalanceTFT("nanaco", balance);
                }
            }

            // waon
            if (requestService(WAON_SERVICE_ID) && readEncryption(WAON_SERVICE_ID, 0, buf)) {
                if (memcmp(idm, &buf[12], 8) != 0) {
                    memcpy(idm, &buf[12], 8);
                    isCaptured = 1;
                }
                else {
                    isCaptured = 0;
                }
                if (readEncryption(WAON_SERVICE_CODE1, 0, buf) && isCaptured) {
                    balance = buf[13];
                    balance = (balance << 8) + buf[12];
                    printBalanceTFT("waon", balance);
                }
            }
        }
        rcs620s.rfOff();
        ThisThread::sleep_for(POLLING_INTERVAL);
#endif
    }
}

int requestService(uint16_t serviceCode){
    int ret;
    uint8_t buf[RCS620S_MAX_RESPONSE_LEN];
    uint8_t responseLen = 0;
    
    buf[0] = 0x02;
    memcpy(buf + 1, rcs620s.idm, 8);
    buf[9] = 0x01;
    buf[10] = (uint8_t)((serviceCode >> 0) & 0xff);
    buf[11] = (uint8_t)((serviceCode >> 8) & 0xff);
    
    ret = rcs620s.cardCommand(buf, 12, buf, &responseLen);
    
    if (!ret || (responseLen != 12) || (buf[0] != 0x03) ||
        (memcmp(buf + 1, rcs620s.idm, 8) != 0) || ((buf[10] == 0xff) && (buf[11] == 0xff))) {
        return 0;
    }
    
    return 1;
}

int readEncryption(uint16_t serviceCode, uint8_t blockNumber, uint8_t *buf){
    int ret;
    uint8_t responseLen = 0;
    
    buf[0] = 0x06;
    memcpy(buf + 1, rcs620s.idm, 8);
    buf[9] = 0x01; // サービス数
    buf[10] = (uint8_t)((serviceCode >> 0) & 0xff);
    buf[11] = (uint8_t)((serviceCode >> 8) & 0xff);
    buf[12] = 0x01; // ブロック数
    buf[13] = 0x80;
    buf[14] = blockNumber;
    
    ret = rcs620s.cardCommand(buf, 15, buf, &responseLen);
    
    if (!ret || (responseLen != 28) || (buf[0] != 0x07) ||
        (memcmp(buf + 1, rcs620s.idm, 8) != 0)) {
        return 0;
    }

    return 1;
}
