package com.myname.lasercontrollerui;

import java.awt.EventQueue;
import java.util.HashMap;
import java.util.TreeMap;
import de.embl.rieslab.emu.controller.SystemController;
import de.embl.rieslab.emu.ui.ConfigurableMainFrame;
import de.embl.rieslab.emu.utils.settings.Setting;
import javax.swing.JPanel;
import java.awt.GridLayout;


public class MyFrame extends ConfigurableMainFrame {

    private static final long serialVersionUID = 1L;

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

        JPanel panel = new JPanel();
        panel.setBounds(5, 55, 631, 313);
        getContentPane().add(panel);
        panel.setLayout(new GridLayout(1, 0, 0, 0));

        // Create laser panels dynamically
        for (int i = 0; i < 7; i++) {
            LaserPanel laserPanel = new LaserPanel("laser" + i);
            panel.add(laserPanel);
           
            // Button listener
//            laserPanel.addButtonListener(e -> {
//            	// Get the text from the text field
//                String textValue = laserPanel.getTextFieldValue();
//
//                // Ensure the text contains only numbers
//                if (textValue.matches("\\d+")) { // Check if the text is a valid number
//                    float value = Float.parseFloat(textValue); // Convert to integer
//
//                    // Ensure the value is within the valid range (0-100)
//                    if (value >= 0.0 && value <= 100.0) {
//                        // Update the slider and label in the LaserPanel
//                    	int percentage = Math.round(value);
//                        laserPanel.getSlider().setValue(percentage);
//                        laserPanel.getLblNewLabel().setText(percentage + "%");
//                    } else {
//                        // Handle invalid range (optional: show an error message)
//                        System.out.println("Value must be between 0 and 100.");
//                    }
//                } else {
//                    // Handle invalid input (optional: show an error message)
//                    System.out.println("Invalid input. Please enter a number.");
//                }
//            });
        }
    }
}
