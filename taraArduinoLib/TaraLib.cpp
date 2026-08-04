#include "TaraLib.h"

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  // App Writes -> ESP32 Receives
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // ESP32 Sends -> App Receives

// Forward declaration of class if not already in header
class TaraLib;

// ==================== CALLBACKS ====================

// Server Connection Callbacks
class MyServerCallbacks : public BLEServerCallbacks {
private:
  TaraLib *_lib;

public:
  MyServerCallbacks(TaraLib *lib)
    : _lib(lib) {}

  void onConnect(BLEServer *pServer) override {
    _lib->_deviceConnected = true;
    digitalWrite(_lib->_pinRelay, _lib->_logicRelay ? HIGH : LOW);
    digitalWrite(_lib->_pinLed, HIGH);
    // Serial.println("[ESP32] Android App Connected!");
  }

  void onDisconnect(BLEServer *pServer) override {
    _lib->_deviceConnected = false;
    digitalWrite(_lib->_pinRelay, _lib->_logicRelay ? LOW : HIGH);
    digitalWrite(_lib->_pinLed, LOW);
    // Serial.println("[ESP32] Android App Disconnected!");
  }
};

// RX Characteristic Callback (Receives data sent from Android app via sendMessage())
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
private:
  TaraLib *_lib;

public:
  MyCharacteristicCallbacks(TaraLib *lib)
    : _lib(lib) {}

  void onWrite(BLECharacteristic *pCharacteristic) override {
    String rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      if (rxValue.startsWith("INFO:")) {
        rxValue.replace("INFO:", "");
        String parts[4];

        // Loop to split the string by commas
        for (int i = 0; i < 4; i++) {
          int commaIndex = rxValue.indexOf(',');
          if (commaIndex != -1) {
            parts[i] = rxValue.substring(0, commaIndex);
            rxValue = rxValue.substring(commaIndex + 1);
          } else {
            parts[i] = rxValue;
          }
        }

        // Action based on parsed data
        if (parts[1].toInt() <= _lib->_chargeStart || parts[3].equals("START_CHARGING")) {
          digitalWrite(_lib->_pinCharge, _lib->_logicCharge ? HIGH : LOW);
          if (!_lib->_bleChrgSendFlag) {
            _lib->_bleChrgSendFlag = true;
            String payload = "START_CHARGING";
            _lib->_pTxCharacteristic->setValue(payload.c_str());
            _lib->_pTxCharacteristic->notify();
          }
        }

        if (parts[1].toInt() >= _lib->_chargeStop || parts[3].equals("STOP_CHARGING")) {
          digitalWrite(_lib->_pinCharge, _lib->_logicCharge ? LOW : HIGH);
          if (_lib->_bleChrgSendFlag) {
            _lib->_bleChrgSendFlag = false;
            String payload = "STOP_CHARGING";
            _lib->_pTxCharacteristic->setValue(payload.c_str());
            _lib->_pTxCharacteristic->notify();
          }
        }

      } else if (rxValue == "PING_FROM_APP") {
        /**
          TODO: toggle gpio 2 here built in led from esp32 on or off
        */

        // Serial.println("[ESP32] Action: Ping received!");
      }
    }
  }
};

// ==================== LIBRARY IMPLEMENTATION ====================

TaraLib::TaraLib(uint8_t pinCoin, uint8_t pinRelay, uint8_t pinCharge, uint8_t pinLed, uint8_t chargeStop, uint8_t chargeStart, bool logicRelay, bool logicCharge) {
  _pinCoin = pinCoin;
  _pinRelay = pinRelay;
  _pinCharge = pinCharge;
  _pinLed = pinLed;
  _chargeStop = chargeStop;
  _chargeStart = chargeStart;
  _logicRelay = logicRelay;
  _logicCharge = logicCharge;
}

TaraLib::~TaraLib() {
}

void TaraLib::taraBegin(String bleName) {
  pinMode(_pinCoin, INPUT_PULLUP);
  pinMode(_pinRelay, OUTPUT);
  pinMode(_pinCharge, OUTPUT);
  pinMode(_pinLed, OUTPUT);
  digitalWrite(_pinRelay, _logicRelay ? LOW : HIGH);
  digitalWrite(_pinCharge, _logicCharge ? LOW : HIGH);

  // 1. Initialize BLE Device
  BLEDevice::init(bleName.c_str());

  // 2. Create the BLE Server & set connection callbacks
  _pServer = BLEDevice::createServer();
  _pServer->setCallbacks(new MyServerCallbacks(this));  // Pass instance pointer

  // 3. Create Nordic UART Service (NUS)
  BLEService *pService = _pServer->createService(SERVICE_UUID);

  // 4. Create TX Characteristic
  _pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY);

  _pTxCharacteristic->addDescriptor(new BLE2902());

  // 5. Create RX Characteristic
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCharacteristicCallbacks(this));  // Pass instance pointer

  // 6. Start Service
  pService->start();

  // 7. Configure BLE Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);

  BLEDevice::startAdvertising();
}

void TaraLib::taraService() {
  if (digitalRead(_pinCoin) == LOW) {
    if (!_bleCmdSendFlag) {
      _bleCmdSendFlag = true;
      String payload = "HIDE_LOCKSCREEN";
      _pTxCharacteristic->setValue(payload.c_str());
      _pTxCharacteristic->notify();
    }
  } else {
    if (_bleCmdSendFlag) {
      _bleCmdSendFlag = false;
      String payload = "SHOW_LOCKSCREEN";
      _pTxCharacteristic->setValue(payload.c_str());
      _pTxCharacteristic->notify();
    }
  }

  // Send periodic heartbeat when connected
  if (_deviceConnected) {
    static unsigned long lastSendTime = 0;
    if (millis() - lastSendTime > 5000) {
      lastSendTime = millis();
      String payload = "ESP32_OK:" + String(millis() / 1000) + "s";
      _pTxCharacteristic->setValue(payload.c_str());
      _pTxCharacteristic->notify();
    }
  }

  // Handle re-advertising on disconnect
  if (!_deviceConnected && _oldDeviceConnected) {
    delay(500);
    _pServer->startAdvertising();
    _oldDeviceConnected = _deviceConnected;
  }

  // Handle new connection state transition
  if (_deviceConnected && !_oldDeviceConnected) {
    _oldDeviceConnected = _deviceConnected;
  }

  if (_deviceConnected == true) {
    digitalWrite(_pinLed, HIGH);
  } else {
    static unsigned long lastBlink = 0;
    static bool blinkState = LOW;
    if (millis() - lastBlink > 500) {
      lastBlink = millis();
      digitalWrite(_pinLed, !blinkState);
    }
  }
}