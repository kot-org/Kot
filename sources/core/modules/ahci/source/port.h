#ifndef _MODULE_AHCI_PORT_H
#define _MODULE_AHCI_PORT_H

#define ATA_SECTOR_SIZE                 0x200
#define ATA_FIS_DRQ                     (1 << 3) // Data transfert resquested
#define ATA_DEV_BUSY                    (1 << 7)  // Port busy

#define HBA_INTERRUPT_STATU_TFE         (1 << 30) // Task File Error
#define HBA_COMMAND_LIST_MAX_ENTRIES    0x20
#define HBA_PRDT_ENTRY_MAX_SIZE         0x1000
#define HBA_MAX_BLOCK_SIZE              0x10000
#define HBA_PRDT_MAX_ENTRIES            HBA_MAX_BLOCK_SIZE / HBA_PRDT_ENTRY_MAX_SIZE
#define HBA_COMMAND_TABLE_SIZE          HBA_PRDT_MAX_ENTRIES * sizeof(hbaprdt_entry_t) + sizeof(hba_command_table_t)
#define HBA_PRDT_ENTRY_ADDRESS_SIZE     PAGE_SIZE
#define HBA_PRDT_ENTRY_SECTOR_COUNT     HBA_PRDT_ENTRY_ADDRESS_SIZE / ATA_SECTOR_SIZE

#define ATA_CMD_TIMEOUT                 1000000

#include <stdint.h>
#include <stddef.h>
#include <controller.h>

#define ATA_COMMAND_READ_PIO            (0x20)
#define ATA_COMMAND_READ_DMA            (0x25)
#define ATA_COMMAND_WRITE_PIO           (0x30)
#define ATA_COMMAND_WRITE_DMA           (0x35)
#define ATA_COMMAND_PACKET              (0xA0)
#define ATA_COMMAND_FLUSH               (0xE7)
#define ATA_COMMAND_IDENTIFY            (0xEC)

#define FIS_TYPE_HOST_TO_DEVICE         (0x27)
#define FIS_TYPE_DEVICE_TO_HOST         (0x34)
#define FIS_TYPE_DMA_ACT                (0x39)
#define FIS_TYPE_DMA_SETUP              (0x41)
#define FIS_TYPE_DATA                   (0x46)
#define FIS_TYPE_BIST                   (0x58)
#define FIS_TYPE_PIO_SETUP              (0x5F)
#define FIS_TYPE_DEV_BITS               (0xA1)


typedef struct{
    uint8_t command_fis_length:5;
    uint8_t atapi:1;
    uint8_t write:1;
    uint8_t prefetchable:1;

    uint8_t reset:1;
    uint8_t bist:1;
    uint8_t clear_busy:1;
    uint8_t reserved0:1;
    uint8_t port_multiplier:4;

    uint16_t prdt_length;
    uint32_t prdb_count;
    uint64_t command_table_base_address;
    uint32_t reserved1[4];
}__attribute__((packed)) hba_command_header_t;

typedef struct{
    uint64_t data_base_address;
    uint32_t reserved0;
    uint32_t byte_count:22;
    uint32_t reserved1:9;
    uint32_t interrupt_on_completion:1;
}__attribute__((packed)) hbaprdt_entry_t;

typedef struct{
    uint8_t command_fis[64];
    uint8_t atapi_command[16];
    uint8_t reserved[48];
    hbaprdt_entry_t prdt_entry[];
}__attribute__((packed)) hba_command_table_t;

typedef struct{
    uint8_t fis_type;

    uint8_t port_multiplier:4;
    uint8_t reserved0:3;
    uint8_t command_control:1;

    uint8_t command;
    uint8_t feature_low;

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device_register;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t feature_high;

    uint8_t count_low;
    uint8_t count_high;
    uint8_t iso_command_completion;
    uint8_t control;

    uint8_t reserved1[4];
}__attribute__((packed)) fis_host_to_device_registers_t;

typedef struct{
    uint8_t fis_type;

    uint8_t port_multiplier:4;

    uint8_t reserved0:2;
    uint8_t interrupt:1;
    uint8_t reserved1:1;
    uint8_t status;
    uint8_t error;

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device_register;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t reserved2;

    uint8_t count_low;
    uint8_t count_high;
    uint8_t reserved3[4];
}__attribute__((packed)) fis_device_to_host_registers_t;

