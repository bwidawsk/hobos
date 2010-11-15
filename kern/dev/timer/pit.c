#include <stdint.h>
#include <arch/asm.h>
#include <arch/irq.h>

#ifdef ONLY_8253
#error No 8253 support because of access mode
#endif

#define PIT_OSC_FREQ 1193182
#define PIT_MINIMUM_NS 838 /* (1/1193182) */
#define PIT_MAX_ONESHOT (838 * 65535) /* roughly .05 seconds */
#define PIT_BCD_MODE_OFF 0
#define PIT_OP_MODE_OFF 1
#define PIT_ACC_MODE_OFF 4
#define PIT_CHAN_OFF 6

#define PIT_4BCD_MODE 1
#define PIT_16BIT_MODE 0

#define PIT_MODE0 (0 << PIT_OP_MODE_OFF) /* interrupt on terminal count */
//#define PIT_MODE1 (1 << PIT_OP_MODE_OFF) /* hardware re-triggerable one-shot (valid only for channel 2) */
#define PIT_MODE2 (2 << PIT_OP_MODE_OFF) /* rate generator */
#define PIT_MODE3 (3 << PIT_OP_MODE_OFF) /* square wave generator */
#define PIT_MODE4 (4 << PIT_OP_MODE_OFF) /* software triggered strobe */
//#define PIT_MODE5 (5 << PIT_OP_MODE_OFF) /* hardware triggered strobe  (valid only for channel 2) */

#define PIT_ACC_LATCH (0 << PIT_ACC_MODE_OFF ) /* Latch count value command */
#define PIT_ACC_LO (1 << PIT_ACC_MODE_OFF ) /* Access mode: lobyte only */
#define PIT_ACC_HI (2 << PIT_ACC_MODE_OFF ) /* Access mode: hibyte only */
#define PIT_ACC_BOTH (3 << PIT_ACC_MODE_OFF ) /* Access mode: lobyte/hibyte */

#define PIT_CHAN0 (0 << PIT_CHAN_OFF)
#define PIT_CHAN1 (1 << PIT_CHAN_OFF)
#define PIT_CHAN2 (2 << PIT_CHAN_OFF)
#define PIT_CHAN_BOTH (3 << PIT_CHAN_OFF)

#define PIT_ONESHOT  (PIT_CHAN0 | PIT_ACC_BOTH | PIT_MODE0 | PIT_16BIT_MODE)

#define PIT_DATA_PORT 0x40
#define PIT_CMD_PORT 0x43
#define PIT_CMD_WRITE(u8) outb((uint8_t)u8, PIT_CMD_PORT)
#define PIT_DATA_WRITE(u8) outb((uint8_t)u8, PIT_DATA_PORT)
#define PIT_DATA_READ inb(PIT_DATA_PORT)

// TODO: This would differ for 8253
#define PIT_RELOAD_WRITE(u16) \
	PIT_DATA_WRITE(u16 & 0xFF); \
	PIT_DATA_WRITE(u16 >> 8)

#define PIT_READ_CURRENT \
	PIT_CMD_WRITE(PIT_CHAN0 | PIT_ACC_LATCH) \
	((uint16_t)PIT_DATA_READ | (PIT_DATA_READ << 8))

#define PIT_SETUP_ONESHOT(timeout) \
	PIT_CMD_WRITE(PIT_ONESHOT); \
	PIT_RELOAD_WRITE(timeout)

#define PIT_IRQ 0
volatile uint64_t pit_ticks = 0;
int
pit_timer_irq(void *data) {
	printf("timer interrupt\n");
	atomic_add_64(&pit_ticks, 1);
	return 0;
}

void
pit_test() {
	register_irq(0, pit_timer_irq, 0);
	// enable below for interrupt testing only!
	sti();
	PIT_SETUP_ONESHOT(1);
	uint8_t nums[4];
	nums[0] = PIT_DATA_READ;
	nums[1] = PIT_DATA_READ;
	nums[2] = PIT_DATA_READ;
	nums[3] = PIT_DATA_READ;
	printf("now = %d\n", nums[0]);
	printf("now = %d\n", nums[1]);
	PIT_SETUP_ONESHOT(1);
	printf("now = %d\n", nums[2]);
	printf("now = %d\n", nums[3]);
	printf("pit_ticks = %d", pit_ticks);
}
