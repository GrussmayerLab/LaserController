package com.myname.lasercontrollerui;

import com.fazecast.jSerialComm.*;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.Comparator;

public class USBManager {
	
    // Instance variables for the serial port settings
    private int baudRate;
    private int numDataBits;
    private int numStopBits;
    private int parity;
    private SerialPort devicePort;
    // Constructor to set the serial port communication settings
    public USBManager(int baudRate, int numDataBits, int numStopBits, int parity) {
        this.baudRate = baudRate;
        this.numDataBits = numDataBits;
        this.numStopBits = numStopBits;
        this.parity = parity;
    }

    // Method to scan available USB devices and check for a response
    public String scanAndIdentifyDevice() {
        SerialPort[] availablePorts = SerialPort.getCommPorts();

        if (availablePorts.length == 0) {
            System.out.println("No USB serial devices found.");
            return "Not Found";
        }

        System.out.println("Scanning available USB devices...");

        // Sort available ports by their COM port number (lowest first)
        Arrays.sort(availablePorts, Comparator.comparingInt(port -> {
            String portName = port.getSystemPortName();
            int comPortNumber = Integer.parseInt(portName.replaceAll("[^0-9]", "")); // Extract COM port number
            return comPortNumber;
        }));

        // Iterate over all available serial ports, starting from the lowest COM port
        for (SerialPort port : availablePorts) {
            System.out.println("Scanning port: " + port.getSystemPortName());

            // Try communicating with the device
            if (sendTestAndReceiveResponse(port)) {
                devicePort = port;
                System.out.println("Device found on port: " + port.getSystemPortName());
                return port.getSystemPortName();
            }
        }
        return "Not Found";
    }

    // Method to send a test message and read the response
    private boolean sendTestAndReceiveResponse(SerialPort port) {
        boolean deviceFound = false;

        // Open the port
        if (port.openPort()) {
            try {
                // Set port parameters based on the instance's settings
                port.setBaudRate(baudRate);
                port.setNumDataBits(numDataBits);
                port.setNumStopBits(numStopBits);
                port.setParity(parity);
                port.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 10, 0);
                // Get the input/output streams for reading/writing
                InputStream inputStream = port.getInputStream();
                OutputStream outputStream = port.getOutputStream();

                // Send get_dive
                byte[] getDeviceMessage = Protocol.createGetDeviceMessage();
                System.out.print("Message: ");
                for (byte b : getDeviceMessage) {
                    System.out.printf("%02X ", b); // Print each byte as two-digit hexadecimal
                }
                System.out.println();
                Thread.sleep(50); // Small delay to ensure data is sent
                outputStream.flush();
                outputStream.write(getDeviceMessage);
                outputStream.flush();
                Thread.sleep(20); // Small delay to ensure data is sent
                
                // Read response from the device
                byte[] buffer = new byte[256]; // Buffer to store the response
                try {
                	int bytesRead = inputStream.read(buffer, 0, getDeviceMessage.length + 1);
                    if (bytesRead == -1) {
                        System.out.println("No data received.");
                        return false;
                    }
                    
                    // Process the data if bytes are read
                    System.out.println("Bytes received: " + bytesRead);
                    byte[] receivedData = Arrays.copyOf(buffer, bytesRead); // Trim to the actual data size
                    System.out.println("Received Data: ");
                    for (byte b : receivedData) {
                        System.out.printf("%02X ", b); // Print each byte as two-digit hexadecimal
                    }
                    System.out.println();
                } catch (IOException e) {
                    System.err.println("Error reading from input stream: " + e.getMessage());
                    return false;
                }
                
                boolean trueDevice = Protocol.validateDeviceMessage(buffer);

                // Check if the response matches the expected response
                if (trueDevice) {
                    deviceFound = true; // We found the device
                } else {
                    System.out.println("No valid response from " + port.getSystemPortName());
                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                // Close the port after the operation
	        	 try {
	                 // Add a small delay after closing the port to ensure Bluetooth connection is fully released
	                 port.closePort();
	                 Thread.sleep(500); // Small delay to ensure the port is released by the system
	             } catch (InterruptedException ie) {
	                 System.err.println("Error waiting for port release: " + ie.getMessage());
	           }
            }
        } else {
            System.out.println("Failed to open port: " + port.getSystemPortName());
        }

        return deviceFound;
    }
    
    void sendLaserCommand(byte channel, short value) {
    	Protocol.SetLaserCommand laserCommand = new Protocol.SetLaserCommand(channel, value); // chId=1, laser value=0xABC
    	byte[] setLaserMessage = Protocol.createSetLaserMessage(laserCommand);
    	
    	if (devicePort.openPort()) {
            try {
                // Set port parameters based on the instance's settings
            	devicePort.setBaudRate(baudRate);
            	devicePort.setNumDataBits(numDataBits);
            	devicePort.setNumStopBits(numStopBits);
                devicePort.setParity(parity);

                // Get the input/output streams for writing
                OutputStream outputStream = devicePort.getOutputStream();
                System.out.print("Message: ");
                for (byte b : setLaserMessage) {
                    System.out.printf("%02X ", b); // Print each byte as two-digit hexadecimal
                }
                System.out.println();
                // Send set_laser ch_id val
                outputStream.write(setLaserMessage);
                outputStream.flush();
                
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                // Close the port after the operation
            	devicePort.closePort();
            }
    	}
    }
    
    void sendMotorCommand(short value) {
    	Protocol.SetMotorCommand motorCommand = new Protocol.SetMotorCommand(value); // chId=1, laser value=0xABC
    	byte[] setMotorMessage = Protocol.createSetMotorMessage(motorCommand);
    	
    	if (devicePort.openPort()) {
            try {
                // Set port parameters based on the instance's settings
            	devicePort.setBaudRate(baudRate);
            	devicePort.setNumDataBits(numDataBits);
            	devicePort.setNumStopBits(numStopBits);
                devicePort.setParity(parity);

                // Get the input/output streams for writing
                OutputStream outputStream = devicePort.getOutputStream();
                System.out.print("Message: ");
                for (byte b : setMotorMessage) {
                    System.out.printf("%02X ", b); // Print each byte as two-digit hexadecimal
                }
                System.out.println();
                // Send set_laser ch_id val
                outputStream.write(setMotorMessage);
                outputStream.flush();
                
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                // Close the port after the operation
            	devicePort.closePort();
            }
    	}
    }
  }