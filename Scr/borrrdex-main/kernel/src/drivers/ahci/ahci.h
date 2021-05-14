#pragma once

#ifndef __cplusplus
#error C++ only
#endif

#include <cstdint>
#include "drivers/device.h"
#include "drivers/gbd.h"

namespace ahci {
    // ATA-ACS8 Commands, pieced together from the Internet since all sources
    // are paywalled
    constexpr uint8_t IDE_COMMAND_PIO_READ      = 0x20;
    constexpr uint8_t IDE_COMMAND_DMA_READ_EX   = 0x25;
    constexpr uint8_t IDE_COMMAND_PIO_WRITE     = 0x30;
    constexpr uint8_t IDE_COMMAND_DMA_WRITE_EX  = 0x35;
    constexpr uint8_t IDE_COMMAND_PACKET        = 0xA1;
    constexpr uint8_t IDE_COMMAND_FLUSH         = 0xE7;
    constexpr uint8_t IDE_COMMAND_IDENTIFY      = 0xEC;

    // AHCI Capabilities Register (Serial ATA AHCI 1.3.1 p.15)
    constexpr uint32_t CAP_S64A_FLAG            = 1 << 31;
    constexpr uint32_t CAP_SNCQ_FLAG            = 1 << 30;
    constexpr uint32_t CAP_SSNTF_FLAG           = 1 << 29;
    constexpr uint32_t CAP_SMPS_FLAG            = 1 << 28;
    constexpr uint32_t CAP_SSS_FLAG             = 1 << 27;
    constexpr uint32_t CAP_SALP_FLAG            = 1 << 26;
    constexpr uint32_t CAP_SAL_FLAG             = 1 << 25;
    constexpr uint32_t CAP_SCLO_FLAG            = 1 << 24;
    constexpr uint8_t  CAP_ISS_OFFSET           = 20;
    constexpr uint8_t  CAP_ISS_MASK             = 0xf;
    constexpr uint32_t CAP_SAM_FLAG             = 1 << 18;
    constexpr uint32_t CAP_SPM_FLAG             = 1 << 17;
    constexpr uint32_t CAP_FBSS_FLAG            = 1 << 16;
    constexpr uint32_t CAP_PMD_FLAG             = 1 << 15;
    constexpr uint32_t CAP_SSC_FLAG             = 1 << 14;
    constexpr uint32_t CAP_PSC_FLAG             = 1 << 13;
    constexpr uint8_t  CAP_NCS_OFFSET           = 8;
    constexpr uint8_t  CAP_NCS_MASK             = 0x1f;
    constexpr uint32_t CAP_CCCS_FLAG            = 1 << 7;
    constexpr uint32_t CAP_EMS_FLAG             = 1 << 6;
    constexpr uint32_t CAP_SXS_FLAG             = 1 << 5;
    constexpr uint8_t  CAP_NP_MASK              = 0xf;

    // Global Host Control Register (Serial ATA AHCI 1.3.1 p. 17)
    constexpr uint32_t GHC_AE_FLAG              = 1 << 31;
    constexpr uint32_t GHC_MSRM_FLAG            = 1 << 2;
    constexpr uint32_t GHC_IE_FLAG              = 1 << 1;
    constexpr uint32_t GHC_HR_FLAG              = 1 << 0;

    // Command Completion Coalescing Control Register (Serial ATA AHCI 1.3.1 p. 19)
    constexpr uint8_t  CCC_TV_OFFSET            = 16;
    constexpr uint16_t CCC_TV_MASK              = 0xffff;
    constexpr uint8_t  CCC_CC_OFFSET            = 8;
    constexpr uint8_t  CCC_CC_MASK              = 0xff;
    constexpr uint8_t  CCC_INT_OFFSET           = 3;
    constexpr uint8_t  CCC_INT_MASK             = 0x1f;
    constexpr uint32_t CCC_EN_FLAG              = 1 << 0;

    // Enclosure Management Location Register (Serial ATA AHCI 1.3.1 p. 20)
    constexpr uint8_t  EMLOC_OFST_OFFSET        = 16;
    constexpr uint16_t EMLOC_OFST_MASK          = 0xffff;
    constexpr uint16_t EMLOC_SZ_MASK            = 0xffff;

