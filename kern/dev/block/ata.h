#ifndef __ATA_H__
#define __ATA_H__

/* PC defaults */
#define IDE_BAR0_DEFAULT 0x1f0
#define IDE_BAR1_DEFAULT 0x3f4
#define IDE_BAR2_DEFAULT 0x170
#define IDE_BAR3_DEFAULT 0x374

/* created with ATA7 spec */

/* !PACKET command feature set */
#define ATA_DATA_OFF 0
#define ATA_ERROR_OFF 1
#define ATA_FEATURES_OFF 1
#define ATA_SECTCOUNT_OFF 2
#define ATA_LBAL_OFF 3
#define ATA_LBAM_OFF 4
#define ATA_LBAH_OFF 5
#define ATA_DEV_OFF 6
#define ATA_STS_OFF 7
#define ATA_CMD_OFF 7
#define ATA_CMD_MAX 7
#define ATA_ASTS_OFF (2 + ATA_CMD_MAX)
#define ATA_CTRL_OFF (2 + ATA_CMD_MAX)

/* PACKET command feature set */
#define ATA_INTY_OFF 2
#define ATA_BCL_OFF 4
#define ATA_BCH_OFF 5
#define ATA_DEVSEL_OFF 6

/* 
 * Most registers are command dependent. Below are defines for bitfields
 * which are generic amongst all conditions.
 */
void ata_cmd_write(uint8_t);

#define ATA_DEV_DEV0 (0 << 4) /* Select device 0 */
#define ATA_DEV_DEV1 (1 << 4) /* Select device 1 */

#define ATA_CTRL_HOB (1 << 7) /* TODO: Not sure what this does*/
#define ATA_CTRL_SRST (1 << 2) /* Soft reset both devices */
#define ATA_CTRL_IEN (0 << 1) /* Interrupt enable the selected device */
#define ATA_CTRL_nIEN (1 << 1) /* Interrupt disable the selected device */

#define ATA_STS_BSY (1 << 7)
#define ATA_STS_DRDY (1 << 6)
#define ATA_STS_DFSE (1 << 5)
#define ATA_STS_DRQ (1 << 3)
#define ATA_STS_ERRCHK (1 << 0)

#define ATA_ERROR_ABRT (1 << 2)

typedef uint8_t ata_cmd_t;
#define ATA_CMD_DEVICE_RESET 0x08
#define ATA_CMD_IDENTIFY 0xEC

// The structure is huge and we want it at the bottom...
struct ata_identify_data;

struct ata_channel {
	int id;
	uint8_t devid;
	uint8_t (*read_port8)(struct ata_channel *ata, uint8_t which);
	uint16_t (*read_port16)(struct ata_channel *ata, uint8_t which);
	void (*write_port8)(struct ata_channel *ata, uint8_t which, uint8_t data);
	void (*write_port16)(struct ata_channel *ata, uint8_t which, uint16_t data);
	uint64_t ata_cmd_base;
	uint64_t ata_ctrl_base;
	uint64_t ata_busm_base;
	struct ata_identify_data *identify_data;
	int disabled;
	uint8_t bus;
	uint8_t dev;
	uint8_t func;
};

struct ata_identify_data {
	uint16_t gen_config; // 0
	uint16_t obs1; //1
	uint16_t spec_conf; //2
	uint16_t obs2; //3
	uint16_t retired1[2]; //4
	uint16_t obs3; //6
	uint16_t cflash_rsvd[2]; //7
	uint16_t retired2; //9
	uint16_t serial[10]; //10
	uint16_t retired3[2]; //20
	uint16_t obs4; //22
	uint16_t firmware_rev[4]; //23
	uint16_t model[20]; // 27
	uint16_t rw_multiple; // 47
	uint16_t rsvd1; // 48
	uint16_t caps[2]; // 49
	uint16_t obs5[2]; // 51
	uint16_t field_valid; // 53
	uint16_t obs6[5];	// 54
	uint16_t multi_sector; // 59
	uint16_t total_sectors[2]; //60
	uint16_t obs7; //62
	uint16_t mword_dma; //63
	uint16_t pio_modes; //64
	uint16_t min_mword_ctime; // 65
	uint16_t rec_mword_ctime; // 66
	uint16_t min_pio_ctime_noiordy; //67
	uint16_t min_pio_ctime_iordy; //68
	uint16_t rsvd2[6]; //69
	uint16_t queue_depth; // 75
	uint16_t sata_rsvd[4]; // 76
	uint16_t major; //80
	uint16_t minor; // 81
	uint16_t features_supported[3]; // 82
	uint16_t features_enabled[3]; //85
	uint16_t udma_modes; //88
	uint16_t secure_erase_time; //89
	uint16_t enhanced_secure_erase_time; //90
	uint16_t advanced_pwr_lvl; //91
	uint16_t master_password_rev; // 92
	uint16_t hw_config_test; // 93
	uint16_t auto_accoustic; // 94
	uint16_t stream_min_rqst; // 95
	uint16_t stream_xfer_time_dma; //96
	uint16_t stream_xfer_time_both; //97
	uint16_t stream_perf[2]; //98
	uint16_t max_lba[4]; //100
	uint16_t stream_xfer_time_pio; //104
	uint16_t not_in_spec; // 105
	uint16_t phys_over_logical__size; // 106
	uint16_t interseek_delay; // 107
	uint16_t ww_name[4]; // 108
	uint16_t ww_name2[4]; // 112
	uint16_t rsvd_techsupport; // 116
	uint16_t logical_sector_size[2]; // 117
	uint16_t rsvd3[8]; // 119
	uint16_t removable_media_features; // 127
	uint16_t security_status; // 128
	uint16_t vendor_specific[31]; // 129
	uint16_t cfa_power_mode; //160
	uint16_t cflash_rsvd2[15]; //161
	uint16_t current_media_serial[30]; //176
	uint16_t rsvd4[49]; // 206
	uint16_t integrity;
} __attribute__((packed));
#endif
