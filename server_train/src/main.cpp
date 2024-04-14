#include <Arduino.h>
#include <TrainController.hpp>
// 7segment
#include <SevenSegmentLEDDriver.hpp>

/////////////////////////　グローバル変数の宣言　/////////////////////////

#define DEVICE_NAME "server1"                                        // ここでサーバーの名前を決める。
BLEUUID SERVICE_UUID("3fd19879-3490-470c-b26d-28dce8c1dd83");        // サービスのUUIDを指定。
BLEUUID CHARACTERISTIC_UUID("5da663e5-1dd1-4080-930f-915988c9a282"); // キャラクタリスティックのUUIDを指定。
// 2機 3842a4a8-c709-457f-941f-df937f30fc6c
// 2機 9faf8474-2648-4935-b05c-f7e0356e9035
// 3機 0789b10d-cd5c-49f6-8583-7857f06fe548
// 3機 9c78e978-5856-46ee-b48b-231662dd27d8
// 4機 a22c7f7d-1de6-4649-9f5a-548b944b4ca3
// 4機 f4d2cd27-988d-4ed7-8f5c-ca8a4d214eb1

#define LED_pin 6       // LEDの接続PIN
#define direction_pin 5 // 方向切り替えスイッチの接続PIN
#define VOLUME 3        // ボリュームの接続PIN

uint8_t velocity = 0; // 0 ~ 255で速度
bool direction = 0;   // 1:前進
bool LED_status = 0;  // 1:LEDを点灯

// 7segment
SevenSegmentLEDDriver SevenSegDriver;

///////////////////////// ここまで ///////////////////////

void setup()
{
  ///////////////////////////　Bluetoothをつなぐ前に一度だけ行う処理を書く(編集可) //////////////////////////////////
  // 7Segment
  SevenSegDriver.init();         // 7segmentの初期化
  Serial.begin(115200);          // シリアル通信の開始
  pinMode(VOLUME, INPUT);        // ボリュームの接続PINを入力に設定
  pinMode(LED_pin, INPUT);       // LEDの接続PINを入力に設定
  pinMode(direction_pin, INPUT); // 方向切り替えスイッチの接続PINを入力に設定

  ////////////////////////////　ここまで　////////////////////////////////////

  doInitialize(DEVICE_NAME, SERVICE_UUID, CHARACTERISTIC_UUID); // BLEの初期化
  //////////////////////  Blutooth接続中に一度だけ行う処理(編集可) ////////////////////////////
  Serial.println("connecting to client"); // シリアル通信でconnecting to clientと表示

  //////////////////////　ここまで //////////////////////////////
}

void loop()
{
  if (device_connected && !abnormal) // 接続されているかつ異常がないか
  {
    if (bReadyTicker) // タイマーが準備できているか
    {
      ///////////////////////////　ここから繰り返し行う処理を書く(編集可)　//////////////////////////////

      // スイッチの状態を読む、LEDのon/off
      if (digitalRead(LED_pin))
      {
        long LED_gage = 0;
        while (digitalRead(LED_pin))
        {
          LED_gage++;
        }
        // ゲージが溜まっているか
        if (LED_gage > 100000)
        {
          // 状態を反転する
          if (LED_status)
          {
            LED_status = 0;
          }
          else
          {
            LED_status = 1;
          }
          delay(50); // チャタリング防止
        }
      }

      // スイッチの状態を読む、進行方向
      if (digitalRead(direction_pin))
      {
        long direction_gage = 0;
        while (digitalRead(direction_pin))
        {
          direction_gage++;
        }
        // ゲージが溜まっているか
        if (direction_gage > 100000)
        {
          // 状態を反転する
          if (direction)
          {
            direction = 0;
          }
          else
          {
            direction = 1;
          }
          delay(50); // チャタリング防止
        }
      }

      velocity = analogRead(VOLUME) * 255 / 4095; // ボリュームの値を読み取る
      if (velocity >= 256)                        // 256以上なら
      {
        velocity = 255; // 255にする
      }
      // 7segment
      SevenSegDriver.ShowDigits((uint8_t)velocity * 9 / 255);                         // 7segmentに速度を表示
      Serial.printf("v : %d, dir : %d, LED : %d\n", velocity, direction, LED_status); // 速度、方向、LEDの状態を表示
      sendData(velocity, direction, LED_status);                                      // 速度、方向、LEDの状態を送信
      ///////////////////////////  ここまで  //////////////////////////////
    }
  }
}