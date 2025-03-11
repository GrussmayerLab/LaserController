#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include "DeviceBase.h" // Ensure this header is included for MM::Device
#include "Protocol.h"
#include <sstream>
#include <cstdio>

uint8_t Protocol::calculateXORChecksum(const std::vector<uint8_t>& data)
{
    uint8_t crc = 0;
    for (uint8_t b : data) {
        crc ^= b;
    }
    return crc;
}

std::vector<uint8_t> Protocol::createGetDeviceMessage()
{
    return createMessage(CommandType::GET_DEVICE, {});
}

std::vector<uint8_t> Protocol::createSetLaserMessage(const SetLaserCommand& command)
{
    return createMessage(CommandType::SET_LASER, { command.chId, uint8_t(command.val >> 8), uint8_t(command.val & 0xFF) });
}

std::vector<uint8_t> Protocol::createSetMotorMessage(const SetMotorCommand& command)
{
    return createMessage(CommandType::SET_MOTOR, { uint8_t(command.val >> 8), uint8_t(command.val & 0xFF) });
}

bool Protocol::validateCRC(const std::vector<uint8_t>& message)
{
    if (message.size() < 5) return false;

    uint8_t receivedCRC = message[4];
    std::vector<uint8_t> messageWithoutCRC(message.begin() + 1, message.end() - 2);
    uint8_t calculatedCRC = calculateXORChecksum(messageWithoutCRC);

    return receivedCRC == calculatedCRC;
}

// Validate device message
bool Protocol::validateDeviceMessage(const std::vector<uint8_t>& message) {
    if (message.size() < 5) {
        error = "Message too short.";
        return false;
    }

    auto startFlagIt = std::find(message.begin(), message.end(), START_FLAG);
    auto stopFlagRevIt = std::find(message.rbegin(), message.rend(), STOP_FLAG);

    if (startFlagIt == message.end() || stopFlagRevIt == message.rend()) {
        error = "Message does not have the correct start and stop flags.";
        return false;
    }

    auto stopFlagIt = stopFlagRevIt.base(); // This points to *after* the STOP_FLAG

    if (startFlagIt >= stopFlagIt - 1) {
        error = "Invalid message structure.";
        return false;
    }

    // Core message: bytes between START_FLAG and STOP_FLAG (excluding both)
    std::vector<uint8_t> coreMessage(startFlagIt + 1, stopFlagIt - 1);

    // Check if coreMessage has the expected size
    if (coreMessage.size() < 3) {
        error = "Core message too short.";
        return false;
    }

    uint8_t payloadLength = coreMessage[1];
    int expectedSize = 3 + payloadLength;

    std::ostringstream coreHexStream;
    coreHexStream << "[";
    for (size_t i = 0; i < coreMessage.size(); ++i) {
        coreHexStream << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
            << static_cast<int>(coreMessage[i]);
        if (i != coreMessage.size() - 1)
            coreHexStream << " ";
    }
    coreHexStream << "]";

    std::ostringstream msgHexStream;
    msgHexStream << "[";
    for (size_t i = 0; i < message.size(); ++i) {
        msgHexStream << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
            << static_cast<int>(message[i]);
        if (i != message.size() - 1)
            msgHexStream << " ";
    }
    msgHexStream << "]";


    if (coreMessage.size() != expectedSize) {
        error = "Message size is incorrect. Expected size: " + std::to_string(expectedSize) +
            ", but got: " + std::to_string(coreMessage.size()) +
            ". CoreMessage (hex): " + coreHexStream.str() +
            ", Full Message (hex): " + msgHexStream.str();
        return false;
    }

    if (validateCRC(message)) {
        uint8_t commandByte = coreMessage[0];
        uint8_t identifierByte = coreMessage[2];

        if (commandByte == static_cast<uint8_t>(CommandType::GET_DEVICE) && identifierByte == DEVICE_ID) {
            return true;
        }
        else {
            std::ostringstream oss;
            oss << "Unknown command. Command Byte: 0x"
                << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                << static_cast<int>(commandByte)
                << ", Identifier Byte: 0x"
                << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                << static_cast<int>(identifierByte)
                << "PayloadLenght: "
                << payloadLength;

            error = oss.str();
        }
    }
    else {
        error = "CRC validation failed. Message may be corrupted.";
    }
    return false;
}

std::vector<uint8_t> Protocol::createMessage(CommandType type, const std::vector<uint8_t>& payload)
{
    std::vector<uint8_t> message;
    message.push_back(static_cast<uint8_t>(type)); // Command byte
    message.push_back(static_cast<uint8_t>(3 + payload.size())); // Payload length
    message.insert(message.end(), payload.begin(), payload.end()); // Payload
    uint8_t crc = calculateXORChecksum(message);
    message.insert(message.begin(), START_FLAG); // Start flag
    message.push_back(crc); // CRC
    message.push_back(STOP_FLAG); // End flag
    return message;
}

std::string Protocol::getError()
{
    return error;
}
