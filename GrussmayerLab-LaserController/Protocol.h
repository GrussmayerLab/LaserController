#pragma once

#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>

class Protocol {
public:
    // Special response code from USB device
    static constexpr uint8_t DEVICE_ID = 0x01;
    static constexpr uint8_t START_FLAG = 0x7E;
    static constexpr uint8_t STOP_FLAG = 0x7F;

    // Command Types (Enum)
    enum class CommandType : uint8_t {
        GET_DEVICE = 0x01,
        SET_SETTINGS = 0x02,
        GET_DATA = 0x03
    };

    // Struct for "set_settings" command
    struct SetSettingsCommand {
        uint8_t avarage;   // Avarage
        float exposure;    // Exposure microseconds
    };

    // Create messages
    std::vector<uint8_t> createGetDeviceMessage();

    std::vector<uint8_t> createSetSettingsMessage(const SetSettingsCommand& command);

    bool validateCRC(const std::vector<uint8_t>& message);

    bool validateDeviceMessage(const std::vector<uint8_t>& message);

    bool validateDataMessage(const std::vector<uint8_t>& message);

    std::string getError();

private:
    std::vector<uint8_t> createMessage(CommandType type, const std::vector<uint8_t>& payload);
    std::string error;
    // Method to calculate XOR checksum
    uint8_t calculateXORChecksum(const std::vector<uint8_t>& data);
};