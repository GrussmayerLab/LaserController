package com.myname.lasercontrollerui;

import de.embl.rieslab.emu.ui.ConfigurablePanel;
import de.embl.rieslab.emu.ui.swinglisteners.SwingUIListeners;
import de.embl.rieslab.emu.ui.uiparameters.ColorUIParameter;
import de.embl.rieslab.emu.ui.uiparameters.StringUIParameter;
import de.embl.rieslab.emu.ui.uiproperties.RescaledUIProperty;
import de.embl.rieslab.emu.ui.uiproperties.TwoStateUIProperty;
import de.embl.rieslab.emu.ui.uiproperties.UIProperty;
import de.embl.rieslab.emu.utils.EmuUtils;
import de.embl.rieslab.emu.utils.exceptions.IncorrectUIParameterTypeException;
import de.embl.rieslab.emu.utils.exceptions.IncorrectUIPropertyTypeException;
import de.embl.rieslab.emu.utils.exceptions.UnknownUIParameterException;
import de.embl.rieslab.emu.utils.exceptions.UnknownUIPropertyException;

import javax.swing.border.TitledBorder;
import javax.swing.JLabel;

import java.awt.Color;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JCheckBox;
import javax.swing.JSlider;
import javax.swing.SwingConstants;
import javax.swing.JTextField;
import javax.swing.JButton;

public class LaserPanel extends ConfigurablePanel {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private JLabel label_1;
	private JCheckBox tglbtnOnoff;
	private JSlider slider;
    // Fields to hold the external listeners
    private ActionListener sliderListener;
    private ActionListener checkboxListener;
    private ActionListener textFieldListener;

	public final String LASER_PERCENTAGE = "power percentage";
	public final String LASER_OPERATION = "enable";
	public final String PARAM_TITLE = "Name";
	public final String PARAM_COLOR = "Color";
	public LaserPanel(String label) {
		super(label);
		
		initComponents();
		// TODO Auto-generated constructor stub
	}
	
	private void initComponents() {
		setBorder(new TitledBorder(null, "Laser", TitledBorder.LEADING, TitledBorder.TOP, null, null));
		setLayout(null);
		
		label_1 = new JLabel("70%");
		label_1.setFont(new Font("Tahoma", Font.BOLD, 12));
		label_1.setBounds(47, 20, 45, 13);
		add(label_1);
		
		tglbtnOnoff = new JCheckBox("On");
		tglbtnOnoff.setBounds(38, 228, 45, 21);
		add(tglbtnOnoff);
		
		slider = new JSlider();
		slider.setMajorTickSpacing(20);
		slider.setPaintTicks(true);
		slider.setPaintLabels(true);
		slider.setOrientation(SwingConstants.VERTICAL);
		slider.setBounds(10, 43, 103, 179);
		add(slider);
		
        // Add internal listeners
        slider.addChangeListener(e -> {
            if (sliderListener != null) {
                sliderListener.actionPerformed(new ActionEvent(slider, ActionEvent.ACTION_PERFORMED, "slider"));
            }
            label_1.setText(slider.getValue() + "%");
        });

        tglbtnOnoff.addActionListener(e -> {
            if (checkboxListener != null) {
                checkboxListener.actionPerformed(new ActionEvent(tglbtnOnoff, ActionEvent.ACTION_PERFORMED, "checkbox"));
            }
        });
	}
	
    // Method to add an external listener for the slider
    public void addSliderListener(ActionListener listener) {
        this.sliderListener = listener;
    }

    // Method to add an external listener for the checkbox
    public void addCheckboxListener(ActionListener listener) {
        this.checkboxListener = listener;
    }
    
    // Method to get the text from the text field
    public void addTextFieldListener(ActionListener listener) {
        this.textFieldListener = listener;
    }

