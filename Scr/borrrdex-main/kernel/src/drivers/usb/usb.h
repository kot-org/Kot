#pragma once
#include "pci/PciDeviceType.h"

namespace usb {
    // https://www.usb.org/defined-class-codes
    static DeviceTypeEntry<uint8_t> ClassList[] = {
        { 0x01, "Audio" },
        { 0x02, "Communications and CDC Control" },
        { 0x03, "Human Interface Device" },
        { 0x05, "Physical Interface Device" },
        { 0x06, "Image" },
        { 0x07, "Printer" },
        { 0x08, "Mass Storage" },
        { 0x09, "Hub" },
        { 0x0A, "CDC-Data" },
        { 0x0B, "Smart Card" },
        { 0x0D, "Content Security" },
        { 0x0E, "Video" },
        { 0x0F, "Personal Healthcare" },
        { 0x10, "Audio/Video Device" },
        { 0x11, "Billboard Device" },
        { 0x12, "USB Type-C Bridge" },
        { 0xDC, "Diagnostic Device" },
        { 0xE0, "Wireless Controller" },
        { 0xEF, "Miscellaneous" }
    };

    // Reset wait times.  USB 2.0 specs, page 153, section 7.1.7.5, paragraph 3
    constexpr uint8_t WAIT_TIME_ROOT_RST            = 50;
    constexpr uint8_t WAIT_TIME_RST                 = 10;
    constexpr uint8_t WAIT_TIME_BETWEEN_RST         = 3;
    constexpr uint8_t WAIT_TIME_RST_RECOVERY        = 10;

    // Device request types (Universal Serial Bus Specification Revision 2.0 p. 250)
    constexpr uint8_t  REQUEST_TYPE_CLR_DEVICE_FEAT = 0x00;
    constexpr uint8_t  REQUEST_TYPE_CLR_INT_FEAT    = 0x01;
    constexpr uint8_t  REQUEST_TYPE_CLR_ENDP_FEAT   = 0x02;
    constexpr uint8_t  REQUEST_TYPE_INTERFACE       = 0x80;
    constexpr uint8_t  REQUEST_TYPE_DESCRIPTOR      = 0x80;
    constexpr uint8_t  REQUEST_TYPE_DEVICE_STAT     = 0x80;
    constexpr uint8_t  REQUEST_TYPE_INT_STAT        = 0x81;
    constexpr uint8_t  REQUEST_TYPE_ENDP_STAT       = 0x82;
    constexpr uint8_t  REQUEST_TYPE_SET_ADDR        = 0x00;
    constexpr uint8_t  REQUEST_TYPE_SET_CONFIG      = 0x00;
    constexpr uint8_t  REQUEST_TYPE_SET_DESC        = 0x00;
    constexpr uint8_t  REQUEST_TYPE_SET_DEVICE_FEAT = 0x00;
    constexpr uint8_t  REQUEST_TYPE_SET_INT_FEAT    = 0x01;
    constexpr uint8_t  REQUEST_TYPE_SET_ENDP_FEAT   = 0x02;
    constexpr uint8_t  REQUEST_TYPE_SET_INTERFACE   = 0x01;
    constexpr uint8_t  REQUEST_TYPE_SYNC_FRAME      = 0x82;

    // The above utilize the following flags
    constexpr uint8_t REQUEST_D2H_FLAG              = 1 << 7;
    constexpr uint8_t REQUEST_TYPE_OFFSET           = 1 << 5;
    constexpr uint8_t REQUEST_TYPE_MASK             = 0x3;
    constexpr uint8_t REQUEST_ENDPOINT_MASK         = 0x1F;

    // Request codes / Descriptor types (Universal Serial Bus Specification Revision 2.0 p. 251)
    constexpr uint8_t  REQUEST_CODE_STATUS          = 0x00;
    constexpr uint8_t  REQUEST_CODE_CLR_FEATURE     = 0x01;
    constexpr uint8_t  REQUEST_CODE_SET_FEATURE     = 0x03;
    constexpr uint8_t  REQUEST_CODE_SET_ADDRESS     = 0x05;
    constexpr uint8_t  REQUEST_CODE_GET_DESCRIPTOR  = 0x06;
    constexpr uint8_t  REQUEST_CODE_SET_DESCRIPTOR  = 0x07;
    constexpr uint8_t  REQUEST_CODE_GET_CONFIG      = 0x08;
    constexpr uint8_t  REQUEST_CODE_SET_CONFIG      = 0x09;
    constexpr uint8_t  REQUEST_CODE_GET_INTERFACE   = 0x0A;
    constexpr uint8_t  REQUEST_CODE_SET_INTERFACE   = 0x0B;
    constexpr uint8_t  REQUEST_CODE_SYNC_FRAME      = 0x0C;

    constexpr uint16_t DESCRIPTOR_TYPE_DEVICE           = 0x0100;
    constexpr uint16_t DESCRIPTOR_TYPE_CONFIG           = 0x0200;
    constexpr uint16_t DESCRIPTOR_TYPE_STRING           = 0x0300;
    constexpr uint16_t DESCRIPTOR_TYPE_INTERFACE        = 0x0400;
    constexpr uint16_t DESCRIPTOR_TYPE_ENDPOINT         = 0x0500;
    constexpr uint16_t DESCRIPTOR_TYPE_DEV_QUAL         = 0x0600;
    constexpr uint16_t DESCRIPTOR_TYPE_OTH_SPD_CONFIG   = 0x0700;
    constexpr uint16_t DESCRIPTOR_TYPE_INTERFACE_PWR    = 0x0800;
}

typedef struct {
    uint8_t length;
    uint8_t type;
    uint16_t release_num;
    uint8_t dev_class;
    uint8_t subclass;
    uint8_t protocol;
    uint8_t max_packet_size;
    uint16_t vendor_id;
    uint16_t product_id;
    uint16_t device_release;
    uint8_t manufacturer;
    uint8_t product;
    uint8_t serial_no;
    uint8_t configs;
} __attribute__((packed)) usb_device_desc_t;

typedef struct {
    uint8_t length;
    uint8_t type;
    uint16_t langid[0];
} __attribute__((packed)) usb_string_desc_t;

typedef struct {
    uint8_t desc_length;
    uint8_t type;
    uint16_t total_length;
    uint8_t num_interfaces;
    uint8_t config_string;
    uint8_t attributes;
    uint8_t max_power;
} __attribute__((packed)) usb_config_desc_t;

typedef struct {
    uint8_t desc_length;
    uint8_t type;
    uint8_t interface_id;
    uint8_t alternate_set;
    uint8_t num_endpoints;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t protocol;
    uint8_t interface_string;
} __attribute__((packed)) usb_interface_desc_t;

typedef struct {
    uint8_t desc_type;
    uint16_t desc_length;
} __attribute__((packed)) usb_desc_type_length_t;

typedef struct {
    uint8_t desc_length;
    uint8_t type;
    uint16_t release;
    uint8_t country_code;
    uint8_t num_descriptors;
    usb_desc_type_length_t descriptors[0];
} __attribute__((packed)) usb_hid_desc_t;

typedef struct {
    uint8_t desc_length;
    uint8_t type;
    uint8_t address;
    uint8_t attributes;
    uint16_t max_packet_size;
    uint8_t interval;
} __attribute__((packed)) usb_endpoint_desc_t;

typedef struct {
    uint8_t request_type;
    uint8_t request;
    uint16_t value;
    uint16_t index;
    uint16_t length;
} __attribute__((packed)) usb_device_req_packet_t;