#ifndef CHIP_CONFIG_H
#define CHIP_CONFIG_H

// ============================================================================
// Core Architecture Detection & Human-Readable Labels
// ============================================================================

#if defined(CONFIG_IDF_TARGET_ESP32)
    #define BOARD_IS_ESP32       1
    #define CHIP_NAME            "ESP32 (Original)"
    #define CHIP_ARCH            "Dual-Core Xtensa LX6"

#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    #define BOARD_IS_ESP32S3     1
    #define CHIP_NAME            "ESP32-S3"
    #define CHIP_ARCH            "Dual-Core Xtensa LX7"

#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    #define BOARD_IS_ESP32C3     1
    #define CHIP_NAME            "ESP32-C3"
    #define CHIP_ARCH            "Single-Core RISC-V (RV32IMC)"

#elif defined(CONFIG_IDF_TARGET_ESP32C5)
    #define BOARD_IS_ESP32C5     1
    #define CHIP_NAME            "ESP32-C5"
    #define CHIP_ARCH            "Single-Core RISC-V (32-bit)"

#elif defined(CONFIG_IDF_TARGET_ESP32C6)
    #define BOARD_IS_ESP32C6     1
    #define CHIP_NAME            "ESP32-C6"
    #define CHIP_ARCH            "Single-Core RISC-V (RV32Xpulp)"

#elif defined(CONFIG_IDF_TARGET_ESP32P4)
    #define BOARD_IS_ESP32P4     1
    #define CHIP_NAME            "ESP32-P4"
    #define CHIP_ARCH            "Dual-Core RISC-V (High Performance)"

#else
    #error "Unsupported target or unconfigured ESP32 core family."
#endif

// ============================================================================
// Feature Flag Macros
// ============================================================================

// Native USB OTG support
#if defined(BOARD_IS_ESP32S3) || defined(BOARD_IS_ESP32P4)
    #define HAS_USB_OTG          1
#endif

// All RISC-V Architecture family members
#if defined(BOARD_IS_ESP32C3) || defined(BOARD_IS_ESP32C5) || \
    defined(BOARD_IS_ESP32C6) || defined(BOARD_IS_ESP32P4)
    #define IS_RISCV_ARCH        1
#endif

// IEEE 802.15.4 (Zigbee / Thread) Support
#if defined(BOARD_IS_ESP32C5) || defined(BOARD_IS_ESP32C6)
    #define HAS_802_15_4         1
#endif

// Dual-band Wi-Fi (2.4 GHz + 5 GHz) Support
#if defined(BOARD_IS_ESP32C5)
    #define HAS_DUAL_BAND_WIFI   1
#endif

#define DEBOUNCE_COIN_DELAY 50
//#define ESP32_1222_COINSLOT
#define ESP32_COINSLOT   //<--------------- STAND ALONE
//#define DEBUG_ENABLE
#define MAX_PAYLOAD_LENGTH 64


#endif // CHIP_CONFIG_H