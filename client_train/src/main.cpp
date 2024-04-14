#include <Arduino.h>
#include <TrainMD.hpp>

#define LED_forward 8  // 前進時LEDの接続PIN
#define LED_backward 7 // 後進時LEDの接続PIN
#define LED_always 6   // 常時点灯LEDの接続PIN
#define IN1 10         // モータードライバーのIN1
#define IN2 9          // モータードライバーのIN2

BLEUUID SERVICE_UUID("3fd19879-3490-470c-b26d-28dce8c1dd83");        // サービスのUUIDを指定。
BLEUUID CHARACTERISTIC_UUID("5da663e5-1dd1-4080-930f-915988c9a282"); // キャラクタリスティックのUUIDを指定。
// 2機 3842a4a8-c709-457f-941f-df937f30fc6c
// 2機 9faf8474-2648-4935-b05c-f7e0356e9035
// 3機 0789b10d-cd5c-49f6-8583-7857f06fe548
// 3機 9c78e978-5856-46ee-b48b-231662dd27d8
// 4機 a22c7f7d-1de6-4649-9f5a-548b944b4ca3
// 4機 f4d2cd27-988d-4ed7-8f5c-ca8a4d214eb1

void setup()
{
  Serial.begin(115200);               //  シリアル通信の開始
  Serial.println("Hello I'm client"); //  シリアル通信でHello I'm clientと表示
  //////////////////////////////　開始時一度だけ行われる処理(編集可) /////////////////////////

  pinMode(LED_forward, OUTPUT);  // LEDの接続PINを出力に設定
  pinMode(LED_backward, OUTPUT); // LEDの接続PINを出力に設定
  pinMode(LED_always, OUTPUT);   // LEDの接続PINを出力に設定

  ledcSetup(1, 2000, 8); // PWMの設定
  ledcAttachPin(IN1, 1); // ピンの設定

  ledcSetup(2, 2000, 8); // PWMの設定
  ledcAttachPin(IN2, 2); // ピンの設定

  //////////////////////////////　ここまで　////////////////////////////
  if (doInitialize(SERVICE_UUID, CHARACTERISTIC_UUID)) //  BLEの初期化
  {
    Serial.println("Connected to the TrainController"); //  シリアル通信でConnected to the TrainControllerと表示
  }
}

void loop()
{
  // 接続状態なら
  if (deviceConnected)
  {
    // 値が更新されていれば
    if (enableNewData)
    {
      ///////////////////////////////////////　受信が行われたあと行う処理(編集可)　///////////////////////////////////
      Serial.printf("velocity : %d\n", data.velocity);   // 速度を表示
      Serial.printf("direction: %d\n", data.direction);  // 方向を表示
      Serial.printf("LED      : %d\n", data.LED_status); // LEDの状態を表示

#if 0
      if(data.direction = 1){
      digitalWrite(LED_forward,data.LED_status);
      digitalWrite(LED_backward,LOW);
      digitalWrite(LED_always,HIGH);
      ledcWrite(1,data.velocity);
      Serial.printf("A \n");
      }
      if(data.direction = 0){
      digitalWrite(LED_forward,LOW);
      digitalWrite(LED_backward,data.LED_status);
      digitalWrite(LED_always,HIGH);
      ledcWrite(2,data.velocity);
      Serial.printf("B \n");
      }
      else{
      digitalWrite(LED_forward,LOW);
      digitalWrite(LED_backward,LOW);
      digitalWrite(LED_always,LOW);
      ledcWrite(2,0);
      Serial.printf("C \n");
      }
#endif

#if 1
      if (data.direction) // 方向が1なら
      {
        ledcWrite(1, data.velocity); // PWMの値を設定
        ledcWrite(2, 0);             // PWMの値を設定
        Serial.println("Forword");   // シリアル通信でForwordと表示
      }
      else // 方向が0なら
      {
        ledcWrite(2, data.velocity); // PWMの値を設定
        ledcWrite(1, 0);             // PWMの値を設定
        Serial.println("Backword");  // シリアル通信でBackwordと表示
      }

      if (data.LED_status) // LEDの状態が1なら
      {
        if (data.direction) // 方向が1なら
        {
          digitalWrite(LED_forward, HIGH); // 前照灯を点灯
          digitalWrite(LED_backward, LOW); // 後照灯を消灯
          digitalWrite(LED_always, HIGH);  // 常時点灯LEDを点灯
        }
        else // 方向が0なら
        {
          digitalWrite(LED_forward, LOW);   // 前照灯を消灯
          digitalWrite(LED_backward, HIGH); // 後照灯を点灯
          digitalWrite(LED_always, HIGH);   // 常時点灯LEDを点灯
        }
      }
      else // LEDの状態が0なら
      {
        digitalWrite(LED_forward, LOW);  // 前照灯を消灯
        digitalWrite(LED_backward, LOW); // 後照灯を消灯
        digitalWrite(LED_always, LOW);   // 常時点灯LEDを消灯
      }
#endif

      /////////////////////////////////////// ここまで ////////////////////////////////////////////

      enableNewData = false;
    }
    // 接続状態でなければ、再接続する。
  }
  else if (doScan) // スキャン指示があれば
  {
    BLEDevice::getScan()->start(0); // スキャンを開始する
  }
}