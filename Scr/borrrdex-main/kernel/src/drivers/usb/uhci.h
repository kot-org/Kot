#pragma once

#include "pci/pci.h"
#include "usb.h"
#include <cstdint>
#include <cstddef>
#include <vector>

namespace uhci {
    constexpr uint8_t CMD_MAX_PACKET_FLAG           = 1 << 7;
    constexpr uint8_t CMD_CONFIG_DONE_FLAG          = 1 << 6;
    constexpr uint8_t CMD_SOFT_DEBUG_FLAG           = 1 << 5;
    constexpr uint8_t CMD_GLOBAL_RESUME_FLAG        = 1 << 4;
    constexpr uint8_t CMD_GLOBAL_SUSPEND_FLAG       = 1 << 3;
    constexpr uint8_t CMD_GLOBAL_RESET_FLAG         = 1 << 2;
    constexpr uint8_t CMD_HOST_CTRL_RESET_FLAG      = 1 << 1;
    constexpr uint8_t CMD_RUN_STOP_FLAG             = 1 << 0;

    constexpr uint8_t STATUS_CTRL_HALTED_FLAG       = 1 << 5;
    constexpr uint8_t STATUS_CTRL_PROC_ERROR_FLAG   = 1 << 4;
    constexpr uint8_t STATUS_HOST_SYS_ERROR_FLAG    = 1 << 3;
    constexpr uint8_t STATUS_RESUME_DETECT_FLAG     = 1 << 2;
    constexpr uint8_t STATUS_USB_ERR_INT_FLAG       = 1 << 1;
    constexpr uint8_t STATUS_USB_INT_FLAG           = 1 << 0;

    constexpr uint8_t INT_SHORT_PACK_INT_FLAG       = 1 << 3;
    constexpr uint8_t INT_IOC_ENABLE_FLAG           = 1 << 2;
    constexpr uint8_t INT_RESUME_INT_ENABLE_FLAG    = 1 << 1;
    constexpr uint8_t INT_TIMEOUT_INT_ENABLE_FLAG   = 1 << 0;

    constexpr uint16_t FRAME_NUMBER_MASK            = 0x3FF;
    constexpr uint16_t FRAME_BASE_ADDRESS_OFFSET    = 12;

    constexpr uint8_t SOF_TIMING_VALUE_MASK         = 0x3F;

    constexpr uint16_t PORT_STAT_SUSPEND_FLAG        = 1 << 12;
    constexpr uint16_t PORT_STAT_RESET_FLAG          = 1 << 9;
    constexpr uint16_t PORT_STAT_LOSPD_FLAG          = 1 << 8;
    constexpr uint16_t PORT_STAT_RES_DETECT_FLAG     = 1 << 6;
    constexpr uint16_t PORT_STAT_DPLUS_FLAG          = 1 << 5;
    constexpr uint16_t PORT_STAT_DMINUS_FLAG         = 1 << 4;
    constexpr uint16_t PORT_STAT_ENABLE_CHANGE_FLAG  = 1 << 3;
    constexpr uint16_t PORT_STATUS_ENABLE_FLAG       = 1 << 2;
    constexpr uint16_t PORT_STATUS_CONN_CHANGE_FLAG  = 1 << 1;
    constexpr uint16_t PORT_STATUS_CONN_FLAG         = 1 << 0;

    constexpr uint8_t REG_CMD_OFFSET                = 0;
    constexpr uint8_t REG_STATUS_OFFSET             = 2;
    constexpr uint8_t REG_INT_ENABLE_OFFSET         = 4;
    constexpr uint8_t REG_FRAME_NO_OFFSET           = 6;
    constexpr uint8_t REG_FRAME_LIST_ADDR_OFFSET    = 8;
    constexpr uint8_t REG_FRAME_MOD_START_OFFSET    = 12;
    constexpr uint8_t REG_PORT1_STAT_CTRL_OFFSET    = 16;
    constexpr uint8_t REG_PORT2_STAT_CTRL_OFFSET    = 18;

    constexpr uint32_t FRAME_PTR_MASK               = 0xfffffff0;
    constexpr uint8_t  FRAME_PTR_QUEUE_FLAG         = 1 << 1;
    constexpr uint8_t  FRAME_PTR_TERMINATE_FLAG     = 1 << 0;

    constexpr uint32_t LINK_PTR_MASK                = FRAME_PTR_MASK;
    constexpr uint8_t  LINK_PTR_QUEUE_FLAG          = FRAME_PTR_QUEUE_FLAG;
    constexpr uint8_t  LINK_PTR_TERMINATE_FLAG      = FRAME_PTR_TERMINATE_FLAG;
    constexpr uint8_t  LINK_PTR_DEPTH_FIRST_FLAG    = 1 << 3;