    // Enclosure Management Control Register (Serial ATA AHCI 1.3.1 p. 20)
    constexpr uint32_t EMCTL_PM_FLAG            = 1 << 27;
    constexpr uint32_t EMCTL_ALHD_FLAG          = 1 << 26;
    constexpr uint32_t EMCTL_XMT_FLAG           = 1 << 25;
    constexpr uint32_t EMCTL_SMB_FLAG           = 1 << 24;
    constexpr uint32_t EMCTL_SGPIO_FLAG         = 1 << 19;
    constexpr uint32_t EMCTL_SES2_FLAG          = 1 << 18;
    constexpr uint32_t EMCTL_SAFTE_FLAG         = 1 << 17;
    constexpr uint32_t EMCTL_LED_FLAG           = 1 << 16;
    constexpr uint32_t EMCTL_RST_FLAG           = 1 << 9;
    constexpr uint32_t EMCTL_TM_FLAG            = 1 << 8;
    constexpr uint32_t EMCTL_MR_FLAG            = 1 << 0;

    // Extended Capabilities Register (Serial ATA AHCI 1.3.1 p. 21)
    constexpr uint32_t CAP2_DESO_FLAG           = 1 << 5;
    constexpr uint32_t CAP2_SADM_FLAG           = 1 << 4;
    constexpr uint32_t CAP2_SDS_FLAG            = 1 << 3;
    constexpr uint32_t CAP2_APST_FLAG           = 1 << 2;
    constexpr uint32_t CAP2_NVMP_FLAG           = 1 << 1;
    constexpr uint32_t CAP2_BOH_FLAG            = 1 << 0;

    // BIOS/OS Handoff Control / Status Register (Serial ATA AHCI 1.3.1 p. 22)
    constexpr uint32_t BOHC_BB_FLAG             = 1 << 4;
    constexpr uint32_t BOHC_OOC_FLAG            = 1 << 3;
    constexpr uint32_t BOHC_SOOE_FLAG           = 1 << 2;
    constexpr uint32_t BOHC_OOS_FLAG            = 1 << 1;
    constexpr uint32_t BOHC_BOS_FLAG            = 1 << 0;

    // Port Interrupt Status Register (Serial ATA AHCI 1.3.1 p. 23)
    // Note these are identical for the Port Interrupt Enable Register
    constexpr uint32_t PXIS_CPDS_FLAG               = 1 << 31;
    constexpr uint32_t PXIS_TFES_FLAG               = 1 << 30;
    constexpr uint32_t PXIS_HBFS_FLAG               = 1 << 29;
    constexpr uint32_t PXIS_HBDS_FLAG               = 1 << 28;
    constexpr uint32_t PXIS_IFS_FLAG                = 1 << 27;
    constexpr uint32_t PXIS_INFS_FLAG               = 1 << 26;
    constexpr uint32_t PXIS_OFS_FLAG                = 1 << 24;
    constexpr uint32_t PXIS_IPMS_FLAG               = 1 << 23;
    constexpr uint32_t PXIS_PRCS_FLAG               = 1 << 22;
    constexpr uint32_t PXIS_DMPS_FLAG               = 1 << 7;
    constexpr uint32_t PXIS_PCS_FLAG                = 1 << 6;
    constexpr uint32_t PXIS_DPS_FLAG                = 1 << 5;
    constexpr uint32_t PXIS_UFS_FLAG                = 1 << 4;
    constexpr uint32_t PXIS_SBDS_FLAG               = 1 << 3;
    constexpr uint32_t PXIS_DSS_FLAG                = 1 << 2;
    constexpr uint32_t PXIS_PSS_FLAG                = 1 << 1;
    constexpr uint32_t PXIS_DHRS_FLAG               = 1 << 0;

