# taraArduinoLib
Arduino library for ESP based Bluetooth Cash Terminals, for TaraKiosk
Basic and easy setup.

## Current Feature
- [x] Auto Reconnection
- [x] Brownout Recovery
- [x] Standalone or Hybrid A1222

## Supported ESP chip
- ESP32 Wroom
- ESP32 C3

  
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


```
### Requirements

<img width="768" height="432" alt="image" src="https://github.com/user-attachments/assets/6a2fbffd-c927-4593-9004-f92d237e6444" /></br>

<img width="387" height="516" alt="image" src="https://github.com/user-attachments/assets/01e3793f-2604-4dc8-a7f0-1c588263ecd6" /></br>

<img width="720" height="720" alt="image" src="https://github.com/user-attachments/assets/5db2b241-bc0a-4210-a02c-f33e35fb83c5" /></br>

<img width="447" height="447" alt="image" src="https://github.com/user-attachments/assets/8239ae4b-5cdc-4ae0-91bf-3f73cb56e571" />





