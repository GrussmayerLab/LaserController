package com.myname.lasercontrollerui;

import java.awt.EventQueue;
import java.util.HashMap;
import java.util.TreeMap;
import de.embl.rieslab.emu.controller.SystemController;
import de.embl.rieslab.emu.ui.ConfigurableMainFrame;
import de.embl.rieslab.emu.utils.settings.Setting;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import java.awt.GridLayout;
import javax.swing.JButton;
import javax.swing.JLabel;
import com.fazecast.jSerialComm.*;

public class MyFrame extends ConfigurableMainFrame {

    private static final long serialVersionUID = 1L;

    private USBManager serialDevice;
    private int baudRate = 115200;
    private int numDataBits = 8;
    private int numStopBits = SerialPort.ONE_STOP_BIT;
    private int parity = SerialPort.NO_PARITY;

    private boolean motorButton = false;
    private boolean[] laserButtons = new boolean[6];  // Array for laser buttons
    private int motorValue = 0;
    private int[] laserValues = new int[6]; // Array for laser values

    public static void main(String[] args) {
        EventQueue.invokeLater(() -> {
            try {
                MyFrame frame = new MyFrame();
                frame.setVisible(true);
            } catch (Exception e) {
                e.printStackTrace();
            }
        });
    }

    public MyFrame() {
        super("", null, null); // Calls super constructor
        initComponents();
    }

    public MyFrame(String arg0, SystemController arg1, TreeMap<String, String> arg2) {
        super(arg0, arg1, arg2);
    }

    @Override
    public HashMap<String, Setting> getDefaultPluginSettings() {
        return new HashMap<>();
    }

    @Override
    protected String getPluginInfo() {
        return "Description of the plugin and mention of the author.";
    }

    @Override
    protected void initComponents() {
        setBounds(100, 100, 650, 427);
        getContentPane().setLayout(null);

        serialDevice = new USBManager(baudRate, numDataBits, numStopBits, parity);

        JPanel panel = new JPanel();
        panel.setBounds(5, 55, 631, 313);
        getContentPane().add(panel);
        panel.setLayout(new GridLayout(1, 0, 0, 0));

        // Create laser panels dynamically
        for (int i = 0; i < 6; i++) {
            LaserPanel laserPanel = new LaserPanel("laser" + i);
            panel.add(laserPanel);

            // Slider listener
            final int laserIndex = i;
            laserPanel.addSliderListener(e -> {
                try {
                    short result;
                    if (laserButtons[laserIndex]) {
                        laserValues[laserIndex] = laserPanel.getSlider().getValue();
                        result = (short) (laserValues[laserIndex] * 4095 / 100);
                        serialDevice.sendLaserCommand((byte) laserIndex, result);
                    }
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            });

            // Checkbox listener
            laserPanel.addCheckboxListener(e -> {
                laserButtons[laserIndex] = laserPanel.getChckbxNewCheckBox().isSelected();
                short result;
                if (!laserButtons[laserIndex]) {
                    serialDevice.sendLaserCommand((byte) laserIndex, (byte) 0);
                } else {
                    laserValues[laserIndex] = laserPanel.getSlider().getValue();
                    result = (short) (laserValues[laserIndex] * 4095 / 100);
                    serialDevice.sendLaserCommand((byte) laserIndex, result);
                }
            });
            
            // Button listener
            laserPanel.addButtonListener(e -> {
            	// Get the text from the text field
                String textValue = laserPanel.getTextFieldValue();

                // Ensure the text contains only numbers
                if (textValue.matches("\\d+")) { // Check if the text is a valid number
                    int value = Integer.parseInt(textValue); // Convert to integer

                    // Ensure the value is within the valid range (0-100)
                    if (value >= 0 && value <= 4095) {
                        // Update the slider and label in the LaserPanel
                    	int percentage = (int)(value * 100 / 4095);
                        laserPanel.getSlider().setValue(percentage);
                        laserPanel.getLblNewLabel().setText(percentage + "%");

                        // Update the laser value and send the command if the laser is on
                        if (laserButtons[laserIndex]) {
                            laserValues[laserIndex] = percentage;
                            short result = (short) (value);
                            System.out.println(value);
                            serialDevice.sendLaserCommand((byte) laserIndex, result);
                        }
                    } else {
                        // Handle invalid range (optional: show an error message)
                        System.out.println("Value must be between 0 and 4095.");
                    }
                } else {
                    // Handle invalid input (optional: show an error message)
                    System.out.println("Invalid input. Please enter a number.");
                }
            });
        }

        // Motor Panel
        LaserPanel motorPanel = new LaserPanel("Motor");
        panel.add(motorPanel);

        // Motor slider listener
        motorPanel.addSliderListener(e -> {
            try {
                short result;
                if (motorButton) {
                    motorValue = motorPanel.getSlider().getValue();
                    result = (short) (motorValue * 65535 / 100);
                    serialDevice.sendMotorCommand(result);
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        });

        // Motor checkbox listener
        motorPanel.addCheckboxListener(e -> {
            motorButton = motorPanel.getChckbxNewCheckBox().isSelected();
            short result;
            if (!motorButton) {
                serialDevice.sendMotorCommand((byte) 0);
            } else {
                motorValue = motorPanel.getSlider().getValue();
                result = (short) (motorValue * 65535 / 100);
                serialDevice.sendMotorCommand(result);
            }
        });

        // Scan Ports Button
        JButton btnScanPorts = new JButton("Scan Ports");
        btnScanPorts.setBounds(512, 24, 104, 21);
        getContentPane().add(btnScanPorts);

        JLabel lblDevice = new JLabel("Device:");
        lblDevice.setBounds(388, 28, 55, 13);
        getContentPane().add(lblDevice);

        JLabel deviceNameLabel = new JLabel("");
        deviceNameLabel.setBounds(440, 28, 55, 13);
        getContentPane().add(deviceNameLabel);

        btnScanPorts.addActionListener(e -> {
            try {
                String result = serialDevice.scanAndIdentifyDevice();
                SwingUtilities.invokeLater(() -> deviceNameLabel.setText(result));
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        });
        
        // Button listener
        motorPanel.addButtonListener(e -> {
        	// Get the text from the text field
            String textValue = motorPanel.getTextFieldValue();

            // Ensure the text contains only numbers
            if (textValue.matches("\\d+")) { // Check if the text is a valid number
                int value = Integer.parseInt(textValue); // Convert to integer

                // Ensure the value is within the valid range (0-100)
                if (value >= 0 && value <= 65355) {
                    // Update the slider and label in the LaserPanel
                	int percentage = (int)(value * 100 / 65355);
                	motorPanel.getSlider().setValue(percentage);
                	motorPanel.getLblNewLabel().setText(percentage + "%");

                    // Update the laser value and send the command if the laser is on
                    if (motorButton) {
                        motorValue = percentage;
                        short result = (short) (value);
                        System.out.println(value);
                        serialDevice.sendMotorCommand(result);
                    }
                } else {
                    // Handle invalid range (optional: show an error message)
                    System.out.println("Value must be between 0 and 65355.");
                }
            } else {
                // Handle invalid input (optional: show an error message)
                System.out.println("Invalid input. Please enter a number.");
            }
        });
    }
}