    // Port Command and Status Register (Serial ATA AHCI 1.3.1 p. 26)
    constexpr uint8_t  PXCMD_ICC_OFFSET             = 28;
    constexpr uint8_t  PXCMD_ICC_MASK               = 0xf;
    constexpr uint32_t PXCMD_ASP_FLAG               = 1 << 27;
    constexpr uint32_t PXCMD_ALPE_FLAG              = 1 << 26;
    constexpr uint32_t PXCMD_DLAE_FLAG              = 1 << 25;
    constexpr uint32_t PXCMD_ATAPI_FLAG             = 1 << 24;
    constexpr uint32_t PXCMD_APSTE_FLAG             = 1 << 23;
    constexpr uint32_t PXCMD_FBSCP_FLAG             = 1 << 22;
    constexpr uint32_t PXCMD_ESP_FLAG               = 1 << 21;
    constexpr uint32_t PXCMD_CPD_FLAG               = 1 << 20;
    constexpr uint32_t PXCMD_MPSP_FLAG              = 1 << 19;
    constexpr uint32_t PXCMD_HPCP_FLAG              = 1 << 18;
    constexpr uint32_t PXCMD_PMA_FLAG               = 1 << 17;
    constexpr uint32_t PXCMD_CPS_FLAG               = 1 << 16;
    constexpr uint32_t PXCMD_CR_FLAG                = 1 << 15;
    constexpr uint32_t PXCMD_FR_FLAG                = 1 << 14;
    constexpr uint32_t PXCMD_MPSS_FLAG              = 1 << 13;
    constexpr uint8_t  PXCMD_CCS_OFFSET             = 8;
    constexpr uint8_t  PXCMD_CCS_MASK               = 0xf;
    constexpr uint32_t PXCMD_FRE_FLAG               = 1 << 4;
    constexpr uint32_t PXCMD_CLO_FLAG               = 1 << 3;
    constexpr uint32_t PXCMD_POD_FLAG               = 1 << 2;
    constexpr uint32_t PXCMD_SUD_FLAG               = 1 << 1;
    constexpr uint32_t PXCMD_ST_FLAG                = 1 << 0;
    
    // Port Task File Data Register (Serial ATA AHCI 1.3.1 p. 28)
    constexpr uint8_t  PXTFD_ERR_OFFSET             = 8;
    constexpr uint16_t PXTFD_ERR_MASK               = 0xffff;
    constexpr uint32_t PXTFD_STS_BUSY_FLAG          = 1 << 7;
    constexpr uint32_t PXTFD_STS_DRQ_FLAG           = 1 << 3;
    constexpr uint32_t PXTFD_STS_ERR_FLAG           = 1 << 0;

    // Port Signature Register (Serial ATA AHCI 1.3.1 p. 29)
    constexpr uint8_t  PXSIG_LBA_HIGH_OFFSET        = 24;
    constexpr uint8_t  PXSIG_LBA_HIGH_MASK          = 0xff;
    constexpr uint8_t  PXSIG_LBA_MID_OFFSET         = 16;
    constexpr uint8_t  PXSIG_LBA_MID_MASK           = 0xff;
    constexpr uint8_t  PXSIG_LBA_LOW_OFFSET         = 8;
    constexpr uint8_t  PXSIG_LBA_LOW_MASK           = 0xff;
    constexpr uint8_t  PXSIG_SECCOUNT_MASK          = 0xff;

    // Port Serial ATA Status Register (Serial ATA AHCI 1.3.1 p. 29)
    constexpr uint8_t  SCR0_IPM_OFFSET              = 8;
    constexpr uint8_t  SCR0_IPM_MASK                = 0xf;
    constexpr uint8_t  SCR0_SPD_OFFSET              = 4;
    constexpr uint8_t  SCR0_SPD_MASK                = 0xf;
    constexpr uint8_t  SCR0_DET_MASK                = 0xf;

    // Port Serial ATA Control Register (Serial ATA AHCI 1.3.1 p. 29)
    constexpr uint8_t  SCR2_IPM_OFFSET              = 8;
    constexpr uint8_t  SCR2_IPM_MASK                = 0xf;
    constexpr uint8_t  SCR2_SPD_OFFSET              = 4;
    constexpr uint8_t  SCR2_SPD_MASK                = 0xf;
    constexpr uint8_t  SCR2_DET_MASK                = 0xf;

