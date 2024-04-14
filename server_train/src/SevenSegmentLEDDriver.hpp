/**
 * @file SevenSegmentLEDDriver.hpp
 * @author GN10
 * @brief 7セグメントLEDドライバーのクラス
 * @version 0.1
 * @date 2024-04-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <Arduino.h>
#include <SPI.h>

class SevenSegmentLEDDriver
{
private:
    static const uint8_t sck = 8;    // 7segmentのSCK
    static const uint8_t latch = 20; // 7segmentのRCK
    static const uint8_t sdi = 10;   // 7segmentのDIN
    const byte digits[10] = {
        0b11111100, // 0
        0b01100000, // 1
        0b11011010, // 2
        0b11110010, // 3
        0b01100110, // 4
        0b10110110, // 5
        0b10111110, // 6
        0b11100000, // 7
        0b11111110, // 8
        0b11110110, // 9
    };

public:
    void init()
    {
        pinMode(latch, OUTPUT);    // 7segmentのRCKを出力に設定
        pinMode(sck, OUTPUT);      // 7segmentのSCKを出力に設定
        pinMode(sdi, OUTPUT);      // 7segmentのDINを出力に設定
        SPI.begin();               // SPI通信の開始
        SPI.setBitOrder(LSBFIRST); // ビットの順番を設定
        SPI.setDataMode(0);        // データモードを設定
    }
    /**
     * @brief 7セグメントLEDに数字を表示する
     *
     * @param number 表示する数字
     */
    void ShowDigits(uint8_t number)
    {
        try
        {
            digitalWrite(latch, LOW);     // 7segmentのRCKをLOWにする
            SPI.transfer(digits[number]); // 7segmentに数字を表示
            digitalWrite(latch, HIGH);    // 7segmentのRCKをHIGHにする
        }
        catch (const std::exception &e) // 例外処理
        {
        }
    }
    /**
     * @brief 7セグメントLEDにセグメントを表示する
     *
     * @param segment 表示するセグメント
     */
    void ShowSegment(byte segment)
    {
        digitalWrite(latch, LOW);
        SPI.transfer((byte)segment);
        digitalWrite(latch, HIGH);
    }
};