#include <Arduino.h>
#include <BLEDevice.h>

BLEUUID service_uuid;
BLEUUID characteristic_uuid;

BLERemoteCharacteristic* pRemoteCharacteristic; // 受信用キャラクタリスティック
BLEAdvertisedDevice* targetDevice;      // 目的のBLEデバイス
bool  doConnect = false;                // 接続指示
bool  doScan = false;                   // スキャン指示
bool  deviceConnected = false;          // デバイスの接続状態
bool  enableNewData = false;        // 受信データが有効

/* 受信したデータ */
struct RXData {
    uint8_t  velocity;
    bool direction;
    bool LED_status;
};
struct RXData  data; // 受信データの構造体

// 接続・切断時コールバック
class funcClientCallbacks: public BLEClientCallbacks {
    void onConnect(BLEClient* pClient) {
    };
    void onDisconnect(BLEClient* pClient) {
        deviceConnected = false;
    }
};

// アドバタイジング受信時コールバック
class advertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        Serial.print("Advertised Device found: ");
        Serial.println(advertisedDevice.toString().c_str());

        // 目的のBLEデバイスならスキャンを停止して接続準備をする
        if (advertisedDevice.haveServiceUUID()) {
            BLEUUID service = advertisedDevice.getServiceUUID();
            Serial.print("Have ServiceUUI: "); Serial.println(service.toString().c_str());
            if (service.equals(service_uuid)) {
                BLEDevice::getScan()->stop();
                targetDevice = new BLEAdvertisedDevice(advertisedDevice);
                doConnect = doScan = true;
            }
        }
    }
};

// Notify時のコールバック関数
static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                uint8_t* pData, size_t length, bool isNotify) {
    // 受信メッセージを構造体(data)へ送る。
    memcpy(&data, pData, length);
    enableNewData = true;
}

/*  準備処理  */
bool doPrepare() {
    // クライアントオブジェクトを作成してコールバックを設定する
    BLEClient* pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new funcClientCallbacks());
    Serial.println(" - Created client.");

    // リモートBLEサーバーと接続して
    pClient->connect(targetDevice);
    Serial.println(" - Connected to server.");

    // サービスへの参照を取得する
    BLERemoteService* pRemoteService = pClient->getService(service_uuid);
    if (pRemoteService == nullptr) {
        Serial.print("Failed to find serviceUUID: ");
        Serial.println(service_uuid.toString().c_str());
        pClient->disconnect();
        return false;
    }
    Serial.println(" - Found target service.");

    // キャラクタリスティックへの参照を取得して
    pRemoteCharacteristic = pRemoteService->getCharacteristic(characteristic_uuid);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find characteristicUUID: ");
      Serial.println(characteristic_uuid.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found characteristic UUID");

    // Notifyのコールバック関数を割り当てる
    if (pRemoteCharacteristic->canNotify()) {
        pRemoteCharacteristic->registerForNotify(notifyCallback);
        Serial.println(" - Registered notify callback function.");
    }

    deviceConnected = true;
    return true;
}

/*  初期化処理  */
bool doInitialize(BLEUUID service_uuid_, BLEUUID characteristic_uuid_) {
    service_uuid = service_uuid_;
    characteristic_uuid = characteristic_uuid_;

    BLEDevice::init("");

    // Scanオブジェクトを取得してコールバックを設定する
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new advertisedDeviceCallbacks());
    // アクティブスキャンで10秒間スキャンする
    pBLEScan->setActiveScan(true);
    pBLEScan->start(10);
    while (!doConnect){}
    return doPrepare();
}