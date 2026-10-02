#include "communication.h"

#include "config.h"
#include "uart.h"

#include <Arduino.h>

void initCommunication() {
    UARTBegin();
}

bool readPacket(PacketReader& pr) {
    uint8_t buffer[64];

    int available = OrchestratorSerial.available();

    if (available > 0) {
        size_t count = min((int)sizeof(buffer), available);
        size_t read = OrchestratorSerial.read(buffer, count);
        UART_BUFFER.insert(UART_BUFFER.end(), buffer, buffer + read);
    }

    return pr.parse();
}