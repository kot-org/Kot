#pragma once

#ifndef __cplusplus
#error C++ only
#endif

#include "pci/pci.h"
#include "ahci.h"
#include "AHCIDrive.h"
#include <cstdint>

typedef struct {
    uint8_t slot_count;
    uint32_t block_size;
    uint64_t block_count;
} ahci_device_info_t;

class AHCIController {
public:
    static int DetectedControllerCount();
    static AHCIController* GetDetectedController(size_t index);

    AHCIController(pci_device_t* pciLocation);
    ~AHCIController();
    
    bool has_64bit() const { return _abar->capabilities & ahci::CAP_S64A_FLAG; }
    bool has_native_cmd_queue() const { return _abar->capabilities & ahci::CAP_SNCQ_FLAG; }
    bool has_snotif_reg() const { return _abar->capabilities & ahci::CAP_SSNTF_FLAG; }
    bool has_mechanical_pres_switch() const { return _abar->capabilities & ahci::CAP_SMPS_FLAG; }
    bool has_staggered_spinup() const { return _abar->capabilities & ahci::CAP_SSS_FLAG; }
    bool has_agressive_lpm() const { return _abar->capabilities & ahci::CAP_SALP_FLAG; }
    bool has_activity_led() const { return _abar->capabilities & ahci::CAP_SAL_FLAG; }
    bool has_commandlist_override() const { return _abar->capabilities & ahci::CAP_SCLO_FLAG; }
    SATAInterfaceSpeed interface_speed() const { return (SATAInterfaceSpeed)((_abar->capabilities >> ahci::CAP_ISS_OFFSET) & ahci::CAP_ISS_MASK); }
    bool has_legacy_interface() const { return !(_abar->capabilities & ahci::CAP_SAM_FLAG); }
    bool has_port_multi() const { return _abar->capabilities & ahci::CAP_SPM_FLAG; }
    bool has_fis() const { return _abar->capabilities & ahci::CAP_FBSS_FLAG; }
    bool has_pio_multidrq() const { return _abar->capabilities & ahci::CAP_PMD_FLAG; }
    bool has_slumber_state() const { return _abar->capabilities & ahci::CAP_SSC_FLAG; }
    bool has_partial_state() const { return _abar->capabilities & ahci::CAP_PSC_FLAG; }
    uint8_t command_slot_count() const { return (uint8_t)((_abar->capabilities >> ahci::CAP_NCS_OFFSET) & ahci::CAP_NCS_MASK) + 1; }
    bool has_ccc() const { return _abar->capabilities & ahci::CAP_CCCS_FLAG; }
    bool has_enclosure_mgmt() const { return _abar->capabilities & ahci::CAP_EMS_FLAG; }
    bool has_external_sata() const { return _abar->capabilities & ahci::CAP_SXS_FLAG; }
    uint8_t port_count() const { return (uint8_t)(_abar->capabilities & ahci::CAP_NP_MASK) + 1; }
    
    bool achi_only() const { return _abar->global_host_control & ahci::GHC_AE_FLAG; }
    void set_achi_only(bool achi_only);
    bool has_mrsm() const { return _abar->global_host_control & ahci::GHC_MSRM_FLAG; }
    bool interrupts_enabled() const { return _abar->global_host_control & ahci::GHC_IE_FLAG; }
    void set_interrupts_enabled(bool enabled);
    void reset() { _abar->global_host_control |= ahci::GHC_HR_FLAG; }

    uint32_t interrupts_pending() const { return _abar->interrupt_status; }

    uint32_t ports_implemented() const { return _abar->port_implemented; }

    const char* ahci_version();

    uint16_t ccc_timeout() const { return (uint16_t)((_abar->ccc_ctrl >> ahci::CCC_TV_OFFSET) & ahci::CCC_TV_MASK); }
    void set_ccc_timeout(uint16_t timeout);
    uint8_t ccc_required_cmd_count() const { return (uint8_t)((_abar->ccc_ctrl >> ahci::CCC_CC_OFFSET) & ahci::CCC_CC_MASK); }
    void set_ccc_required_cmd_count(uint8_t count);
    uint8_t ccc_interrupt() const { return (uint8_t)((_abar->ccc_ctrl >> ahci::CCC_INT_OFFSET) & ahci::CCC_INT_MASK); }
    bool ccc_enabled() const { return _abar->ccc_ctrl & ahci::CCC_EN_FLAG; }
    void set_ccc_enabled(bool enabled);

