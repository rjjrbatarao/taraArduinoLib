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
 * 
 * REQUIREMENTS:
 *  - Allan Coinslot + Allan 1222 + ESP32
 *  - CB + ESP32
 */


#include "TaraLib.h"

TaraLib cash_terminal(
  PIN_COIN, 
  PIN_RELAY, 
  PIN_CHARGER, 
  PIN_LED, 
  PIN_BUZZER, 
  CHARGE_STOP, 
  CHARGE_START, 
  RELAY_LOGIC, 
  CHARGE_LOGIC, 
  LED_LOGIC, 
  BUZZER_LOGIC
);

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  cash_terminal.taraBegin("ESP32C3 Terminal");
}

void loop() {
  // put your main code here, to run repeatedly:
  cash_terminal.taraService();
}