/* identify info */
typedef struct{
    uint8_t reserved0:2;
    uint8_t response_incomplete:1;
    uint8_t reserved1:3;
    uint8_t fixed_device:1;
    uint8_t removable_media:1;
    uint8_t reserved2:7;
    uint8_t device_type:1;        
}__attribute__((packed)) identify_info_general_configuration_t;

typedef struct{
    uint8_t trusted_computing_feature_set_options:1;
    uint16_t reserved0:15;
}__attribute__((packed)) identify_info_trusted_computing_t;

typedef struct{
    uint8_t current_long_physical_sector_alignment:2;
    uint8_t reserved0:6;
    uint8_t dma_supported:1;
    uint8_t lba_supported:1;
    uint8_t iordy_disable:1;
    uint8_t iordy_supported:1;
    uint8_t reserved1:1;
    uint8_t standyby_timer_support:1;
    uint8_t reserved2:2;
    uint16_t reserved3:16;
}__attribute__((packed)) identify_info_capabilities_t;

typedef struct{
    uint8_t zoned_capabilities:2;
    uint8_t non_volatile_write_cache:1;
    uint8_t extended_user_addressable_sectors_supported:1;
    uint8_t device_encrypts_all_user_data:1;
    uint8_t read_zero_after_trim_supported:1;
    uint8_t optional28_bit_commands_supported:1;
    uint8_t ieee1667:1;
    uint8_t download_microcode_dma_supported:1;
    uint8_t set_max_set_password_unlock_dma_supported:1;
    uint8_t write_buffer_dma_supported:1;
    uint8_t read_buffer_dma_supported:1;
    uint8_t device_config_identify_set_dma_supported:1;
    uint8_t lpsaerc_supported:1;
    uint8_t deterministic_read_after_trim_supported:1;
    uint8_t c_fast_spec_supported:1;
}__attribute__((packed)) identify_info_additional_supported_t;

typedef struct{
    uint8_t reserved0:1;
    uint8_t sata_gen1:1;
    uint8_t sata_gen2:1;
    uint8_t sata_gen3:1;
    uint8_t reserved1:4;
    uint8_t ncq:1;
    uint8_t hipm:1;
    uint8_t phy_events:1;
    uint8_t ncq_unload:1;
    uint8_t ncq_priority:1;
    uint8_t host_auto_ps:1;
    uint8_t device_auto_ps:1;
    uint8_t read_log_dma:1;
    uint8_t reserved2:1;
    uint8_t current_speed:3;
    uint8_t ncq_streaming:1;
    uint8_t ncq_queue_mgmt:1;
    uint8_t ncq_receive_send:1;
    uint8_t devsl_pto_reduced_pwr_state:1;
    uint8_t reserved3:8;
}__attribute__((packed)) identify_info_serial_ata_capabilities_t;

typedef struct{
    uint8_t reserved0:1;
    uint8_t non_zero_offsets:1;
    uint8_t dma_setup_auto_activate:1;
    uint8_t dipm:1;
    uint8_t in_order_data:1;
    uint8_t hardware_feature_control:1;
    uint8_t software_settings_preservation:1;
    uint8_t ncq_autosense:1;
    uint8_t devslp:1;
    uint8_t hybrid_information:1;
    uint8_t reserved1:6;
}__attribute__((packed)) identify_info_serial_ata_features_supported_t;

typedef struct{
    uint8_t reserved0:1;
    uint8_t non_zero_offsets:1;
    uint8_t dma_setup_auto_activate:1;
    uint8_t dipm:1;
    uint8_t in_order_data:1;
    uint8_t hardware_feature_control:1;
    uint8_t software_settings_preservation:1;
    uint8_t device_auto_ps:1;
    uint8_t devslp:1;
    uint8_t hybrid_information:1;
    uint8_t reserved1:6;
}__attribute__((packed)) identify_info_serial_ata_features_enabled_t;