    constexpr uint16_t TD_ACTUAL_LENGTH_MASK        = 0x7ff;
    constexpr uint8_t  TD_STATUS_OFFSET             = 16;
    constexpr uint8_t  TD_STATUS_MASK               = 0xff;
    constexpr uint32_t TD_IOC_FLAG                  = 1 << 24;
    constexpr uint32_t TD_ISOCHRONOUS_FLAG          = 1 << 25;
    constexpr uint32_t TD_LOW_SPEED_FLAG            = 1 << 26;
    constexpr uint8_t  TD_ERROR_OFFSET              = 27;
    constexpr uint8_t  TD_ERROR_MASK                = 0x3;
    constexpr uint32_t TD_SHORT_PACKET_FLAG         = 1 << 29;

    constexpr uint8_t  TD_PID_MASK                  = 0xff;
    constexpr uint8_t  TD_DEV_ADDRESS_OFFSET        = 8;
    constexpr uint8_t  TD_DEV_ADDRESS_MASK          = 0x7f;
    constexpr uint8_t  TD_ENDPOINT_OFFSET           = 15;
    constexpr uint8_t  TD_ENDPOINT_MASK             = 0xf;
    constexpr uint32_t TD_DATA_TOGGLE_FLAG          = 1 << 19;
    constexpr uint8_t  TD_MAX_LEN_OFFSET            = 21;
    constexpr uint16_t TD_MAX_LEN_MASK              = 0x7ff;

    constexpr uint8_t  TD_PACKET_SETUP              = 0x2D;
    constexpr uint8_t  TD_PACKET_IN                 = 0x69;
    constexpr uint8_t  TD_PACKET_OUT                = 0xE1;
}

typedef struct {
    uint32_t head_ptr;
    uint32_t element_link_ptr;
    uint32_t previous_link_ptr;
    uint32_t reserved;
} uhci_queue_t;

typedef struct {
    uint32_t link_ptr;
    uint32_t ctrl_status;
    uint32_t packet_hdr;
    uint32_t buf_ptr;
    uint32_t reserved[4];
} uhci_transfer_desc_t;

int uhci_init(pci_device_t* dev);
class UHCIController;
class UHCIDevice;

class UHCIConfiguration {
public:

};

class UHCIString {
public:
    const char* get() { return _value; }

    ~UHCIString();
private:
    friend class UHCIDevice;

    UHCIString(UHCIController& controller, uint8_t deviceAddress, uint16_t port, uint8_t englishAddress, uint8_t stringIndex);

    const char* _value;
};


class UHCIDevice {
public:
    UHCIDevice(UHCIController& parent, usb_device_desc_t desc, uint16_t port, int address);
    ~UHCIDevice();

    const char* usb_version() const;
    uint8_t device_class() const { return _desc.dev_class; }
    uint8_t device_subclass() const { return _desc.subclass; }
    uint8_t device_protocol() const { return _desc.protocol; }
    uint8_t max_packet_size() const { return _desc.max_packet_size; }
    uint16_t vendor_id() const { return _desc.vendor_id; }
    uint16_t product_id() const { return _desc.product_id; }
    const char* device_release() const;
    const char* manufacturer();
    const char* product();
    const char* serial_number();
    uint8_t config_count() const { return _desc.configs; }

    void set_configuration(uint8_t index);

private:
    UHCIController& _parent;
    usb_device_desc_t _desc;
    uint16_t _port;
    int _address;
    int _englishAddress { -1 };

    uint8_t find_english_address();

    UHCIString* _manufacturer { nullptr };
    UHCIString* _product { nullptr };
    UHCIString* _serialNumber { nullptr };
};

class UHCIController {
public:
    static size_t detected_count();
    static UHCIController* get(size_t index);

    UHCIController(uint16_t port, void* stack);
    UHCIController(UHCIController&& other);
    ~UHCIController();

    void discover_devices();
    const UHCIDevice& get_device(size_t index);

private:
    UHCIController(const UHCIController& other) = delete;

    bool insert(uint8_t queue, uhci_queue_t* entry, bool wait);
    void remove(uhci_queue_t* entry);
    bool get_device_desc(usb_device_desc_t* dev_desc, bool ls_device, 
                         int dev_address, int packet_size, int size);
    bool set_address(int dev_address, bool ls_device);

    int get_language_index(uint16_t lcid, int device_address, bool ls_device);
    char* get_string(int device_address, uint16_t port, uint8_t langIndex, int index);
    
    friend class UHCIDevice;
    friend class UHCIString;

    uint16_t _port;
    void* _stack;
    uhci_queue_t *_queues;
    uint8_t _portCount{0};
    std::vector<UHCIDevice> _connectedDevices;
};