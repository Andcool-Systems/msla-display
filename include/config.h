/// Timeout (ms) after which a packet that was not completely received is
/// considered invalid
#define COMMUNICATION_TIMEOUT 100

/// Baud rate used for orchestrator connectivity
#define ORCHESTRATOR_SERIAL_BAUD_RATE 115200

/// Use hardware serial for orchestrator connectivity
// #define ORCHESTRATOR_HARDWARE_SERIAL

#ifdef ORCHESTRATOR_HARDWARE_SERIAL
/// Physical serial ID
#define PHYSICAL_SERIAL 1

#define ORCHESTRATOR_RX_PIN 26
#define ORCHESTRATOR_TX_PIN 25
#endif

#define CTP_SDA_PIN 32
#define CTP_SCL_PIN 25
#define CTP_RST_PIN 33
#define CTP_INT_PIN 4

#define TFT_BCKL 21
#define BUZZER_PIN 23
#define SD_CS_PIN 22

/// ----------------------------- COLORS ------------------------------
#define TFT_BG_COLOR 0x0000
#define TFT_CARD_COLOR 0x1082
#define TFT_EL_COLOR 0x2105
#define TFT_ACT_COLOR 0x0576