#pragma once

#include "config.h"

#include <Arduino.h>
#include <vector>

class TransmitPacket {
public:
    uint8_t data[MTU];
    size_t size;
};

/// @brief Check if uart buffer is starts with sync bytes
/// @return bool
bool startsWithSync();

/// @brief Pointed packet reader
class PacketReader {
private:
    size_t pos = 0;
    size_t size = 0;

    uint32_t packetStartTime = 0;
    bool waitingPacket = false;

    bool canRead(size_t size) const {
        return size <= data.size() - pos;
    }

public:
    std::vector<uint8_t> data;

    PacketReader() = default;
    PacketReader(const uint8_t* data, size_t size) : data(data, data + size), pos(0) {}

    /// @brief Parse buffer
    bool parse();

    /// @brief read int32 from buf
    bool readInt32(int32_t& v);

    /// @brief read double from buf
    bool readDouble(double& v);

    /// @brief read u8 from buf
    bool readUInt8(uint8_t& v);

    /// @brief read u16 from buf
    bool readUInt16(uint16_t& v);

    bool readUInt32(uint32_t& v);
};

extern const uint8_t SYNC_BYTES[];

class PacketWriter {
private:
    std::vector<uint8_t> data;
    size_t pos = 0;
    size_t size = 0;

    void resize(size_t add) {
        size_t oldSize = data.size();
        data.resize(oldSize + add);
    }

public:
    void writeBytes(const void* ptr, size_t size) {
        size_t oldSize = data.size();
        data.resize(oldSize + size);

        memcpy(data.data() + oldSize, ptr, size);
    }

    /// @brief write int32 to buf
    void write(int32_t val) {
        writeBytes(&val, sizeof(val));
    }

    /// @brief write double to buf
    void write(double val) {
        writeBytes(&val, sizeof(val));
    }

    /// @brief write u8 to buf
    void write(uint8_t val) {
        writeBytes(&val, sizeof(val));
    }

    /// @brief write u16 to buf
    void write(uint16_t val) {
        writeBytes(&val, sizeof(val));
    }

    /// @brief Convert PacketWriter to a vector
    std::vector<uint8_t> getPacket() {
        return data;
    }
};