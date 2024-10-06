/**
 * @file myusb_driver.c
 * @author 
 * @brief USB Reader module for reading data from USB and displaying it in user space.
 * @version 1.7
 * @date 2024-10-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/mutex.h> 

/**
 * @brief Device name and class name definition for USB reader.
 * 
 */
#define DEVICE_NAME "usb_reader"
#define CLASS_NAME  "usb"
#define USB_DEVICE_PATH "/dev/ttyACM1"

/**
 * @brief USB Vendor ID and Product ID definitions.
 * 
 */
#define VENDOR_ID 0x0483
#define PRODUCT_ID 0x374b

/**
 * @brief Major number and class/device structures for USB reader device.
 * 
 */
static int major_number;
static struct class*  usb_reader_class  = NULL;
static struct device* usb_reader_device = NULL;

static int device_open = 0;
static DEFINE_MUTEX(my_mutex); // Define and initialize the mutex

/**
 * @brief Macro for printing USB interface descriptor details.
 * 
 * @param i USB interface descriptor structure.
 */
#define PRINT_USB_INTERFACE_DESCRIPTOR( i )                         \
{                                                                   \
    pr_info("USB_INTERFACE_DESCRIPTOR:\n");                         \
    pr_info("-----------------------------\n");                     \
    pr_info("bLength: 0x%x\n", i.bLength);                          \
    pr_info("bDescriptorType: 0x%x\n", i.bDescriptorType);          \
    pr_info("bInterfaceNumber: 0x%x\n", i.bInterfaceNumber);        \
    pr_info("bAlternateSetting: 0x%x\n", i.bAlternateSetting);      \
    pr_info("bNumEndpoints: 0x%x\n", i.bNumEndpoints);              \
    pr_info("bInterfaceClass: 0x%x\n", i.bInterfaceClass);          \
    pr_info("bInterfaceSubClass: 0x%x\n", i.bInterfaceSubClass);    \
    pr_info("bInterfaceProtocol: 0x%x\n", i.bInterfaceProtocol);    \
    pr_info("iInterface: 0x%x\n", i.iInterface);                    \
    pr_info("\n");                                                  \
}

/**
 * @brief Macro for printing USB endpoint descriptor details.
 * 
 * @param e USB endpoint descriptor structure.
 */
#define PRINT_USB_ENDPOINT_DESCRIPTOR( e )                          \
{                                                                   \
    pr_info("USB_ENDPOINT_DESCRIPTOR:\n");                          \
    pr_info("------------------------\n");                          \
    pr_info("bLength: 0x%x\n", e.bLength);                          \
    pr_info("bDescriptorType: 0x%x\n", e.bDescriptorType);          \
    pr_info("bEndPointAddress: 0x%x\n", e.bEndpointAddress);        \
    pr_info("bmAttributes: 0x%x\n", e.bmAttributes);                \
    pr_info("wMaxPacketSize: 0x%x\n", e.wMaxPacketSize);            \
    pr_info("bInterval: 0x%x\n", e.bInterval);                      \
    pr_info("\n");                                                  \
}

/**
 * @brief USB device ID table for supported devices.
 * 
 */
