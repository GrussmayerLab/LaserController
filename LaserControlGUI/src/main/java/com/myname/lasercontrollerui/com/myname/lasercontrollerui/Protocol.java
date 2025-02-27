package com.myname.lasercontrollerui;

import java.nio.ByteBuffer;
import java.util.Arrays;

public class Protocol {
	
	// Special response code we're looking for from the USB device
    private static final byte DEVICE_ID = 0x01;
    // Command Types (Enum)
    public enum CommandType {
        GET_DEVICE(0x01),      // Command for get_device
        SET_LASER(0x02),       // Command for set_laser
        SET_MOTOR(0x03);       // Command for set_motor

        private final int code;

        CommandType(int code) {
            this.code = code;
        }

        public int getCode() {
            return code;
        }
    }

    // Struct for "set_laser" command
    public static class SetLaserCommand {
        public byte chId;        // Channel ID (1 byte)
        public short val;        // 12-bit value (stored in 2 bytes)

        public SetLaserCommand(byte chId, short val) {
            this.chId = chId;
            this.val = val;
        }
    }

    // Struct for "set_motor" command
    public static class SetMotorCommand {
        public short val;        // 12-bit value (stored in 2 bytes)

        public SetMotorCommand(short val) {
            this.val = val;
        }
    }
    
 // Method to calculate xor checksum for the data
    public static byte calculateXORChecksum(byte[] data) {
        byte crc = 0;
        for (byte b : data) {
            crc ^= b;
        }
        return crc;
    }

    // Method to create the message for GET_DEVICE (no arguments)
    public static byte[] createGetDeviceMessage() {
        return createMessage(CommandType.GET_DEVICE, null, null);
    }

    // Method to create the message for SET_LASER command
    public static byte[] createSetLaserMessage(SetLaserCommand command) {
        return createMessage(CommandType.SET_LASER, command.chId, command.val);
    }

    // Method to create the message for SET_MOTOR command
    public static byte[] createSetMotorMessage(SetMotorCommand command) {
        return createMessage(CommandType.SET_MOTOR, command.val);
    }

    // Helper method to create a message with given command and arguments
    private static byte[] createMessage(CommandType type, Object... args) {
        ByteBuffer buffer;
        
        switch (type) {
            case GET_DEVICE:
                buffer = ByteBuffer.allocate(3); // Command byte + Payload length (1) 
                buffer.put((byte) type.getCode()); // Command byte
                buffer.put((byte) 3);  // No payload for GET_DEVICE
                break;

            case SET_LASER:
                byte chId = (byte) args[0];
                short laserVal = (short) args[1];
                buffer = ByteBuffer.allocate(5); // Command byte + 1 byte payload length + 1 byte chId + 2 bytes for val
                buffer.put((byte) type.getCode()); // Command byte
                buffer.put((byte) 6); // Payload length (chId + val)
                buffer.put(chId); // Channel ID (1 byte)
                buffer.putShort(laserVal); // Laser value (2 bytes)
                break;

            case SET_MOTOR:
                short motorVal = (short) args[0];
                buffer = ByteBuffer.allocate(4); // Command byte + 1 byte payload length + 2 bytes for val
                buffer.put((byte) type.getCode()); // Command byte
                buffer.put((byte) 5); // Payload length (motorVal)
                buffer.putShort(motorVal); // Motor value (2 bytes)
                break;

            default:
                throw new IllegalArgumentException("Unsupported command type");
        }

        // Create byte array for the message without CRC
        byte[] messageWithoutCRC = new byte[buffer.position()];
        buffer.flip();
        buffer.get(messageWithoutCRC);

        // Calculate XOR-based CRC (1 byte)
        byte crc = calculateXORChecksum(messageWithoutCRC);

        // Reallocate buffer with space for the CRC and End flag
        ByteBuffer finalBuffer = ByteBuffer.allocate(buffer.position() + 3); // 1 byte for CRC + 1 byte for End flag
        finalBuffer.put((byte) 0x7E); // Start flag
        finalBuffer.put(messageWithoutCRC);  // Put the message excluding CRC and End flag
        finalBuffer.put(crc); // Append 1-byte CRC
        finalBuffer.put((byte) 0x7F); // End flag

        return finalBuffer.array();
    }

    // Method to validate the CRC of a message
    public static boolean validateCRC(byte[] message) {
        if (message.length < 5) {
            return false;
        }

        byte receivedCRC = message[4];
        byte[] messageWithoutCRC = new byte[message.length - 4];
        System.arraycopy(message, 1, messageWithoutCRC, 0, 3);
        int calculatedCRC = calculateXORChecksum(messageWithoutCRC);
        return receivedCRC == calculatedCRC;
    }

    // Method to parse and print the command message
    public static boolean validateDeviceMessage(byte[] message) {
        // Find the start and stop flags (0x7E and 0x7F)
        int startFlagIndex = -1;
        int stopFlagIndex = -1;

        // Find the start flag (0x7E) and stop flag (0x7F) in the message
        for (int i = 0; i < message.length; i++) {
            if (message[i] == 0x7E) {
                startFlagIndex = i;
                break; // Exit loop once start flag is found
            }
        }

        for (int i = message.length - 1; i >= 0; i--) {
            if (message[i] == 0x7F) {
                stopFlagIndex = i;
                break; // Exit loop once stop flag is found
            }
        }
        
        // Check if both flags were found and they are in the correct order
        if (startFlagIndex == -1 || stopFlagIndex == -1 || startFlagIndex >= stopFlagIndex) {
            System.out.println("Message does not have the correct start and stop flags.");
            return false;
        }

        // Extract the core message (between start and stop flags)
        byte[] coreMessage = Arrays.copyOfRange(message, startFlagIndex + 1, stopFlagIndex);
        
        // Extract the payload length from message[1]
        byte payloadLength = message[1];
        
        // Calculate the expected size of the message
        int expectedSize = 3 + payloadLength;  // 3 is the fixed header part

        // Check if the message length matches the expected size
        if (coreMessage.length != expectedSize) {
            System.out.println("Message size is incorrect. Expected size: " + expectedSize + ", but got: " + coreMessage.length);
            return false;
        }
        
        if (validateCRC(message)) {
            byte commandByte = coreMessage[0];
            byte identifierByte = coreMessage[2];

            System.out.println("Command Byte: " + commandByte);
            System.out.println("Payload Length: " + payloadLength);

            if (commandByte == CommandType.GET_DEVICE.getCode() && identifierByte == DEVICE_ID) {
                System.out.println("GET_DEVICE command received.");
                return true;
            } else {
                System.out.println("Unknown command.");
            }
        } else {
            System.out.println("CRC validation failed. Message may be corrupted.");
        }
        return false;
    }
}
