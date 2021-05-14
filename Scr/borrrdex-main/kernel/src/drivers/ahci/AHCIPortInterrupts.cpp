#include "AHCIPortInterrupts.h"

static inline void set_enabled(volatile uint32_t* val, uint32_t mask, bool enabled) {
    if(enabled) {
        *val |= mask;
    } else {
        *val &= ~mask;
    }
}

void AHCIPortInterrupts::set_cold_port_detect(bool enabled) {
    set_enabled(&_status, ahci::PXIS_CPDS_FLAG, enabled);
}

void AHCIPortInterrupts::set_task_file_error(bool enabled) {
    set_enabled(&_status, ahci::PXIS_TFES_FLAG, enabled);
}

void AHCIPortInterrupts::set_host_bus_fatal_error(bool enabled) {
    set_enabled(&_status, ahci::PXIS_HBFS_FLAG, enabled);
}

void AHCIPortInterrupts::set_host_bus_data_error(bool enabled) {
    set_enabled(&_status, ahci::PXIS_HBDS_FLAG, enabled);
}

void AHCIPortInterrupts::set_interface_fatal_error(bool enabled){
    set_enabled(&_status, ahci::PXIS_IFS_FLAG, enabled);
}

void AHCIPortInterrupts::set_interface_nonfatal_error(bool enabled) {
    set_enabled(&_status, ahci::PXIS_INFS_FLAG, enabled);
}

void AHCIPortInterrupts::set_overflow(bool enabled) {
    set_enabled(&_status, ahci::PXIS_OFS_FLAG, enabled);
}

void AHCIPortInterrupts::set_incorrect_port_multiplier(bool enabled) {
    set_enabled(&_status, ahci::PXIS_IPMS_FLAG, enabled);
}

void AHCIPortInterrupts::set_phyrdy_changed(bool enabled) {
    set_enabled(&_status, ahci::PXIS_PRCS_FLAG, enabled);
}

void AHCIPortInterrupts::set_device_mech_presence(bool enabled) {
    set_enabled(&_status, ahci::PXIS_DMPS_FLAG, enabled);
}

void AHCIPortInterrupts::set_port_connect_changed(bool enabled){
    set_enabled(&_status, ahci::PXIS_PCS_FLAG, enabled);
}

void AHCIPortInterrupts::set_descriptor_processed(bool enabled){
    set_enabled(&_status, ahci::PXIS_DPS_FLAG, enabled);
}

void AHCIPortInterrupts::set_unknown_fis_interrupt(bool enabled) {
    set_enabled(&_status, ahci::PXIS_UFS_FLAG, enabled);
}

void AHCIPortInterrupts::set_device_bits(bool enabled) {
    set_enabled(&_status, ahci::PXIS_SBDS_FLAG, enabled);
}

void AHCIPortInterrupts::set_dms_setup_fis(bool enabled) {
    set_enabled(&_status, ahci::PXIS_DSS_FLAG, enabled);
}

void AHCIPortInterrupts::set_pio_setup_fis(bool enabled) {
    set_enabled(&_status, ahci::PXIS_PSS_FLAG, enabled);
}

void AHCIPortInterrupts::set_d2h_register_fis(bool enabled) {
    set_enabled(&_status, ahci::PXIS_DHRS_FLAG, enabled);
} 