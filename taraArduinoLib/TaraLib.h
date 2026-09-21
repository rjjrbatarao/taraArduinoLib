/**
 * @file TaraLib.h / TaraLib.cpp
 * @brief ESP32 BLE Library for handling Android App communications, charging logic, and signal triggers.
 * 
 * @author  [rjjrbatarao / TaraKiosk]
 * @date    August 2026
 * @version 1.0.0
 * 
 * @details
 * FEATURES & CAPABILITIES:
 *  - Nordic UART Service (NUS) integration over Bluetooth Low Energy (BLE).
 *  - Real-time bi-directional communication between ESP32 and custom Android app.
 *  - Automated battery charging control with configurable start/stop thresholds.
 *  - Lockscreen UI trigger control based on physical hardware input (Coin Pin).
 *  - Automatic re-advertising and reconnection handling on BLE disconnects.
 *  - Periodic status/heartbeat notifications sent to connected client.
 */

#ifndef TARA_LIB_BLE_H
#define TARA_LIB_BLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <FunctionalInterrupt.h>  // Required header
#include "config.h"

// 1. Forward declaration of callback classes
class MyServerCallbacks;
class MyCharacteristicCallbacks;

struct Message {
  char payload[MAX_PAYLOAD_LENGTH];
};

class TaraLib {
  // 2. Grant friend access to the callbacks
  friend class MyServerCallbacks;
  friend class MyCharacteristicCallbacks;

private:
  // Member variables
  uint8_t _pinCoin;
  uint8_t _pinRelay;
  uint8_t _pinCharge;
  uint8_t _pinLed;
  uint8_t _pinBuzzer;
  uint8_t _chargeStop;
  uint8_t _chargeStart;
  bool _logicRelay;
  bool _logicCharge;
  bool _logicLed;
  bool _logicBuzzer;
  bool _isRelayOn;

  bool _deviceConnected = false;
  bool _oldDeviceConnected = false;
  bool _bleCmdSendFlag = false;
  bool _bleChrgSendFlag = false;

  QueueHandle_t _msgQueue;
  
  BLEServer* _pServer = nullptr;
  BLECharacteristic* _pTxCharacteristic = nullptr;
  void taraSend(String data);

#ifdef ESP32_COINSLOT


  volatile uint32_t _coinCount = 0;
  volatile unsigned long _lastDebounceTime;
  volatile unsigned long _lastPulseTime = 0;
  portMUX_TYPE _synch = portMUX_INITIALIZER_UNLOCKED;
  bool _coinCounting = true;

  void IRAM_ATTR handleInterrupt() {
    unsigned long _currentTime = millis();
    if(_coinCounting && (_currentTime - _lastPulseTime > DEBOUNCE_COIN_DELAY)){
      portENTER_CRITICAL(&_synch);
      _lastPulseTime = _currentTime;
      _coinCount++;
      _lastDebounceTime = millis();
      portEXIT_CRITICAL(&_synch);
    }
  }

  uint32_t getCoin();
  void setCoin(uint32_t coin);

#endif

public:
  TaraLib(uint8_t pinCoin, uint8_t pinRelay, uint8_t pinCharge, uint8_t pinLed, uint8_t pinBuzzer, uint8_t chargeStop, uint8_t chargeStart, bool logicRelay, bool logicCharge, bool logicLed, bool logicBuzzer);
  ~TaraLib();


  void taraBegin(String bleName);
  void taraService();
};

#endif