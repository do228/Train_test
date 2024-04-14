#include <Arduino.h>
#include <TrainMD.hpp>

#define LED_forward 8
#define LED_backward 7
#define LED_always 6
#define IN1 10
#define IN2 9

BLEUUID SERVICE_UUID("3fd19879-3490-470c-b26d-28dce8c1dd83");
BLEUUID CHARACTERISTIC_UUID("5da663e5-1dd1-4080-930f-915988c9a282");
// 2機 3842a4a8-c709-457f-941f-df937f30fc6c
// 2機 9faf8474-2648-4935-b05c-f7e0356e9035
// 3機 0789b10d-cd5c-49f6-8583-7857f06fe548
// 3機 9c78e978-5856-46ee-b48b-231662dd27d8
// 4機 a22c7f7d-1de6-4649-9f5a-548b944b4ca3
// 4機 f4d2cd27-988d-4ed7-8f5c-ca8a4d214eb1

void setup()
{
  Serial.begin(115200);
  Serial.println("Hello I'm client");
  //////////////////////////////　開始時一度だけ行われる処理(編集可) /////////////////////////

  pinMode(LED_forward, OUTPUT);
  pinMode(LED_backward, OUTPUT);
  pinMode(LED_always, OUTPUT);

  ledcSetup(1, 2000, 8);
  ledcAttachPin(IN1, 1);

  ledcSetup(2, 2000, 8);
  ledcAttachPin(IN2, 2);

  //////////////////////////////　ここまで　////////////////////////////
  if (doInitialize(SERVICE_UUID, CHARACTERISTIC_UUID))
  {
    Serial.println("Connected to the TrainController");
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
      Serial.printf("velocity : %d\n", data.velocity);
      Serial.printf("direction: %d\n", data.direction);
      Serial.printf("LED      : %d\n", data.LED_status);

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
      if (data.direction)
      {
        ledcWrite(1, data.velocity);
        ledcWrite(2, 0);
        Serial.println("Forword");
      }
      else
      {
        ledcWrite(2, data.velocity);
        ledcWrite(1, 0);
        Serial.println("Backword");
      }

      if (data.LED_status)
      {
        if (data.direction)
        {
          digitalWrite(LED_forward, HIGH);
          digitalWrite(LED_backward, LOW);
          digitalWrite(LED_always, HIGH);
        }
        else
        {
          digitalWrite(LED_forward, LOW);
          digitalWrite(LED_backward, HIGH);
          digitalWrite(LED_always, HIGH);
        }
      }
      else
      {
        digitalWrite(LED_forward, LOW);
        digitalWrite(LED_backward, LOW);
        digitalWrite(LED_always, LOW);
      }
#endif

      /////////////////////////////////////// ここまで ////////////////////////////////////////////

      enableNewData = false;
    }
    // 接続状態でなければ、再接続する。
  }
  else if (doScan)
  {
    BLEDevice::getScan()->start(0);
  }
}