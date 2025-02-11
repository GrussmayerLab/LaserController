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
    private boolean laserButton_0 = false;
    private boolean laserButton_1 = false;
    private boolean laserButton_2 = false;
    private boolean laserButton_3 = false;
    private boolean laserButton_4 = false;
    private int motorValue = 0;
    private int laserValue_0 = 0;
    private int laserValue_1 = 0;
    private int laserValue_2 = 0;
    private int laserValue_3 = 0;
    private int laserValue_4 = 0;

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


	/**
	 * Create the frame.
	 */
	public MyFrame() {
		super("",null,null); // calls superconstructor
		initComponents();
	}

	public MyFrame(String arg0, SystemController arg1, TreeMap<String, String> arg2) {
		super(arg0, arg1, arg2);
	}

	@Override
	public HashMap<String, Setting> getDefaultPluginSettings() {
		HashMap<String, Setting>  settgs = new HashMap<String, Setting>();
		return settgs;
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
		
		LaserPanel laserPanel_0 = new LaserPanel("laser0");
		panel.add(laserPanel_0);
		
		LaserPanel laserPanel_1 = new LaserPanel("laser1");
		panel.add(laserPanel_1);
		
		LaserPanel laserPanel_2 = new LaserPanel("laser2");
		panel.add(laserPanel_2);
		
		LaserPanel laserPanel_3 = new LaserPanel("laser3");
		panel.add(laserPanel_3);
		
		LaserPanel laserPanel_4 = new LaserPanel("laser4");
		panel.add(laserPanel_4);
		
		LaserPanel laserPanel_5 = new LaserPanel("Motor");
		panel.add(laserPanel_5);
		
		// Add a listener to track slider changes
		laserPanel_5.addSliderListener(e -> {
			try {
				short result;
				if (motorButton) {
				    // Get the current value of the slider (this value will be in percentage)
					motorValue = laserPanel_5.getSlider().getValue();
				    System.out.println("Slider value changed: " + motorValue + "%");
				    result = (short) (motorValue * 65535 / 100); // result will be in the range 0 to 65535
				    serialDevice.sendMotorCommand(result); 
				} else {
				    result = 0;
				}

			} catch (Exception ex) {
	            ex.printStackTrace(); // Handle any exceptions
	        }
		});

		// Add a listener to track checkbox changes (on/off)
		laserPanel_5.addCheckboxListener(e -> {
		    // Check if the checkbox is selected (laser on or off)
			motorButton = laserPanel_5.getChckbxNewCheckBox().isSelected();
			short result;
		    if (!motorButton) {
			    serialDevice.sendMotorCommand((byte)0); 
		    } else {
		    	motorValue = laserPanel_5.getSlider().getValue();
			    result = (short) (motorValue * 65535 / 100); // result will be in the range 0 to 65535
			    serialDevice.sendMotorCommand(result); 
		    }
		    System.out.println("Checkbox value changed: " + (motorButton ? "On" : "Off"));
		});
		
		// Add a listener to track slider changes
		laserPanel_4.addSliderListener(e -> {
			try {
				short result;
				if (laserButton_4) {
				    // Get the current value of the slider (this value will be in percentage)
				    laserValue_4 = laserPanel_4.getSlider().getValue();
				    System.out.println("Slider value changed: " + laserValue_4 + "%");
				    result = (short) (laserValue_4 * 4096 / 100); // result will be in the range 0 to 65535
				    serialDevice.sendLaserCommand((byte)4, result); 
				} else {
				    result = 0;
				}

			} catch (Exception ex) {
	            ex.printStackTrace(); // Handle any exceptions
	        }
		});

		// Add a listener to track checkbox changes (on/off)
		laserPanel_4.addCheckboxListener(e -> {
		    // Check if the checkbox is selected (laser on or off)
			laserButton_4 = laserPanel_4.getChckbxNewCheckBox().isSelected();
			short result;
		    if (!laserButton_4) {
			    serialDevice.sendMotorCommand((byte)4); 
		    } else {
		    	laserValue_4 = laserPanel_4.getSlider().getValue();
			    result = (short) (laserValue_4 * 4096 / 100); // result will be in the range 0 to 65535
			    serialDevice.sendLaserCommand((byte)4, result); 
		    }
		    System.out.println("Checkbox value changed: " + (laserButton_4 ? "On" : "Off"));
		});
		
		// Add a listener to track slider changes
		laserPanel_3.addSliderListener(e -> {
			try {
				short result;
				if (laserButton_3) {
				    // Get the current value of the slider (this value will be in percentage)
				    laserValue_3 = laserPanel_3.getSlider().getValue();
				    System.out.println("Slider value changed: " + laserValue_3 + "%");
				    result = (short) (laserValue_3 * 4096 / 100); // result will be in the range 0 to 65535
				    serialDevice.sendLaserCommand((byte)3, result); 
				} else {
				    result = 0;
				}

			} catch (Exception ex) {
	            ex.printStackTrace(); // Handle any exceptions
	        }
		});

		// Add a listener to track checkbox changes (on/off)
		laserPanel_3.addCheckboxListener(e -> {
		    // Check if the checkbox is selected (laser on or off)
			laserButton_3 = laserPanel_3.getChckbxNewCheckBox().isSelected();
			short result;
		    if (!laserButton_3) {
			    serialDevice.sendMotorCommand((byte)3); 
		    } else {
		    	laserValue_3 = laserPanel_3.getSlider().getValue();
			    result = (short) (laserValue_3 * 4096 / 100); // result will be in the range 0 to 65535
			    serialDevice.sendLaserCommand((byte)3, result); 
		    }
		    System.out.println("Checkbox value changed: " + (laserButton_3 ? "On" : "Off"));
		});
		
		// Add a listener to track slider changes
		laserPanel_2.addSliderListener(e -> {
			try {
				short result;
				if (laserButton_2) {
				    // Get the current value of the slider (this value will be in percentage)
				    laserValue_2 = laserPanel_2.getSlider().getValue();
				    System.out.println("Slider value changed: " + laserValue_2 + "%");
				    result = (short) (laserValue_2 * 4096 / 100); // result will be in the range 0 to 65535
				    serialDevice.sendLaserCommand((byte)2, result); 
				} else {
				    result = 0;
				}

			} catch (Exception ex) {
	            ex.printStackTrace(); // Handle any exceptions
	        }
		});

		// Add a listener to track checkbox changes (on/off)
		laserPanel_2.addCheckboxListener(e -> {
		    // Check if the checkbox is selected (laser on or off)
			laserButton_2 = laserPanel_2.getChckbxNewCheckBox().isSelected();
			short result;
		    if (!laserButton_2) {
			    serialDevice.sendMotorCommand((byte)2); 
		    } else {
		    	laserValue_2 = laserPanel_2.getSlider().getValue();
			    result = (short) (laserValue_2 * 4096 / 100); // result will be in the range 0 to 65535
			    serialDevice.sendLaserCommand((byte)2, result); 
		    }
		    System.out.println("Checkbox value changed: " + (laserButton_2 ? "On" : "Off"));
		});
		
		// Add a listener to track slider changes
		laserPanel_1.addSliderListener(e -> {
			try {
				short result;
				if (laserButton_1) {
				    // Get the current value of the slider (this value will be in percentage)
				    laserValue_1 = laserPanel_1.getSlider().getValue();
				    System.out.println("Slider value changed: " + laserValue_1 + "%");
				    result = (short) (laserValue_1 * 4096 / 100); // result will be in the range 0 to 65535
				    serialDevice.sendLaserCommand((byte)1, result); 
				} else {
				    result = 0;
				}

			} catch (Exception ex) {
	            ex.printStackTrace(); // Handle any exceptions
	        }
		});

		// Add a listener to track checkbox changes (on/off)
		laserPanel_1.addCheckboxListener(e -> {
		    // Check if the checkbox is selected (laser on or off)
			laserButton_1 = laserPanel_1.getChckbxNewCheckBox().isSelected();
			short result;
		    if (!laserButton_1) {
			    serialDevice.sendMotorCommand((byte)1); 
		    } else {
		    	laserValue_1 = laserPanel_1.getSlider().getValue();
			    result = (short) (laserValue_1 * 4096 / 100); // result will be in the range 0 to 65535
			    serialDevice.sendLaserCommand((byte)1, result); 
		    }
		    System.out.println("Checkbox value changed: " + (laserButton_1 ? "On" : "Off"));
		});
		
		// Add a listener to track slider changes
		laserPanel_0.addSliderListener(e -> {
			try {
				short result;
				if (laserButton_0) {
				    // Get the current value of the slider (this value will be in percentage)
				    laserValue_0 = laserPanel_0.getSlider().getValue();
				    System.out.println("Slider value changed: " + laserValue_0 + "%");
				    result = (short) (laserValue_0 * 4096 / 100); // result will be in the range 0 to 65535
				    serialDevice.sendLaserCommand((byte)0, result); 
				} else {
				    result = 0;
				}

			} catch (Exception ex) {
	            ex.printStackTrace(); // Handle any exceptions
	        }
		});

		// Add a listener to track checkbox changes (on/off)
		laserPanel_0.addCheckboxListener(e -> {
		    // Check if the checkbox is selected (laser on or off)
			laserButton_0 = laserPanel_0.getChckbxNewCheckBox().isSelected();
			short result;
		    if (!laserButton_0) {
			    serialDevice.sendMotorCommand((byte)0); 
		    } else {
		    	laserValue_1 = laserPanel_1.getSlider().getValue();
			    result = (short) (laserValue_1 * 4096 / 100); // result will be in the range 0 to 65535
			    serialDevice.sendLaserCommand((byte)0, result); 
		    }
		    System.out.println("Checkbox value changed: " + (laserButton_0 ? "On" : "Off"));
		});
		
		JButton btn_scan_ports = new JButton("Scan Ports");
		btn_scan_ports.setBounds(512, 24, 104, 21);
		getContentPane().add(btn_scan_ports);
		
		JLabel lblNewLabel = new JLabel("Device:");
		lblNewLabel.setBounds(388, 28, 55, 13);
		getContentPane().add(lblNewLabel);
		
		JLabel device_name_label = new JLabel("");
		device_name_label.setBounds(440, 28, 55, 13);
		getContentPane().add(device_name_label);
		btn_scan_ports.addActionListener(e -> {
	        try {
	            // Perform the serial device scanning and identification in the background
	            String result = serialDevice.scanAndIdentifyDevice();
	            
	            // Update the device name label in the Swing UI thread (Swing is not thread-safe)
	            SwingUtilities.invokeLater(() -> {
	                device_name_label.setText(result);
	            });
	        } catch (Exception ex) {
	            ex.printStackTrace(); // Handle any exceptions
	        }
		});

	}
}