    uint32_t ccc_ports() const { return _abar->ccc_ports; }

    uint16_t emloc_offset() const { return (uint16_t)((_abar->em_location >> ahci::EMLOC_OFST_OFFSET) & ahci::EMLOC_OFST_MASK); }
    uint16_t emloc_size() const { return (uint16_t)(_abar->em_location & ahci::EMLOC_SZ_MASK); }

    bool has_em_portmulti() const { return _abar->em_ctrl & ahci::EMCTL_PM_FLAG; }
    bool has_em_activity_led() const { return _abar->em_ctrl & ahci::EMCTL_ALHD_FLAG; }
    bool is_em_transmit_only() const { return _abar->em_ctrl & ahci::EMCTL_TM_FLAG; }
    bool has_em_sgpio_support() const { return _abar->em_ctrl & ahci::EMCTL_SGPIO_FLAG; }
    bool has_em_ses2_support() const { return _abar->em_ctrl & ahci::EMCTL_SES2_FLAG; }
    bool has_em_ledmsg_support() const { return _abar->em_ctrl & ahci::EMCTL_LED_FLAG; }
    bool em_is_resetting() const { return _abar->em_ctrl & ahci::EMCTL_RST_FLAG; }
    void em_reset() { _abar->em_ctrl |= ahci::EMCTL_RST_FLAG; }
    bool em_is_sending() const { return _abar->em_ctrl & ahci::EMCTL_TM_FLAG; }
    void em_send() { _abar->em_ctrl |= ahci::EMCTL_TM_FLAG; }
    bool em_has_message() const { return _abar->em_ctrl & ahci::EMCTL_MR_FLAG; }
    void em_read_finished() { _abar->em_ctrl |= ahci::EMCTL_MR_FLAG; }

    bool has_deso() const { return _abar->capabilities2 & ahci::CAP2_DESO_FLAG; }
    bool has_sadm() const { return _abar->capabilities2 & ahci::CAP2_SADM_FLAG; }
    bool has_device_sleep() const { return _abar->capabilities2 & ahci::CAP2_SDS_FLAG; }
    bool has_apst() const { return _abar->capabilities2 & ahci::CAP2_APST_FLAG; }
    bool has_nvmhci() const { return _abar->capabilities2 & ahci::CAP2_NVMP_FLAG; }
    bool has_bios_handoff() const { return _abar->capabilities2 & ahci::CAP2_BOH_FLAG; }

    bool bios_is_busy() const { return _abar->bios_handoff_ctrl & ahci::BOHC_BB_FLAG; }
    void set_bios_busy(bool busy);
    bool bios_ownership_changed() const { return _abar->bios_handoff_ctrl & ahci::BOHC_OOC_FLAG; }
    void clear_ownership_changed();
    bool bios_smi_on_ooc() const { return _abar->bios_handoff_ctrl & ahci::BOHC_SOOE_FLAG; }
    void set_bios_smi_on_ooc(bool enabled);
    bool bios_os_owned_semaphore() const { return !(_abar->bios_handoff_ctrl & ahci::BOHC_BOS_FLAG); }
    void set_os_owned_semaphore(bool os_owned);

    int drive_count(ACHIDeviceType type) const { return _driveCount; }
    AHCIDrive get_drive(ACHIDeviceType type, int index);

private:
    bool register_achi_disk(ahci_hba_port_t* port, uint32_t index, uint8_t max_command_slots);

    ahci_hba_mem_t* _abar;
    int _driveCount;
    device_t _sata_dev[SATA_MAX_DRIVES];
    gbd_t _sata_gbd[SATA_MAX_DRIVES];
    ahci_device_info_t _sata_devices[SATA_MAX_DRIVES];
    void* _commandPages[SATA_MAX_DRIVES];
    char _version[6];
};