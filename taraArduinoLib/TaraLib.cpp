#include "TaraLib.h"

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  // App Writes -> ESP32 Receives
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // ESP32 Sends -> App Receives
#define HEARTBEAT_INTERVAL 10000
#define MAX_PAYLOAD_SIZE 64  // in reality its only 61 max bytes
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
#ifdef ESP32_1222_COINSLOT
    digitalWrite(_lib->_pinRelay, _lib->_logicRelay ? HIGH : LOW);
#endif
    digitalWrite(_lib->_pinLed, _lib->_logicLed ? HIGH : LOW);
    // Serial.println("[ESP32] Android App Connected!");
  }

  void onDisconnect(BLEServer *pServer) override {
    _lib->_deviceConnected = false;
#ifdef ESP32_1222_COINSLOT
    digitalWrite(_lib->_pinRelay, _lib->_logicRelay ? LOW : HIGH);
#endif
    digitalWrite(_lib->_pinLed, _lib->_logicLed ? LOW : HIGH);
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
        String parts[5];

        // Loop to split the string by commas
        for (int i = 0; i < 5; i++) {
          int commaIndex = rxValue.indexOf(',');
          if (commaIndex != -1) {
            parts[i] = rxValue.substring(0, commaIndex);
            rxValue = rxValue.substring(commaIndex + 1);
          } else {
            parts[i] = rxValue;
          }
#ifdef DEBUG_ENABLE
          Serial.println(parts[i]);
#endif
        }
        //Serial.println(parts[1].toInt());
        //Serial.println(_lib->_chargeStart);
        //Serial.println(parts[3]);  // charging state in android memory
        //Serial.println(parts[4]);  // tells where the webview is if its on lockscreen: locked or menu: unlocked
        // Action based on parsed data
        if (parts[1].toInt() <= _lib->_chargeStart || parts[3].equals("START_CHARGING")) {
          digitalWrite(_lib->_pinCharge, _lib->_logicCharge ? HIGH : LOW);
          if (!_lib->_bleChrgSendFlag) {
            _lib->_bleChrgSendFlag = true;
            String payload = "START_CHARGING";
            _lib->taraSend(payload);
          }
        }

        if (parts[1].toInt() >= _lib->_chargeStop || parts[3].equals("STOP_CHARGING")) {
          digitalWrite(_lib->_pinCharge, _lib->_logicCharge ? LOW : HIGH);
          if (_lib->_bleChrgSendFlag) {
            _lib->_bleChrgSendFlag = false;
            String payload = "STOP_CHARGING";
            _lib->taraSend(payload);
          }
        }
#ifdef ESP32_COINSLOT
        if (parts[3].equals("CLEAR_COIN")) {
          _lib->setCoin(0);
        }
#endif
      } else if (rxValue.startsWith("DATA:")) {
#ifdef ESP32_COINSLOT
        /**
          TODO: toggle gpio 2 here built in led from esp32 on or off
        */
        rxValue.replace("DATA:", "");
        //#ifdef DEBUG_ENABLE
        //Serial.println(rxValue);
        if (rxValue.equals("ON")) {
          digitalWrite(_lib->_pinRelay, _lib->_logicRelay ? HIGH : LOW);
          _lib->_isRelayOn = true;
        } else if (rxValue.equals("OFF")) {
          digitalWrite(_lib->_pinRelay, _lib->_logicRelay ? LOW : HIGH);
          _lib->_isRelayOn = false;
        }
//#endif
// Serial.println("[ESP32] Action: Ping received!");
#endif
      }
    }
  }
};

// ==================== LIBRARY IMPLEMENTATION ====================

TaraLib::TaraLib(uint8_t pinCoin, uint8_t pinRelay, uint8_t pinCharge, uint8_t pinLed, uint8_t pinBuzzer, uint8_t chargeStop, uint8_t chargeStart, bool logicRelay, bool logicCharge, bool logicLed, bool logicBuzzer) {
  _pinCoin = pinCoin;
  _pinRelay = pinRelay;
  _pinCharge = pinCharge;
  _pinLed = pinLed;
  _pinBuzzer = pinBuzzer;
  _chargeStop = chargeStop;
  _chargeStart = chargeStart;
  _logicRelay = logicRelay;
  _logicCharge = logicCharge;
  _logicLed = logicLed;
  _logicBuzzer = logicBuzzer;
  _isRelayOn = false;
}

TaraLib::~TaraLib() {
}

void TaraLib::taraBegin(String bleName) {

  pinMode(_pinCoin, INPUT_PULLUP);
  pinMode(_pinRelay, OUTPUT);
  pinMode(_pinCharge, OUTPUT);
  pinMode(_pinLed, _logicLed ? LOW : HIGH);
#ifdef ESP32_COINSLOT
  digitalWrite(_pinRelay, _logicRelay ? LOW : HIGH);
#endif
#ifdef ESP32_1222_COINSLOT
  digitalWrite(_pinRelay, _logicRelay ? HIGH : LOW);
#endif
  digitalWrite(_pinCharge, _logicCharge ? LOW : HIGH);
  digitalWrite(_pinBuzzer, _logicBuzzer ? LOW : HIGH);


  // 1. Initialize BLE Device
  BLEDevice::init(bleName.c_str());
  BLEDevice::setMTU(MAX_PAYLOAD_SIZE);
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


#ifdef ESP32_COINSLOT
  attachInterrupt(
    digitalPinToInterrupt(_pinCoin),
    [this]() {
      this->handleInterrupt();
    },
    FALLING);
#endif
}

/**
* Blindly send any string data
*/
void TaraLib::taraSend(String data) {
  if ((data.length() - 3) <= MAX_PAYLOAD_SIZE) {
    _pTxCharacteristic->setValue(data.c_str());
    _pTxCharacteristic->notify();
  } else {
    // length too long
  }
}

void TaraLib::taraService() {
#ifdef ESP32_1222_COINSLOT
  if (digitalRead(_pinCoin) == LOW) {
    if (!_bleCmdSendFlag) {
      _bleCmdSendFlag = true;
      String payload = "HIDE_LOCKSCREEN";
      taraSend(payload);
    }
  } else {
    if (_bleCmdSendFlag) {
      _bleCmdSendFlag = false;
      String payload = "SHOW_LOCKSCREEN";
      taraSend(payload);
    }
  }
#endif

  // Send periodic heartbeat when connected
  if (_deviceConnected == true && _isRelayOn == false) {
    static unsigned long lastSendTime = 0;
    if (millis() - lastSendTime > HEARTBEAT_INTERVAL) {
      lastSendTime = millis();
      String payload = "ESP32_OK:" + String(millis() / 1000) + "s";
      taraSend(payload);
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
#ifdef ESP32_COINSLOT
  if (_coinCounting) {
    unsigned long curr_t = millis();
    if ((curr_t - _lastDebounceTime) < 250)  // low 20ms, med 50ms, high 100ms
    {
      return;
    }
    _coinCounting = false;

    if (_coinCount > 0) {
      /**
      Send to bluetooth
    */
      String payload = "DATA:" + String(_coinCount);
      taraSend(payload);
    }
  }
  noInterrupts();
  _coinCount = 0;
  _coinCounting = true;
  interrupts();
#endif
}

#ifdef ESP32_COINSLOT
uint32_t TaraLib::getCoin() {
  return _coinCount;
}

void TaraLib::setCoin(uint32_t coin) {
  _coinCount = coin;
}
#endif