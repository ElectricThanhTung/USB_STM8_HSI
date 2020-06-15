
#ifndef __USB_DESC_H__
#define __USB_DESC_H__

#include "usb_def.h"

#define USB_VENDOR_ID           0x1234
#define USB_DEVICE_ID           0x5678
#define USB_DEVICE_VERSION      0x9012

static const unsigned char usb_device_descriptor[] = {
  18, 										// Size of the Descriptor in Bytes
  USBDESCR_DEVICE,								// Device Descriptor (0x01)
  0x10, 
  0x01,										// USB 1.1 = 0x0110, USB 1.0 = 0x0100
  0x00,										//0x00, // Class Code
  0x00,										// Subclass Code
  0x00,										// Protocol Code
  0x08,										// Maximum Packet Size for Zero Endpoint
  (unsigned char)(USB_VENDOR_ID & 0xFF),
  (unsigned char)(USB_VENDOR_ID >> 8), 						// VID
  (unsigned char)(USB_DEVICE_ID & 0xFF),
  (unsigned char)(USB_DEVICE_ID >> 8),						// PID
  (unsigned char)(USB_DEVICE_VERSION & 0xFF),
  (unsigned char)(USB_DEVICE_VERSION >> 8), 					// Device Release Number
  0x01,										// Index of Manufacturer String Descriptor
  0x02,										// Index of Product String Descriptor
  0x03,										// Index of Serial Number String Descriptor
  0x01,										// Number of Possible Configurations
};

#define CUSTOM_HID_EPIN_SIZE    0x08
#define CUSTOM_HID_EPOUT_SIZE   0x08

static const unsigned char usb_report_descriptor[] = {
  0x06, 0x00, 0xFF,                                                             // Usage Page = 0xFF00 (Vendor Defined Page 1)
  0x09, 0x01,                                                                   // Usage (Vendor Usage 1)
  0xA1, 0x01,                                                                   // Collection (Application)
  // Input report
  0x19, 0x01,                                                                   // Usage Minimum
  0x29, 0x40,                                                                   // Usage Maximum
  0x15, 0x00,                                                                   // Logical Minimum (data bytes in the report may have minimum value = 0x00)
  0x26, 0xFF, 0x00,                                                             // Logical Maximum (data bytes in the report may have maximum value = 0x00FF = unsigned 255)
  0x75, 0x08,                                                                   // Report Size: 8-bit field size
  0x95, CUSTOM_HID_EPIN_SIZE,                                                   // Report Count
  0x81, 0x02,                                                                   // Input (Data, Array, Abs)
  // Output report
  0x19, 0x01,                                                                   // Usage Minimum
  0x29, 0x40,                                                                   // Usage Maximum
  0x75, 0x08,                                                                   // Report Size: 8-bit field size
  0x95, CUSTOM_HID_EPOUT_SIZE,                                                  // Report Count
  0x91, 0x02,                                                                   // Output (Data, Array, Abs) 
  0xC0                         						        // END_COLLECTION
};

static const unsigned char usb_configuration_descriptor[] = {
  9, 										// Size of Descriptor in Bytes
  USBDESCR_CONFIG,								// Configuration Descriptor (0x02)
  41, 0,									// Total length in bytes of data returned
  0x01,										// Number of Interfaces
  0x01,										// Value to use as an argument to select this configuration
  0x00,										// Index of String Descriptor describing this configuration
  USBATTR_BUSPOWER,								// D7 Reserved, set to 1. (USB 1.0 Bus Powered), D6 Self Powered, D5 Remote Wakeup, D4..0 Reserved, set to 0.
  USB_CFG_MAX_BUS_POWER / 2,							// Maximum Power Consumption in 2mA units

  // interface descriptor
  9,										// Size of Descriptor in Bytes (9 Bytes)
  USBDESCR_INTERFACE,								// Interface Descriptor (0x04)
  0x00,										// Number of Interface
  0x00,										// Value used to select alternative setting
  0x02,										// Number of Endpoints used for this interface
  0x03,										// Class Code
  0x00,										// Subclass Code
  0x00,										// Protocol Code
  0x00,										// Index of String Descriptor Describing this interface

  // HID descriptor
  9,										// Size of Descriptor in Bytes (9 Bytes)
  USBDESCR_HID,									// HID descriptor (0x21)
  0x01, 
  0x01,										// BCD representation of HID version
  0x00,										// Target country code
  0x01,										// Number of HID Report (or other HID class) Descriptor infos to follow
  0x22,										// Descriptor type: report
  (unsigned char)(ARRAY_LENGHT(usb_report_descriptor) & 0xFF),
  (unsigned char)(ARRAY_LENGHT(usb_report_descriptor) >> 8),		        // total length of report descriptor

  // Endpoint descriptor
  7,										// Size of Descriptor in Bytes (7 Bytes)
  USBDESCR_ENDPOINT,								// Endpoint descriptor (0x05)
  0x81,									        // IN endpoint number 1 (0x81)
  0x03,									        // attrib: Interrupt endpoint
  0x08,
  0x00,									        // maximum packet size
  0x01,									        // POLL INTERVAL (ms)
    
  7,										// Size of Descriptor in Bytes (7 Bytes)
  USBDESCR_ENDPOINT,								// Endpoint descriptor (0x05)
  0x01,									        // OUT endpoint number 1 (0x81)
  0x03,									        // attrib: Interrupt endpoint
  0x08,
  0x00,									        // maximum packet size
  0x01,									        // POLL INTERVAL (ms)
};

static const unsigned char usb_string_descriptor_language[] = {
  4,
  USBDESCR_STRING,
  0x09, 0x04,
};

static const unsigned char usb_string_descriptor_vendor[] = {
  22,
  USBDESCR_STRING,
  'T', 0,
  'h', 0,
  'a', 0,
  'n', 0,
  'h', 0,
  ' ', 0,
  'T', 0,
  0xF9, 0,
  'n', 0,
  'g', 0,
};

static const unsigned char usb_string_descriptor_device[] = {
  26,
  USBDESCR_STRING,
  'S', 0,
  'T', 0,
  'M', 0,
  '8', 0,
  ' ', 0,
  'U', 0,
  'S', 0,
  'B', 0,
  ' ', 0,
  'H', 0,
  'I', 0,
  'D', 0,
};

static const unsigned char usb_string_descriptor_serial[] = {
  22,
  USBDESCR_STRING,
  '1', 0,
  '7', 0,
  '-', 0,
  '0', 0,
  '8', 0,
  '-', 0,
  '2', 0,
  '0', 0,
  '1', 0,
  '9', 0,
};

#endif