typedef struct{
    uint8_t smart_commands:1;
    uint8_t security_mode:1;
    uint8_t removable_media_feature:1;
    uint8_t power_management:1;
    uint8_t reserved1:1;
    uint8_t write_cache:1;
    uint8_t look_ahead:1;
    uint8_t release_interrupt:1;
    uint8_t service_interrupt:1;
    uint8_t device_reset:1;
    uint8_t host_protected_area:1;
    uint8_t obsolete1:1;
    uint8_t write_buffer:1;
    uint8_t read_buffer:1;
    uint8_t nop:1;
    uint8_t obsolete2:1;
    uint8_t download_microcode:1;
    uint8_t dma_queued:1;
    uint8_t cfa:1;
    uint8_t advanced_pm:1;
    uint8_t msn:1;
    uint8_t power_up_in_standby:1;
    uint8_t manual_power_up:1;
    uint8_t reserved2:1;
    uint8_t set_max:1;
    uint8_t acoustics:1;
    uint8_t big_lba:1;
    uint8_t device_config_overlay:1;
    uint8_t flush_cache:1;
    uint8_t flush_cache_ext:1;
    uint8_t word_valid83:2;
    uint8_t smart_error_log:1;
    uint8_t smart_self_test:1;
    uint8_t media_serial_number:1;
    uint8_t media_card_pass_through:1;
    uint8_t streaming_feature:1;
    uint8_t gp_logging:1;
    uint8_t write_fua:1;
    uint8_t write_queued_fua:1;
    uint8_t wwn64_bit:1;
    uint8_t urg_read_stream:1;
    uint8_t urg_write_stream:1;
    uint8_t reserved_for_tech_report:2;
    uint8_t idle_with_unload_feature:1;
    uint8_t word_valid:2;
}__attribute__((packed)) identify_info_command_set_support_t;

typedef struct{
    uint8_t smart_commands:1;
    uint8_t security_mode:1;
    uint8_t removable_media_feature:1;
    uint8_t power_management:1;
    uint8_t reserved1:1;
    uint8_t write_cache:1;
    uint8_t look_ahead:1;
    uint8_t release_interrupt:1;
    uint8_t service_interrupt:1;
    uint8_t device_reset:1;
    uint8_t host_protected_area:1;
    uint8_t obsolete1:1;
    uint8_t write_buffer:1;
    uint8_t read_buffer:1;
    uint8_t nop:1;
    uint8_t obsolete2:1;
    uint8_t download_microcode:1;
    uint8_t dma_queued:1;
    uint8_t cfa:1;
    uint8_t advanced_pm:1;
    uint8_t msn:1;
    uint8_t power_up_in_standby:1;
    uint8_t manual_power_up:1;
    uint8_t reserved2:1;
    uint8_t set_max:1;
    uint8_t acoustics:1;
    uint8_t big_lba:1;
    uint8_t device_config_overlay:1;
    uint8_t flush_cache:1;
    uint8_t flush_cache_ext:1;
    uint8_t resrved3:1;
    uint8_t words119_120_valid:1;
    uint8_t smart_error_log:1;
    uint8_t smart_self_test:1;
    uint8_t media_serial_number:1;
    uint8_t media_card_pass_through:1;
    uint8_t streaming_feature:1;
    uint8_t gp_logging:1;
    uint8_t write_fua:1;
    uint8_t write_queued_fua:1;
    uint8_t wwn64_bit:1;
    uint8_t urg_read_stream:1;
    uint8_t urg_write_stream:1;
    uint8_t reserved_for_tech_report:2;
    uint8_t idle_with_unload_feature:1;
    uint8_t reserved4:2;
}__attribute__((packed)) identify_info_command_set_active_t;

typedef struct{
    uint16_t time_required:15;
    uint8_t extended_time_reported:1;
}__attribute__((packed)) identify_info_normal_security_erase_unit_t;

typedef struct{
    uint16_t time_required:15;
    uint8_t extended_time_reported:1;
}__attribute__((packed)) identify_info_enhanced_security_erase_unit_t;

typedef struct{
    uint8_t logical_sectors_per_physical_sector:4;
    uint8_t reserved0:8;
    uint8_t logical_sector_longer_than512_bytes:1;
    uint8_t multiple_logical_sectors_per_physical_sector:1;
    uint8_t reserved1:2;
}__attribute__((packed)) identify_info_physical_logical_sector_size_t;

typedef struct{
    uint16_t reserved_for_drq_technical_report:1;
    uint8_t write_read_verify:1;
    uint8_t write_uncorrectable_ext:1;
    uint8_t read_write_log_dma_ext:1;
    uint8_t download_microcode_mode3:1;
    uint8_t freefall_control:1;
    uint8_t sense_data_reporting:1;
    uint8_t extended_power_conditions:1;
    uint8_t reserved0:6;
    uint8_t word_valid:2;
}__attribute__((packed)) identify_info_command_set_support_ext_t;

