#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Ticker.h>

///////////////////////////////////////////  内部で使用するデータの初期化  ////////////////////////////////////////////

BLECharacteristic *pCharacteristic; // キャラクタリスティックのポインタを宣言。（この宣言によってグローバルに同じcharacteristicを使用することができる）

bool device_connected = false; // クライアントが接続されているか。trueで接続済み。
bool abnormal = false;         // 異常かどうか。trueで異常。

/* タイマー制御用 */
Ticker ticker;
bool bReadyTicker = false;

struct TXdata
{
    uint8_t velocity; // 速度（モーターに出力する値）
    bool direction;
    bool LED_status;
};
struct TXdata data; // TXdataにdataという名前をつける。（詳しく言うとインスタンスの作成）

// 接続・切断時コールバック
class funcServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        device_connected = true;
    }
    void onDisconnect(BLEServer *pServer)
    {
        device_connected = false;
    }
};

//  タイマー割り込み関数  //
static void kickRoutine()
{
    bReadyTicker = true;
}

/*  準備処理  */
void doPrepare(BLEService *pService, BLEUUID characteristic_uuid__)
{
    // Notify用のキャラクタリスティックを作成する
    pCharacteristic = pService->createCharacteristic(
        characteristic_uuid__,
        BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic->addDescriptor(new BLE2902());
}

/**
 * @brief BLEの初期化を行う
 *
 * @param device_name_ デバイスの名前
 * @param service_uuid_ サーバーのUUID
 * @param characteristic_uuid_ キャラクタリスティックのUUID
 */
void doInitialize(std::string device_name_, BLEUUID service_uuid_, BLEUUID characteristic_uuid_, float interval_time_ = 1)
{
    BLEDevice::init(device_name_);

    // Serverオブジェクトを作成してコールバックを設定する
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new funcServerCallbacks());

    // Serviceオブジェクトを作成して準備処理を実行する
    BLEService *pService = pServer->createService(service_uuid_);
    doPrepare(pService, characteristic_uuid_);

    // サービスを開始して、SERVICE_UUIDでアドバタイジングを開始する
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(service_uuid_);
    pAdvertising->start();
    // タイマー割り込みを開始する
    ticker.attach(interval_time_, kickRoutine);
}

/**
 * @brief 送信するデータを設定する
 *
 * @param velocity_ 速度
 * @param direction_ 方向
 * @param LED_status_ LEDの状態
 */
void sendData(uint8_t velocity_, bool direction_, bool LED_status_)
{
    // 構造体に値を設定して送信する
    data.velocity = velocity_;
    data.direction = direction_;
    data.LED_status = LED_status_;
    pCharacteristic->setValue((uint8_t *)&data, sizeof(data));
    pCharacteristic->notify();
}