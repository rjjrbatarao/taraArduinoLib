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

// 1. Forward declaration of callback classes
class MyServerCallbacks;
class MyCharacteristicCallbacks;

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
  uint8_t _chargeStop;
  uint8_t _chargeStart;
  bool _logicRelay;
  bool _logicCharge;


  bool _deviceConnected = false;
  bool _oldDeviceConnected = false;
  bool _bleCmdSendFlag = false;
  bool _bleChrgSendFlag = false;

  BLEServer* _pServer = nullptr;
  BLECharacteristic* _pTxCharacteristic = nullptr;

public:
  TaraLib(uint8_t pinCoin, uint8_t pinRelay, uint8_t pinCharge, uint8_t pinLed, uint8_t chargeStop, uint8_t chargeStart, bool logicRelay, bool logicCharge);
  ~TaraLib();

  void taraBegin(String bleName);
  void taraService();
};

#endif