
# Laser Controller Project

## TL;DR (Quick Setup)

1. **Assemble the PCB**: 
   - Order the PCB from [JLCPCB](https://cart.jlcpcb.com) for ~$100 using the `gerber.zip`, `bom.csv`, and `laser_controller-top-pos.csv` files.
   - Use compatible [JST cables](https://www.amazon.nl/-/en/Mini-Micro-150mm-Cable-Female/dp/B01DU9OY40).

2. **Install MicroManager 2.0**:
   - Download and install [MicroManager 2.0](https://download.micro-manager.org/nightly/2.0/Windows/).

3. **Copy Files**:
   - Copy the precompiled `EMU` folder and `mmgr_dal_GrussmayerLab-LaserController.dll` into the root directory of MicroManager.

4. **Use the LaserControlGUI Plugin**:
   - Run MicroManager, open the **LaserControlGUI** plugin from **EMU**, and start controlling the laser!

---

This repository consists of the hardware, firmware, and software design for the Laser Controller system. It is divided into four main sections:

- **Electronics**: Schematic and PCB design files for the laser controller.
- **LaserControlFirmware**: C/C++ firmware for the Raspberry Pi Pico.
- **GrussmayerLab-LaserController**: C++ Micromanager Device Adapter to drive the laser controller.
- **LaserControlGUI**: Java-based plugin for Micromanager, built upon the EMU plugin.

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

1. **Hardware**: Electronics, including a schematic and PCB design, for laser control.
2. **Firmware**: Raspberry Pi Pico firmware (C/C++) managing communication with the hardware.
3. **Software**: A Java plugin for MicroManager, based on the EMU plugin framework, along with a C++ Device Adapter for MicroManager.

## Folders

### Electronics

This folder contains the hardware designs for the laser controller:

- **Schematic**: Electrical design for the laser control system.
- **PCB Design**: Printed circuit board design files created using KiCAD.

Files in this folder:
- `schematic.kicad_sch`: Schematic file.
- `pcb.kicad_pcb`: PCB design file.

For more details, refer to the [KiCAD Documentation](https://kicad.org/documentation/).

### LaserControlFirmware

Firmware for the Raspberry Pi Pico, written in C/C++, responsible for controlling the laser system.

Files in this folder:
- `main.cpp`: Main program code.
- `CMakeLists.txt`: CMake configuration.
- `README.md`: Firmware setup instructions.

### LaserControlGUI

Java-based plugin for Micromanager, allowing control of the laser system.

Files in this folder:
- `LaserPanel.java`: Laser slide panel code.
- `MyFrame.java`: Main plugin UI containing multiple LaserPanel instances.
- `MyPlugin.java`: Main plugin class implementing the UIPlugin interface.

## Getting Started

### Hardware Setup

1. **Review Hardware Files**: Use KiCAD to view schematic and PCB design files in the `Electronics` folder.
2. **Assemble the PCB**: Using `gerber.zip`, `bom.csv`, and `laser_controller-top-pos.csv`, you can assemble your PCB at [JLCPCB](https://cart.jlcpcb.com) for approximately $100.
3. **Connect Components**: The board has **JST S3B-PH-K-S (LF)(SN)** connectors. Buy [compatible cables](https://www.amazon.nl/-/en/Mini-Micro-150mm-Cable-Female/dp/B01DU9OY40) and modify them as needed.

### Firmware Setup

1. **Flashing Precompiled Firmware**:
   - The compiled UF2 firmware is already included in the repository.
   - Hold the **BOOTSEL** button on the Raspberry Pi Pico, connect it to a PC via USB, and it will appear as a **USB drive**.
   - Copy the UF2 file to the drive, and the Pico is programmed.

2. **Optional: Building from Source**:
   If you wish to modify the firmware, install the following tools:
   - [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
   - [CMake](https://cmake.org/)
   - [ARM GCC Toolchain](https://developer.arm.com/tools-and-software/openSourceTools/gnu-toolchain/gnu-toolchain-r)

   Clone the repository and build:
   ```bash
   git clone https://github.com/GrussmayerLab/LaserController.git
   cd LaserControlFirmware
   mkdir build
   cd build
   cmake ..
   make
   ```

### Software Setup

1. **Install MicroManager 2.0**:
   - Download and install the latest version of [MicroManager 2.0](https://download.micro-manager.org/nightly/2.0/Windows/). Copy the precompiled `EMU` folder (provided) into the root directory of MicroManager.

2. **Install the EMU Plugin** (for source code editing):
   - If you wish to modify or extend the plugin, follow the instructions on how to install [EMU](https://jdeschamps.github.io/EMU-guide/):

3. **Install LaserControlGUI Plugin**:
   - To enable the LaserControlUI plugin, export the Java project from Eclipse into the EMU folder in MicroManager.

4. **Install the Device Adapter**:
   - Copy the `mmgr_dal_GrussmayerLab-LaserController.dll` file in the root directory of MicroManager.
   - If you need to configure or extend the device adapter, follow the [MicroManager Visual Studio setup guide](https://micro-manager.org/Visual_Studio_project_settings_for_device_adapters). After setting up your project, place `.h` files in the header folder and `.cpp` files in the source folder. Compile the project, and place the resulting `.dll` file in the root directory of MicroManager.

## Usage

1. Start MicroManager.
2. Open the **LaserControlGUI** plugin.
3. The plugin will interface with the Raspberry Pi Pico, allowing laser control via MicroManager.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
