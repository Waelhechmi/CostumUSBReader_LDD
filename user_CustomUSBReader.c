/**
 * @file usb_reader_user.c
 * @author 
 * @brief User-space program to read data from USB device via the custom kernel module.
 * @version 0.1
 * @date 2024-10-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


#define DEVICE_PATH "/dev/usb_reader"
#define BUFFER_SIZE 1024

/**
 * @brief Main function to open the device, read data, and display it on the terminal.
 * 
 * @return int Exit status.
 */
int main() {
    int fd;
    ssize_t bytes_read;
    char buffer[BUFFER_SIZE];

    // Open the device file
    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device file");
        return EXIT_FAILURE;
    }

    // Read data from the device
    bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read < 0) {
        perror("Failed to read from the device");
        close(fd);
        return EXIT_FAILURE;
    }

    // Null-terminate the buffer and display the data
    buffer[bytes_read] = '\0';
    printf("click Ctrl+C for quit:\n");
    while(1)
    {
    printf("Data read from USB device: %s\n", buffer);
    sleep(1);
    }

    // Close the device file
    close(fd);

    return EXIT_SUCCESS;
}