    // Port Serial ATA Error Register (Serial ATA AHCI 1.3.1 p. 31)
    constexpr uint32_t SCR1_DIAG_X_FLAG             = 1 << 26;
    constexpr uint32_t SCR1_DIAG_F_FLAG             = 1 << 25;
    constexpr uint32_t SCR1_DIAG_T_FLAG             = 1 << 24;
    constexpr uint32_t SCR1_DIAG_S_FLAG             = 1 << 23;
    constexpr uint32_t SCR1_DIAG_H_FLAG             = 1 << 22;
    constexpr uint32_t SCR1_DIAG_C_FLAG             = 1 << 21;
    constexpr uint32_t SCR1_DIAG_B_FLAG             = 1 << 19;
    constexpr uint32_t SCR1_DIAG_W_FLAG             = 1 << 18;
    constexpr uint32_t SCR1_DIAG_I_FLAG             = 1 << 17;
    constexpr uint32_t SCR1_DIAG_N_FLAG             = 1 << 16;
    constexpr uint32_t SCR1_ERR_E_FLAG              = 1 << 11;
    constexpr uint32_t SCR1_ERR_P_FLAG              = 1 << 10;
    constexpr uint32_t SCR1_ERR_C_FLAG              = 1 << 9;
    constexpr uint32_t SCR1_ERR_T_FLAG              = 1 << 8;
    constexpr uint32_t SCR1_ERR_M_FLAG              = 1 << 1;
    constexpr uint32_t SCR1_ERR_I_FLAG              = 1 << 0;

    // Port Serial ATA Notfication Register (Serial ATA AHCI 1.3.1 p. 33)
    constexpr uint16_t SCR4_PMN_MASK                = 0xffff;

    // Port FIS-Based Switching Control (Serial ATA AHCI 1.3.1 p. 33)
    constexpr uint8_t  PXFBS_DWE_OFFSET             = 16;
    constexpr uint8_t  PXFBS_DWE_MASK               = 0xf;
    constexpr uint8_t  PXFBS_ADO_OFFSET             = 12;
    constexpr uint8_t  PXFBS_ADO_MASK               = 0xf;
    constexpr uint8_t  PXFBS_DEV_OFFSET             = 8;
    constexpr uint8_t  PXFBS_DEV_MASK               = 0xf;
    constexpr uint32_t PXFBS_SDE_FLAG               = 1 << 2;
    constexpr uint32_t PXFBS_DEC_FLAG               = 1 << 1;
    constexpr uint32_t PXFBS_EN_FLAG                = 1 << 0;

    // Port Device Sleep Register (Serial ATA AHCI 1.3.1 p. 34)
    constexpr uint8_t  PXDEVSLP_DM_OFFSET           = 25;
    constexpr uint8_t  PXDEVSLP_DM_MASK             = 0xf;
    constexpr uint8_t  PXDEVSLP_DITO_OFFSET         = 15;
    constexpr uint16_t PXDEVSLP_DITO_MASK           = 0x3ff;
    constexpr uint8_t  PXDEVSLP_MDAT_OFFSET         = 10;
    constexpr uint8_t  PXDEVSLP_MDAT_MASK           = 0x1f;
    constexpr uint8_t  PXDEVSLP_DETO_OFFSET         = 2;
    constexpr uint8_t  PXDEVSLP_DETO_MASK           = 0xff;
    constexpr uint32_t PXDEVSLP_DSP_FLAG            = 1 << 1;
    constexpr uint32_t PXDEVSLP_ADSE_FLAG           = 1 << 0;
}

// Arbirtary limit because it doesn't use dynamic memory
constexpr uint8_t SATA_MAX_DRIVES   = 32;

