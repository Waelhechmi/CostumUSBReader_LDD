# CostumUSBReader_LDD
## USB Reader Module

This project implements a Linux USB reader module that reads data from a connected USB device and makes it available in user space. It supports USB devices with a specific Vendor ID and Product ID, and it creates a character device file to allow interaction with the USB device from user space.

### Features
- Reads data from a USB device.
- Displays the data in user space via a character device.
- USB probe and disconnect functions to handle device connection and removal.
- Mutex to ensure safe access to the device in a multi-threaded environment.

### Prerequisites
- Linux Kernel development environment.
- USB device with Vendor ID `0x0483` and Product ID `0x374b` (e.g., STM32 device).
- Basic understanding of Linux kernel modules and USB driver development.

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/usb_reader_module.git
   cd usb_reader_module