	@Override
	protected void addComponentListeners() {
		String propertyName1 = getPanelLabel() + " " + LASER_PERCENTAGE;
		String propertyName2 = getPanelLabel() + " " + LASER_OPERATION;

		SwingUIListeners.addActionListenerOnIntegerValue(this, propertyName1, slider,
	                                                     label_1, "", "%");

		try {
			SwingUIListeners.addActionListenerToTwoState(this, propertyName2, tglbtnOnoff);
		} catch (IncorrectUIPropertyTypeException e) {
			e.printStackTrace();
		}
		
	}

	@Override
	public String getDescription() {
		return "Panel controlling the power percentage and operation of a single laser.";
	}

	@Override
	protected void initializeInternalProperties() {
		// Do nothing: we do not have internalProperties here.

	}

	@Override
	protected void initializeParameters() {
		addUIParameter(new StringUIParameter(this, PARAM_TITLE, "Panel title.",
	                                         getPanelLabel()));
		addUIParameter(new ColorUIParameter(this, PARAM_COLOR, "Laser color.", Color.black));
	}

	@Override
	protected void initializeProperties() {
		// Defines convenience variables
		String text1 = "Property changing the power percentage of the device.";
		String text2 = "Property turning the laser on and off.";
        String propertyName1 = getPanelLabel() + " " + LASER_PERCENTAGE;
        String propertyName2 = getPanelLabel() + " " + LASER_OPERATION;
		
        // We declare a UIProperty for the laser percentage
		addUIProperty(new RescaledUIProperty(this, propertyName1, text1));
		
		// and one for the laser on/off
		addUIProperty(new TwoStateUIProperty(this, propertyName2, text2));
	}

	@Override
	public void internalpropertyhasChanged(String arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	protected void parameterhasChanged(String parameterName) {
		if (PARAM_TITLE.equals(parameterName)) {
			try {
				((TitledBorder) this.getBorder())
	                	.setTitle(getStringUIParameterValue(PARAM_TITLE));
				this.repaint();
			} catch (UnknownUIParameterException e) {
				e.printStackTrace();			
	        }
	    } else if (PARAM_COLOR.equals(parameterName)) {
			try {
				((TitledBorder) this.getBorder())
	               	.setTitleColor(getColorUIParameterValue(PARAM_COLOR));
				this.repaint();
			} catch (IncorrectUIParameterTypeException | UnknownUIParameterException e) {
				e.printStackTrace();
			}
		}
	}

	@Override
	protected void propertyhasChanged(String propertyName, String newvalue) {
		// convenience variables
        String propertyName1 = getPanelLabel() + " " + LASER_PERCENTAGE;
        String propertyName2 = getPanelLabel() + " " + LASER_OPERATION;
		
		if (propertyName1.equals(propertyName)) { // if the change concerns the laser percentage
			if (EmuUtils.isNumeric(newvalue)) { // we only accept numerical value (no string)
				// JSlider accept only an integer, this has the effect of rounding up the value 
				int val = (int) Double.parseDouble(newvalue); 

				// We make sure it is a value between 0 and 100
				if (val >= 0 && val <= 100) {
					// sets the value of the JSLider
					slider.setValue(val);
					
					// change the text of the JLabel to reflect the change
					label_1.setText(String.valueOf(val) + "%");
				}
			}
		} else if (propertyName2.equals(propertyName)) { // if the change pertains to the laser on/off
			try {
				// Gets the value of the TwoStateUIProperty's ON value.
				String onValue = ((TwoStateUIProperty) getUIProperty(propertyName2)).getOnStateValue();
				
				// Selects the JToggleButton if the new value is the TwoStateUIProperty's ON value,
				// unselects it otherwise.
				tglbtnOnoff.setSelected(newvalue.equals(onValue));
			} catch (UnknownUIPropertyException e) { // Necessary in case propertyName2 is NOT a TwoStateUIProperty
				e.printStackTrace();
			}
		}
	}
	
	@Override
	public void shutDown() {
		// TODO Auto-generated method stub
		
	}
	protected JLabel getLblNewLabel() {
		return label_1;
	}
	protected JCheckBox getChckbxNewCheckBox() {
		return tglbtnOnoff;
	}
	protected JSlider getSlider() {
		return slider;
	}
}