// AHCI Port Register (Serial ATA AHCI 1.3.1 p. 22)
typedef volatile struct ahci_hba_port {
    /*
    The memory address to write commands to
    
    It must be aligned on a 1 KiB boundary and 
    if the CAP_S64A_FLAG flag is not set, it is
    limited to 32-bit
    */
    uint64_t cmd_list_base;

    /*
    The memory address the controller writes responses to

    It must be aligned on a 256 byte boundary and
    if the CAP_S64A_FLAG flag is not set, it is
    limited to 32-bit
    */     
    uint64_t fis_base;

    /*
    Each bit represents an interrupt that has happened.
    The interrupts are defined above as ahci::PXIS_*
    */
    uint32_t interrupt_status;

    /*
    Defines which interrupts are enabled, and uses the
    same interrupt definitions as interrupt_status
    */
    uint32_t interrupt_enable;

    /*
    This value is used to configure the port, and uses
    the ahci::PXCMD_* values above
    */
    uint32_t command;

    uint32_t reserved0;

    /*
    This field basically is a convenience copy of the
    status and error fields from the latest received FIS
    in certain situations
    */
    uint32_t task_file_data;

    /*
    Stores the signature of the device received after the
    first Device to Host Register FIS is received from the
    controller.
    */
    uint32_t signature;

    /*
    SCR0: The current state of the interface and host,
    uses the ahci::SCR0_* values above
    */
    uint32_t sata_status;

    /*
    SCR2: Used to control SATA capabilities, writes will
    result in an action being taken by the host adapter or 
    interface.  Uses the ahci::SCR2_* values above
    */
    uint32_t sata_control;

    /*
    SCR1: Contains diagnostic or error information.
    Uses the ahci::SCR1_* values above
    */
    uint32_t sata_error;

    /*
    SCR3: Each bit represents an active native command
    in the NCQ (native command queue)
    */
    uint32_t sata_active;

    /*
    When a bit in this value is set to 1, the controller will
    read the corresponding entry in the command list and
    execute it.  It will set the bit back to 0 on completion.
    */
    uint32_t command_issue;

    /*
    AHCI 1.1+

    Each of the low 16 bits represent a device with the
    corresponding Port Multiplier number has issued
    a Set Device Bits FIS message with the notification
    flag set
    */
    uint32_t sata_notification;

    /*
    AHCI 1.2+

    Used to control and obtain status for Port Multiplier
    FIS-based switching
    */
    uint32_t fis_switch_control;

    /*
    AHCI 1.3.1+

    Configures the sleeping of the device, with things
    like timeouts (also indicates if the device supports
    sleep in the first place)
    */
    uint32_t port_device_sleep;

    uint32_t reserved1[10];
    uint32_t vendor[4];
} __attribute__((packed)) ahci_hba_port_t;
static_assert(sizeof(ahci_hba_port_t) == 0x80);

// Generic Host Control (Serial ATA AHCI 1.3.1 p. 15)
typedef volatile struct achi_hba_mem {
    /*
    Represents the capabilities of this controller.
    Uses the ahci::CAP_* values above
    */
    uint32_t capabilities;

    /*
    Controls a few global actions for the controller.
    Uses the ahci::GHC_* values above
    */
    uint32_t global_host_control;

    /*
    Each bit represents a port that has an interrupt
    pending.
    */
    uint32_t interrupt_status;

    /*
    Each bit represents a port that is valid for use
    */
    uint32_t port_implemented;

    /*
    The version of the AHCI controller
    */
    uint32_t version;

    /*
    AHCI 1.1+

    Command Completion Coalescing Control, used to
    send interrupts in batches instead of one by one.
    Uses the ahci::CCC_* values above
    */
    uint32_t ccc_ctrl;

    /*
    AHCI 1.1+

    Each bit represents a port that is going to 
    participate in command coalescing
    */
    uint32_t ccc_ports;

    /*
    AHCI 1.1+

    Stores the location (top 16 bits) and size
    (lower 16 bits) of the Enclosure Management
    message buffer.
    */
    uint32_t em_location;

    /*
    AHCI 1.1+

    Configuration for the Enclosure Management
    feature.  Uses the ahci::EMCTL_* values above
    */
    uint32_t em_ctrl;


    /*
    AHCI 1.2+

    The extended capabilities of the controller.
    Uses the ahci::CAP2_* values above
    */
    uint32_t capabilities2;

    /*
    AHCI 1.2+

    Enables the explicit handoff from BIOS to OS, 
    and vice versa.  Uses the ahci::BHOC_* values
    above
    */
    uint32_t bios_handoff_ctrl;

    uint8_t reserved[0xA0-0x2C];
    uint8_t vendor[0x100-0xA0];

    // Up to 32 ports can be used here
    ahci_hba_port_t ports[0];
} __attribute__((packed)) ahci_hba_mem_t;
static_assert(sizeof(ahci_hba_mem_t) == 0x100);

enum ACHIDeviceType {
    Null,
    SATA,
    SEMB,
    PM,
    SATAPI
};

enum SATAInterfaceSpeed : uint8_t {
    GEN1_1_5 = 1,
    GEN2_3_0 = 2,
    GEN3_6_0 = 3
};
