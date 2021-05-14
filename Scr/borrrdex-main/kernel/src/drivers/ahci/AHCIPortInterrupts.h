#pragma once

#include "ahci.h"

class AHCIPortInterrupts {
public:
    AHCIPortInterrupts()
        :AHCIPortInterrupts(0)
    { }

    AHCIPortInterrupts(uint32_t status)
        :_status(status)
    { }

    bool cold_port_detect() const { return _status & ahci::PXIS_CPDS_FLAG; }
    bool task_file_error() const { return _status & ahci::PXIS_TFES_FLAG; }
    bool host_bus_fatal_err() const { return _status & ahci::PXIS_HBFS_FLAG; }
    bool host_bus_data_err() const { return _status & ahci::PXIS_HBDS_FLAG; }
    bool interface_fatal_err() const { return _status & ahci::PXIS_IFS_FLAG; }
    bool interface_nonfatal_err() const { return _status & ahci::PXIS_INFS_FLAG; }
    bool overflow() const { return _status & ahci::PXIS_OFS_FLAG; }
    bool incorrect_port_multiplier() const { return _status & ahci::PXIS_IPMS_FLAG; }
    bool phyrdy_changed() const { return _status & ahci::PXIS_PRCS_FLAG; }
    bool device_mech_presence() const { return _status & ahci::PXIS_DMPS_FLAG; }
    bool port_connect_changed() const { return _status & ahci::PXIS_PCS_FLAG; }
    bool descriptor_processed() const { return _status & ahci::PXIS_DPS_FLAG; }
    bool unknown_fis_interrupt() const { return _status & ahci::PXIS_UFS_FLAG; }
    bool set_device_bits() const { return _status & ahci::PXIS_SBDS_FLAG; }
    bool dms_setup_fis() const { return _status & ahci::PXIS_DSS_FLAG; }
    bool pio_setup_fis() const { return _status & ahci::PXIS_PSS_FLAG; }
    bool d2h_register_fis() const { return _status & ahci::PXIS_DHRS_FLAG; }

    void set_cold_port_detect(bool enabled);
    void set_task_file_error(bool enabled);
    void set_host_bus_fatal_error(bool enabled);
    void set_host_bus_data_error(bool enabled);
    void set_interface_fatal_error(bool enabled);
    void set_interface_nonfatal_error(bool enabled);
    void set_overflow(bool enabled);
    void set_incorrect_port_multiplier(bool enabled);
    void set_phyrdy_changed(bool enabled);
    void set_device_mech_presence(bool enabled);
    void set_port_connect_changed(bool enabled);
    void set_descriptor_processed(bool enabled);
    void set_unknown_fis_interrupt(bool enabled);
    void set_device_bits(bool enabled);
    void set_dms_setup_fis(bool enabled);
    void set_pio_setup_fis(bool enabled);
    void set_d2h_register_fis(bool enabled);
    
    explicit operator uint32_t() { return _status; }
private:
    uint32_t _status;
};