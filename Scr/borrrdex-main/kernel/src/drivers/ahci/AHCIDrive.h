#pragma once

#include "ahci.h"
#include "AHCIPortInterrupts.h"

enum IPMStatus : uint8_t {
    NotPresent,
    Active,
    Partial,
    Slumber = 6,
    DevSleep = 8
};

enum DeviceDetectionStatus : uint8_t {
    NotDetected,
    NoPhyComm,
    PhyComm = 3,
    PhyOffline
};

class AHCIDrive {
public:
    AHCIDrive(ahci_hba_port_t* data);

    void* command_list_base() const { return (void *)_data->cmd_list_base; }
    void* fis_base() const { return (void *)_data->fis_base; }

    AHCIPortInterrupts interrupt_status() const { return AHCIPortInterrupts(_data->interrupt_status); }
    AHCIPortInterrupts interrupts_enabled() const { return AHCIPortInterrupts(_data->interrupt_enable); }
    void set_interrupt_enabled(AHCIPortInterrupts interrupts) { _data->interrupt_enable = (uint32_t)interrupts; }

    //ICCState icc_state() const { return (ICCState)((_data->command >> ahci::PXCMD_ICC_OFFSET) & ahci::PXCMD_ICC_MASK); }
    //void request_icc_state(ICCState state) { _data->command |= ((uint8_t)state & ahci::PXCMD_ICC_MASK) << ahci::PXCMD_ICC_OFFSET; }

    bool aggressive_slumber_partial() const { return _data->command & ahci::PXCMD_ASP_FLAG; }
    void set_aggressive_slumber_partial(bool enabled);

    bool aggressive_link_pm_enabled() const { return _data->command & ahci::PXCMD_ALPE_FLAG; }
    void set_aggressive_link_pm(bool enabled);

    bool led_on_atapi() const { return _data->command & ahci::PXCMD_DLAE_FLAG; }
    void set_led_on_atapi(bool enabled);

    bool is_atapi() const { return _data->command & ahci::PXCMD_ATAPI_FLAG; }
    void set_is_atapi(bool atapi);

    bool auto_partial_to_slumber() const { return _data->command & ahci::PXCMD_APSTE_FLAG; }
    void set_auto_partial_to_slumber(bool enabled);

    bool has_fis_switching_port() const { return _data->command & ahci::PXCMD_FBSCP_FLAG; }
    bool has_ext_sata_port() const { return _data->command & ahci::PXCMD_ESP_FLAG; }
    bool has_cold_presence_detect() const { return _data->command & ahci::PXCMD_CPD_FLAG; }
    bool has_mech_presence_switch() const { return _data->command & ahci::PXCMD_MPSP_FLAG; }
    bool hot_plug_capable() const { return _data->command & ahci::PXCMD_HPCP_FLAG; }

    bool port_multi_attached() const { return _data->command & ahci::PXCMD_PMA_FLAG; }
    void set_port_multi_attached();

    bool has_cold_presence() const { return _data->command & ahci::PXCMD_CPS_FLAG; }
    bool is_cmd_list_running() const { return _data->command & ahci::PXCMD_CR_FLAG; }
    bool is_fis_receive_running() const { return _data->command & ahci::PXCMD_FR_FLAG; }
    bool mech_presence_state() const { return _data->command & ahci::PXCMD_MPSS_FLAG; }
    uint16_t command_slot_val() const { return (uint16_t)((_data->command >> ahci::PXCMD_CCS_OFFSET) & ahci::PXCMD_CCS_MASK); }

    bool fis_receive_enabled() const { return _data->command & ahci::PXCMD_FRE_FLAG; }
    void set_fis_receive(bool enabled);

    bool command_list_override() const { return _data->command & ahci::PXCMD_CLO_FLAG; }
    void set_command_list_override();

    bool device_has_cold_power() const { return _data->command & ahci::PXCMD_POD_FLAG; }
    void set_device_cold_power(bool on);

    bool device_spun_up() const { return _data->command & ahci::PXCMD_SUD_FLAG; }
    void set_device_spin_up(bool on);

    bool started() const { return _data->command & ahci::PXCMD_ST_FLAG; }
    void set_started(bool started);

    IPMStatus ipm_status() const { return (IPMStatus)((_data->sata_status >> ahci::SCR0_IPM_OFFSET) & ahci::SCR0_IPM_MASK); }
private:
    ahci_hba_port_t* _data;
};