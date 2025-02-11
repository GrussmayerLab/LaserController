# Laser Controller Project

This repository consists of the hardware, firmware, and software design for the Laser Controller system. It is divided into three main sections:

- **Electronics**: Schematic and PCB design files for the laser controller.
- **LaserControlFirmware**: C/C++ firmware for the Raspberry Pi Pico.
- **LaserControllerUI**: Java-based plugin for Micromanager, built upon the EMU plugin.

## Table of Contents

- [Overview](#overview)
- [Folders](#folders)
  - [Electronics](#electronics)
  - [LaserControlFirmware](#lasercontrolfirmware)
  - [LaserControllerUI](#lasercontrollerui)
- [Getting Started](#getting-started)
  - [Hardware Setup](#hardware-setup)
  - [Firmware Setup](#firmware-setup)
  - [Software Setup](#software-setup)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Overview

The Laser Controller project integrates hardware and software components to control a laser system. It consists of:

1. **Hardware**: The electronics, which include a schematic and PCB design, are implemented to control the laser.
2. **Firmware**: The Raspberry Pi Pico firmware written in C/C++ manages communication with the hardware.
3. **Software**: The user interface for controlling the system is implemented as a Java plugin for Micromanager, based on the EMU plugin framework.

## Folders

### Electronics

This folder contains the hardware designs for the laser controller, including:

- **Schematic**: The electrical design for the laser control system.
- **PCB Design**: The printed circuit board design files created using KiCAD.

Files in this folder:
- `schematic.kicad_sch`: The schematic file.
- `pcb.kicad_pcb`: The PCB design file.

For more information on how to use KiCAD to view and modify these files, refer to the [KiCAD Documentation](https://kicad.org/documentation/).

### LaserControlFirmware

This folder contains the firmware for the Raspberry Pi Pico written in C/C++ to control the laser system. The firmware interacts with the hardware designed in the **Electronics** folder.

Files in this folder:
- `main.cpp`: The main program code for the Raspberry Pi Pico.
- `CMakeLists.txt`: CMake configuration for building the project.
- `README.md`: Firmware setup instructions.

For compiling and flashing the firmware, you will need to follow the steps outlined below in the **Firmware Setup** section.

### LaserControllerUI

This folder contains the Java plugin for Micromanager. The plugin extends Micromanager to communicate with the Laser Controller hardware.

- Built on top of the EMU plugin, this software enables controlling the laser system via Micromanager.
- The plugin allows the user to easily interface with the laser hardware through Micromanager's graphical user interface.

Files in this folder:
- `LaserControllerUI.java`: The main code for the plugin.

For details on how to set up and install Micromanager with this plugin, refer to the **Software Setup** section.

## Getting Started

### Hardware Setup

1. **Schematic and PCB Design**: Review the schematic and PCB design files in the `Electronics` folder to understand the hardware layout.
   
   You can use KiCAD to open these files and modify them if needed.

2. **Assemble the Hardware**: After reviewing the designs, manufacture and assemble the PCB. You'll need components such as resistors, capacitors, connectors, etc., as specified in the schematic.

3. **Connections**: Connect the Raspberry Pi Pico to the hardware according to the schematic provided in the `Electronics` folder.

### Firmware Setup

1. **Install Development Tools**: To compile the firmware, ensure you have the following tools installed:
   - [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
   - [CMake](https://cmake.org/)
   - [ARM GCC Toolchain](https://developer.arm.com/tools-and-software/openSourceTools/gnu-toolchain/gnu-toolchain-r)
   
2. **Clone the Repository**: Ensure you have cloned the repository locally:
   ```bash
   git clone https://github.com/yourusername/laser-controller.git
   cd laser-controller/LaserControlFirmware

3. **Build the Firmware**: Create a build directory:
    ```bash
    cd LaserControllerUI
    mkdir build
    cd build
    cmake ..
    make
4. **Flash the Firmware**: Follow the instructions in the [Raspberry Pi Pico Documentation](https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html) to flash the firmware to your Raspberry Pi Pico.

#### Software Setup

1. Install Micromanager: [Download and install Micromanager-gamma](https://micro-manager.org/news/2020-12-12-version-2-gamma-available).
2. Follow the instruction on how to install [EMU](https://jdeschamps.github.io/EMU-guide/):
3. Install the Plugin: Copy the generated .jar file into the EMU folder in Micro-Manager.
3. Once installed, configure the plugin in Micromanager to interface with the Raspberry Pi Pico by opening Plugins/User Interface/EMU. Details of the configuration will be provided in the Micromanager UI.

#### Usage
* After setup, launch Micromanager and start the LaserControllerUI plugin.
* The plugin should interface with the Raspberry Pi Pico and allow you to control the laser system via the Micromanager interface.

#### License
This project is licensed under the MIT License - see the LICENSE file for details.