static struct usb_device_id usb_dev_table [] = {
    { USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
    {},
};
MODULE_DEVICE_TABLE(usb, usb_dev_table);

/**
 * @brief USB probe function, called when a matching USB device is connected.
 * 
 * @param intf Pointer to the USB interface structure.
 * @param id Pointer to the USB device ID structure.
 * @return int 0 if successful, error code otherwise.
 */
static int my_usb_probe(struct usb_interface *intf, const struct usb_device_id *id) {
    printk("my_usb_devdrv - Probe Function\n");
    printk("Device connected\n");

    return 0;
}

/**
 * @brief USB disconnect function, called when the USB device is disconnected.
 * 
 * @param intf Pointer to the USB interface structure.
 */
static void my_usb_disconnect(struct usb_interface *intf) {
    printk("my_usb_devdrv - Disconnect Function\n");
    printk("Device disconnected\n");
}

/**
 * @brief USB driver structure defining the probe and disconnect callbacks.
 * 
 */
static struct usb_driver my_usb_driver = {
    .name = "my_usb_devdrv",
    .id_table = usb_dev_table,
    .probe = my_usb_probe,
    .disconnect = my_usb_disconnect,
};

/**
 * @brief Opens the device when a user process attempts to open the device file.
 * 
 * @param inodep Pointer to the inode structure.
 * @param filep Pointer to the file structure.
 * @return int 0 if successful, error code otherwise.
 */
static int dev_open(struct inode *inodep, struct file *filep) {
    mutex_lock(&my_mutex); // Lock the mutex

    if (device_open) {
        mutex_unlock(&my_mutex); // Unlock before returning
        return -EBUSY;
    }
    device_open++;
    printk(KERN_INFO "Open Device file ...\n");

    mutex_unlock(&my_mutex); // Unlock after updating
    return 0;
}

/**
 * @brief Releases the device when a user process closes the device file.
 * 
 * @param inodep Pointer to the inode structure.
 * @param filep Pointer to the file structure.
 * @return int 0 if successful.
 */
static int dev_release(struct inode *inodep, struct file *filep) {
    mutex_lock(&my_mutex); // Lock the mutex
    device_open--;
    printk(KERN_INFO "Close Device file ...\n");
    mutex_unlock(&my_mutex); // Unlock after updating

    return 0;
}

/**
 * @brief Reads data from the USB device and transfers it to user space.
 * 
 * @param filep Pointer to the file structure.
 * @param buffer Buffer in user space to store the data.
 * @param len Length of data to read.
 * @param offset Pointer to file offset.
 * @return ssize_t Number of bytes read if successful, error code otherwise.
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    struct file *usb_file;
    char *kernel_buffer;
    loff_t pos = 0;
    int bytes_read;

    // Allocate a kernel buffer for reading data
    kernel_buffer = kmalloc(len, GFP_KERNEL);
    if (!kernel_buffer) {
        return -ENOMEM;
    }

    // Open the USB device
    usb_file = filp_open(USB_DEVICE_PATH, O_RDONLY, 0);
    if (IS_ERR(usb_file)) {
        kfree(kernel_buffer);
        printk(KERN_ALERT "Failed to open %s\n", USB_DEVICE_PATH);
        return PTR_ERR(usb_file);
    }

    // Read data from the USB device
    bytes_read = kernel_read(usb_file, kernel_buffer, len - 1, &pos);
    
    filp_close(usb_file, NULL);

    if (bytes_read < 0) {
        kfree(kernel_buffer);
        printk(KERN_ALERT "Failed to read from %s\n", USB_DEVICE_PATH);
        return bytes_read;
    }

    kernel_buffer[bytes_read] = '\0'; // Null-terminate the string

    // Copy data to user space
    if (copy_to_user(buffer, kernel_buffer, bytes_read)) {
        kfree(kernel_buffer);
        return -EFAULT; 
    }
    printk(KERN_INFO "Read Data ...\n");
    kfree(kernel_buffer);
    return bytes_read; // Return the number of bytes read
}

/**
 * @brief File operations structure for the USB reader device.
 * 
 */
static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .release = dev_release,
};

/**
 * @brief Module initialization function. Registers the USB driver and character device.
 * 
 * @return int 0 if successful, error code otherwise.
 */
static int __init usb_reader_init(void) {
    int result;
    printk("my_usb_devdrv - Init Function\n");
    result = usb_register(&my_usb_driver);
    if (result) {
        printk("my_usb_devdrv - Error during register!\n");
        return -result;
    }

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }

    usb_reader_class = class_create(CLASS_NAME);
    if (IS_ERR(usb_reader_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(usb_reader_class);
    }

    usb_reader_device = device_create(usb_reader_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(usb_reader_device)) {
        class_destroy(usb_reader_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(usb_reader_device);
    }

    mutex_init(&my_mutex); // Initialize the mutex
    return 0;
}

/**
 * @brief Module exit function. Unregisters the USB driver and character device.
 * 
 */
static void __exit usb_reader_exit(void) {
    device_destroy(usb_reader_class, MKDEV(major_number, 0));
    class_unregister(usb_reader_class);
    class_destroy(usb_reader_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    usb_deregister(&my_usb_driver);
    printk("my_usb_devdrv - Exit Function\n");
}

module_init(usb_reader_init);
module_exit(usb_reader_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("waelhechmi100@gmail.com");
MODULE_DESCRIPTION("USB Reader Module");
MODULE_VERSION("1.7");
