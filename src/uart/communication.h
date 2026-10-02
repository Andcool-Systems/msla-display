#pragma once

#include "packet.h"

#include <Arduino.h>

/// @brief Init serial
void initCommunication();

/// @brief Read next packet
bool readPacket(PacketReader& pr);