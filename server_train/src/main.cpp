#include <Arduino.h>
#include <TrainController.hpp>
// 7segment
#include <SevenSegmentLEDDriver.hpp>

/////////////////////////　グローバル変数の宣言　/////////////////////////

#define DEVICE_NAME "server1" // ここでサーバーの名前を決める。
BLEUUID SERVICE_UUID("3fd19879-3490-470c-b26d-28dce8c1dd83"); // サービスのUUIDを指定。
BLEUUID CHARACTERISTIC_UUID("5da663e5-1dd1-4080-930f-915988c9a282"); // キャラクタリスティックのUUIDを指定。

#define LED_pin 6
#define direction_pin 5
#define VOLUME 3

uint8_t velocity = 0; // 0 ~ 255で速度
bool direction = 0; // 1:前進
bool LED_status = 0; // 1:LEDを点灯

// 7segment
SevenSegmentLEDDriver SevenSegDriver;

///////////////////////// ここまで ///////////////////////

void setup() {
    ///////////////////////////　Bluetoothをつなぐ前に一度だけ行う処理を書く(編集可) //////////////////////////////////
    // 7Segment
    SevenSegDriver.init();
    Serial.begin(115200);
    pinMode(VOLUME, INPUT);
    pinMode(LED_pin,INPUT);
    pinMode(direction_pin,INPUT);

    ////////////////////////////　ここまで　////////////////////////////////////

    doInitialize(DEVICE_NAME, SERVICE_UUID, CHARACTERISTIC_UUID);
    //////////////////////  Blutooth接続中に一度だけ行う処理(編集可) ////////////////////////////
    Serial.println("connecting to client");

    //////////////////////　ここまで //////////////////////////////
}

void loop() {

    if (device_connected && !abnormal)
    {
        if (bReadyTicker)
        {
            ///////////////////////////　ここから繰り返し行う処理を書く(編集可)　//////////////////////////////


          // スイッチの状態を読む、LEDのon/off
          if(digitalRead(LED_pin)){
            long LED_gage = 0;
            while(digitalRead(LED_pin)){
             LED_gage++;
             }
            // ゲージが溜まっているか
            if(LED_gage > 100000){
              // 状態を反転する
              if (LED_status)
              {
                LED_status = 0;
              } else {
                LED_status = 1;
              }
              delay(50); // チャタリング防止
            }
          }

          // スイッチの状態を読む、進行方向
          if(digitalRead(direction_pin)){
            long direction_gage = 0;
            while(digitalRead(direction_pin)){
             direction_gage++;
             }
            // ゲージが溜まっているか
            if(direction_gage > 100000){
              // 状態を反転する
              if (direction)
              {
                direction = 0;
              } else {
                direction= 1;
              }
                delay(50); // チャタリング防止
            }
          }

            velocity = analogRead(VOLUME) * 255/4095;
            if(velocity >= 256)
            {
              velocity=255;
            }
            // 7segment
            SevenSegDriver.ShowDigits((uint8_t)velocity*9/255);
            Serial.printf("v : %d, dir : %d, LED : %d\n", velocity, direction, LED_status);
            sendData(velocity,direction,LED_status);
            ///////////////////////////  ここまで  //////////////////////////////
        }
        
    }
    
}