typedef struct{
    uint8_t reserved_for_drq_technical_report:1;
    uint8_t write_read_verify:1;
    uint8_t write_uncorrectable_ext:1;
    uint8_t read_write_log_dma_ext:1;
    uint8_t download_microcode_mode3:1;
    uint8_t freefall_control:1;
    uint8_t sense_data_reporting:1;
    uint8_t extended_power_conditions:1;
    uint8_t reserved0:6;
    uint8_t reserved1:2;
}__attribute__((packed)) identify_info_command_set_active_ext_t;

typedef struct{
    uint8_t security_supported:1;
    uint8_t security_enabled:1;
    uint8_t security_locked:1;
    uint8_t security_frozen:1;
    uint8_t security_count_expired:1;
    uint8_t enhanced_security_erase_supported:1;
    uint8_t reserved0:2;
    uint8_t security_level:1;
    uint8_t reserved1:7;
}__attribute__((packed)) identify_info_security_status_t;

typedef struct{
    uint16_t maximum_current_in_ma:12;
    uint8_t cfa_power_mode1_disabled:1;
    uint8_t cfa_power_mode1_required:1;
    uint8_t reserved0:1;
    uint8_t word160_supported:1;
}__attribute__((packed)) identify_info_cfa_power_mode1_t;

typedef struct{
    uint8_t supports_trim:1;
    uint16_t reserved0:15;
}__attribute__((packed)) identify_info_data_set_management_feature_t;

typedef struct{
    uint8_t supported:1;
    uint8_t reserved0:1;
    uint8_t write_same_suported:1;
    uint8_t error_recovery_control_supported:1;
    uint8_t feature_control_suported:1;
    uint8_t data_tables_suported:1;
    uint8_t reserved1:6;
    uint8_t vendor_specific:4;
}__attribute__((packed)) identify_info_sct_command_transport_t;

typedef struct{
    uint16_t alignment_of_logical_within_physical:14;
    uint8_t word209_supported:1;
    uint8_t reserved0:1;
}__attribute__((packed)) identify_info_block_alignment_t;

typedef struct{
    uint8_t nv_cache_power_mode_enabled:1;
    uint8_t reserved0:3;
    uint8_t nv_cache_feature_set_enabled:1;
    uint8_t reserved1:3;
    uint8_t nv_cache_power_mode_version:4;
    uint8_t nv_cache_feature_set_version:4;
}__attribute__((packed)) identify_info_nv_cache_capabilities_t;

typedef struct{
    uint8_t nv_cache_estimated_time_to_spin_up_in_seconds;
    uint8_t reserved;
}__attribute__((packed)) identify_info_nv_cache_options_t;

typedef struct{
    uint16_t major_version:12;
    uint8_t transport_type:4;
}__attribute__((packed)) identify_info_transport_major_version_t;

