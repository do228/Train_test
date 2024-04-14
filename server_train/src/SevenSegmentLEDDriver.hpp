#include <Arduino.h>
#include <SPI.h>

class SevenSegmentLEDDriver
{
private:
    static const uint8_t sck = 8;
    static const uint8_t latch = 20;
    static const uint8_t sdi = 10;
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
        pinMode(latch, OUTPUT);
        pinMode(sck, OUTPUT);
        pinMode(sdi, OUTPUT);
        SPI.begin();
        SPI.setBitOrder(LSBFIRST);
        SPI.setDataMode(0);
    }

    void ShowDigits(uint8_t number)
    {
        try
        {
            digitalWrite(latch, LOW);
            SPI.transfer(digits[number]);
            digitalWrite(latch, HIGH);
        }
        catch (const std::exception &e)
        {
        }
    }

    void ShowSegment(byte segment)
    {
        digitalWrite(latch, LOW);
        SPI.transfer((byte)segment);
        digitalWrite(latch, HIGH);
    }
};