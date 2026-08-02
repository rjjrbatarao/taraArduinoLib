# taraArduinoLib
Arduino library for ESP based Bluetooth Cash Terminals.
Basic and easy setup.

## Connection:

Coin slot-----------> Allan 1222 -----------> ESP32 </br>

### Basic Setup
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
### Arduino ESP32 V3.3.3
```c++
#include "TaraLib.h"

#define PIN_COIN 23
#define PIN_RELAY 17
#define PIN_CHARGER 25
#define CHARGE_STOP 99
#define CHARGE_START 85
#define RELAY_LOGIC LOW
#define CHARGE_LOGIC LOW

TaraLib cash_terminal(PIN_COIN, PIN_RELAY, PIN_CHARGER, CHARGE_STOP, CHARGE_START, RELAY_LOGIC, CHARGE_LOGIC);

void setup() {
  // put your setup code here, to run once:
  cash_terminal.taraBegin("ESP32Cash Terminal");
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




