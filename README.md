# taraArduinoLib
Arduino library for ESP based Bluetooth Cash Terminals, for TaraKiosk
Basic and easy setup.

## Current Feature
- [x] Auto Reconnection
- [x] Brownout Recovery
- [x] Standalone or Hybrid A1222

## Connection Hybrid:

Coin slot-----------> Allan 1222 -----------> ESP32 </br>

### Wiring Hybrid A1222
```
Coin slot - Allan 1222 
white     - com 
black     - gnd 
red       - 12v 
```

```
Allan 1222 - ESP32 
no         - 23 
com        - gnd 
```

## Connection Standalone:

Coin slot-----------> ESP32 </br>

### Wiring Standalone
```
Coin slot - ESP32 
white     - 23
black     - gnd 
red       - 12V
```

### Arduino ESP32 V3.3.3
```c++
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
 */


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
 */


#include "TaraLib.h"

#define PIN_COIN 23
#define PIN_RELAY 17
#define PIN_CHARGER 25
#define PIN_LED 02
#define PIN_BUZZER 24
#define CHARGE_STOP 90
#define CHARGE_START 85
#define RELAY_LOGIC HIGH
#define CHARGE_LOGIC LOW
#define LED_LOGIC LOW
#define BUZZER_LOGIC HIGH

TaraLib cash_terminal(PIN_COIN, PIN_RELAY, PIN_CHARGER, PIN_LED, PIN_BUZZER, CHARGE_STOP, CHARGE_START, RELAY_LOGIC, CHARGE_LOGIC, LED_LOGIC, BUZZER_LOGIC);

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  cash_terminal.taraBegin("ESP32Cash Terminal");
}

void loop() {
  // put your main code here, to run repeatedly:
  cash_terminal.taraService();
  if(Serial.available() > 0){
    char c = Serial.read();
    if(c == 's'){
      cash_terminal.taraSend("DATA:{\"hello\":\"world test\"}");
    }
  }
}


```
### Requirements

<img width="768" height="432" alt="image" src="https://github.com/user-attachments/assets/6a2fbffd-c927-4593-9004-f92d237e6444" /></br>

<img width="387" height="516" alt="image" src="https://github.com/user-attachments/assets/01e3793f-2604-4dc8-a7f0-1c588263ecd6" /></br>

<img width="720" height="720" alt="image" src="https://github.com/user-attachments/assets/5db2b241-bc0a-4210-a02c-f33e35fb83c5" /></br>

<img width="447" height="447" alt="image" src="https://github.com/user-attachments/assets/8239ae4b-5cdc-4ae0-91bf-3f73cb56e571" />





