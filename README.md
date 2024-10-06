# USB Reader Module

## Project Overview

The USB Reader Module is a Linux kernel module that facilitates the reading of data from a USB device and displays it in user space. It is designed to work with USB devices that have a specific Vendor ID and Product ID.

- **Version:** 1.7
- **Date:** 2024-10-05
- **Author:** [HECHMI Wael] <waelhechmi100@gmail.com>
- **License:** GPL

## File Structure

- **myusb_driver.c:** The main source file containing the USB reader module implementation.

## Installation Instructions

1. **Prepare the Environment:**
   - Ensure that you have the necessary build tools installed, including the Linux kernel headers and build essentials.

2. **Compile the Module:**
   - Navigate to the directory containing `myusb_driver.c` and run the following command:
     ```bash
     make
     ```

3. **Load the Module:**
   - Load the compiled module into the kernel using:
     ```bash
     sudo insmod myusb_driver.ko
     ```

4. **Check Module Status:**
   - Verify that the module has been loaded successfully:
     ```bash
     lsmod | grep myusb_driver
     ```

5. **Create Device File:**
   - The device file will be created automatically as part of the module initialization. You can check for the device file under `/dev/`.

## Usage Instructions

1. **Open the Device:**
   - Use the following command to open the device file (replace `usb_reader` with your device name):
     ```bash
     sudo cat /dev/usb_reader
     ```

2. **Read Data:**
   - The module will read data from the connected USB device and display it in the terminal.

3. **Close the Device:**
   - Close the terminal or stop the `cat` command to release the device.

## Code Explanation

The main functionalities of the module are as follows:

- **USB Device Probe:** The `my_usb_probe` function is called when a matching USB device is connected. It initializes the module and prepares for data reading.

- **USB Device Disconnect:** The `my_usb_disconnect` function handles disconnection events for cleanup purposes.

- **Device Operations:**
  - `dev_open`: Opens the device file and handles concurrent access using a mutex.
  - `dev_read`: Reads data from the USB device and transfers it to user space.
  - `dev_release`: Releases the device when no longer needed.

- **Error Handling:** The module includes error handling mechanisms for memory allocation, file operations, and device status.

## Error Handling

The module provides robust error handling in the following areas:

- **Memory Allocation:** Checks for successful memory allocation using `kmalloc()`.
- **File Operations:** Verifies the success of file opening and reading operations, returning appropriate error codes when failures occur.
- **Mutex Locking:** Ensures that device access is properly synchronized to prevent race conditions.

## Future Improvements

- **Support for Multiple USB Devices:** Enhance the module to handle multiple connected USB devices.
- **Data Processing:** Implement data processing features for the received data, such as parsing and formatting.
- **User Interface:** Develop a graphical user interface (GUI) for easier interaction with the module.

## Contact Information

For any questions or suggestions regarding the USB Reader Module, please contact:

- **Name:** [HECHMI Wael]
- **Email:** waelhechmi100@gmail.com