typedef struct{
    identify_info_general_configuration_t general_configuration;
    uint16_t number_logical_cylinders;
    uint16_t specific_configurations;
    uint16_t number_logical_head;
    uint16_t reserved0[2];
    uint16_t number_sector_per_track;
    uint16_t vendor_unique0[3]; 
    uint8_t serial_number[20];
    uint16_t reserved1[2];
    uint16_t reserved2; //obsolete
    uint8_t firmware_revision[8];
    uint8_t drive_model_number[40];
    uint8_t maximum_block_transfer; //maximum sectors per interrupt on read multiple and write multiple 
    uint8_t vendor_unique1;
    identify_info_trusted_computing_t trusted_computing;
    identify_info_capabilities_t capabilities;
    uint16_t reserved3[2];
    uint8_t translation_fields_valid:3;
    uint8_t reserved4:3;
    uint8_t free_fall_control_sensitivity;
    uint16_t number_of_current_cylinders;
    uint16_t number_of_current_heads;
    uint16_t current_sectors_per_track;
    uint32_t current_sector_capacity;
    uint8_t current_multi_sector_setting;
    uint8_t multi_sector_setting_valid:1;
    uint8_t reserved5:3;
    uint8_t sanitize_feature_supported:1;
    uint8_t crypto_scramble_ext_command_supported:1;
    uint8_t overwrite_ext_command_supported:1;
    uint8_t block_erase_ext_command_supported:1;
    uint32_t user_addressable_sectors;
    uint16_t reserved6;
    uint8_t multi_word_dma_support;
    uint8_t multi_word_dma_active;
    uint8_t advanced_pio_modes;
    uint8_t reserved7;
    uint16_t minimum_mw_xfer_cycle_time;
    uint16_t recommended_mw_xfer_cycle_time;
    uint16_t minimum_pio_cycle_time;
    uint16_t minimum_pio_cycle_time_iordy;
    identify_info_additional_supported_t additional_supported;
    uint16_t reserved8[5];
    uint16_t queue_depth:5;
    uint16_t reserved9:11;
    identify_info_serial_ata_capabilities_t serial_ata_capabilities;
    identify_info_serial_ata_features_supported_t serial_ata_features_supported;
    identify_info_serial_ata_features_enabled_t serial_ata_features_enabled;
    uint16_t major_revision;
    uint16_t minor_revision;
    identify_info_command_set_support_t command_set_support;
    identify_info_command_set_active_t command_set_active;
    uint8_t ultra_dma_support;
    uint8_t ultra_dma_active;
    identify_info_normal_security_erase_unit_t normal_security_erase_unit;
    identify_info_enhanced_security_erase_unit_t enhanced_security_erase_unit;
    uint8_t current_apm_level;
    uint8_t reserved10;
    uint16_t master_password_id;
    uint16_t hardware_reset_result;
    uint8_t current_acoustic_value;
    uint8_t recommended_acoustic_value;
    uint16_t stream_min_request_size;
    uint16_t streaming_transfer_time_dma;
    uint16_t streaming_access_latency_dmapio;
    uint32_t streaming_perf_granularity;
    uint64_t total_user_addressable_sectors;
    uint16_t streaming_transfer_time;
    uint16_t dsm_cap;
    identify_info_physical_logical_sector_size_t physical_logical_sector_size;
    uint16_t inter_seek_delay;
    uint16_t world_wide_name[8];
    uint16_t reserved_for_tlc_technical_report;
    uint16_t words_per_logical_sector[2];
    identify_info_command_set_support_ext_t command_set_support_ext;
    identify_info_command_set_active_ext_t command_set_active_ext;
    uint16_t reserved_for_expanded_supportand_active[6];
    uint16_t msn_support:2;
    uint16_t reserved11:14;
    identify_info_security_status_t security_status;
    uint16_t reserved12[31];
    identify_info_cfa_power_mode1_t cfa_power_mode1;
    uint16_t reserved_for_cfa_word161[7];
    uint16_t nominal_form_factor:4;
    uint16_t reserved13:12;
    identify_info_data_set_management_feature_t data_set_management_feature;
    uint16_t additional_product_id[4];
    uint16_t reserved_for_cfa_word174[2];
    uint16_t current_media_serial_number[30];
    identify_info_sct_command_transport_t sct_command_transport;
    uint16_t reserved14[2];
    identify_info_block_alignment_t block_alignment;
    uint16_t write_read_verify_sector_count_mode3_only[2];
    uint16_t write_read_verify_sector_count_mode2_only[2];
    identify_info_nv_cache_capabilities_t nv_cache_capabilities;
    uint16_t nv_cache_size_lsw;
    uint16_t nv_cache_size_msw;
    uint16_t nominal_media_rotation_rate;
    uint16_t reserved15;
    identify_info_nv_cache_options_t nv_cache_options;
    uint8_t write_read_verify_sector_count_mode;
    uint8_t reserved16;
    uint16_t reserved17;
    identify_info_transport_major_version_t transport_major_version;
    uint16_t transport_minor_version;
    uint16_t reserved18[6];
    uint64_t extended_number_of_user_addressable_sectors;
    uint16_t min_blocks_per_download_microcode_mode03;
    uint16_t max_blocks_per_download_microcode_mode03;
    uint16_t reserved19[19];
    uint8_t signature;
    uint8_t check_sum;
}__attribute__((packed)) identify_info_t;

typedef struct ahci_device_t{
    void* block_cache;
    size_t block_cache_size;
    size_t alignement;
    spinlock_t lock;
    storage_device_t* storage;
    uint64_t size;
    void* internal_data;
    int (*read)(struct ahci_device_t*, uint64_t, size_t, void*);
    int (*write)(struct ahci_device_t*, uint64_t, size_t, void*);
} ahci_device_t;

typedef struct ahci_sata_device_t {
    ahci_device_t ahci_device;
    hba_port_t* port;
    hba_command_header_t* command_header;
    hba_command_table_t* command_address_table[HBA_COMMAND_LIST_MAX_ENTRIES];
    uint8_t main_slot;
    void* identify_info_physical;
    identify_info_t* identify_info;
} ahci_sata_device_t;

#endif // _MODULE_AHCI_PORT_H