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
        for (int i = 0; i < 6; i++) {
            LaserPanel laserPanel = new LaserPanel("laser" + i);
            panel.add(laserPanel);           
        }
    }
}
