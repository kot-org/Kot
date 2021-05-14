#include <sys/defs.h>
#include <sys/gdt.h>

/* adapted from Chris Stones, shovelos */

#define GDT_CS        (0x00180000000000)  /*** code segment descriptor ***/
#define GDT_DS        (0x00100000000000)  /*** data segment descriptor ***/

#define C             (0x00040000000000)  /*** conforming ***/
#define DPL0          (0x00000000000000)  /*** descriptor privilege level 0 ***/
#define DPL1          (0x00200000000000)  /*** descriptor privilege level 1 ***/
#define DPL2          (0x00400000000000)  /*** descriptor privilege level 2 ***/
#define DPL3          (0x00600000000000)  /*** descriptor privilege level 3 ***/
#define P             (0x00800000000000)  /*** present ***/
#define L             (0x20000000000000)  /*** long mode ***/
#define D             (0x40000000000000)  /*** default op size ***/
#define W             (0x00020000000000)  /*** writable data segment ***/

#define MAX_GDT 32

struct tss_t {
  uint32_t reserved_0;
  void *rsp0;
  void *rsp1;
  void *rsp2;
  uint64_t reserved_1;
  void *ist1;
  void *ist2;
  void *ist3;
  void *ist4;
  void *ist5;
  void *ist6;
  void *ist7;
  uint64_t reserved_2;
  uint16_t reserved_3;
  uint16_t iomap_base;
}__attribute__((packed));

struct sys_segment_descriptor {
  uint64_t sd_lolimit :16;/* segment extent (lsb) */
  uint64_t sd_lobase :24; /* segment base address (lsb) */
  uint64_t sd_type :5; /* segment type */
  uint64_t sd_dpl :2; /* segment descriptor priority level */
  uint64_t sd_p :1; /* segment descriptor present */
  uint64_t sd_hilimit :4; /* segment extent (msb) */
  uint64_t sd_xx1 :3; /* avl, long and def32 (not used) */
  uint64_t sd_gran :1; /* limit granularity (byte/page) */
  uint64_t sd_hibase :40; /* segment base address (msb) */
  uint64_t sd_xx2 :8; /* reserved */
  uint64_t sd_zero :5; /* must be zero */
  uint64_t sd_xx3 :19; /* reserved */
}__attribute__((packed));

struct gdtr_t {
  uint16_t size;
  uint64_t addr;
}__attribute__((packed));

static uint64_t gdt[MAX_GDT] = {
  0, /*** NULL descriptor ***/
  GDT_CS | P | DPL0 | L, /*** kernel code segment descriptor ***/
  GDT_DS | P | W | DPL0, /*** kernel data segment descriptor ***/
  0,                     /*** user data segment descriptor (32-bit) ***/
  GDT_DS | P | W | DPL3, /*** user data segment descriptor ***/
  GDT_CS | P | DPL3 | L, /*** user code segment descriptor (64-bit) ***/
  0, 0, /*** TSS ***/
};
static struct gdtr_t gdtr = { sizeof(gdt), (uint64_t)gdt };
static struct tss_t tss;

void _x86_64_asm_lgdt(struct gdtr_t *gdtr, uint64_t cs_idx, uint64_t ds_idx);
void _x86_64_asm_ltr(uint64_t tss_idx);
void init_gdt() {
  struct sys_segment_descriptor *sd = (struct sys_segment_descriptor*)&gdt[6]; // 7th&8th entry in GDT
  sd->sd_lolimit = sizeof(struct tss_t) - 1;
  sd->sd_lobase = ((uint64_t)&tss);
  sd->sd_type = 9; // TSS
  sd->sd_dpl = 0;
  sd->sd_p = 1;
  sd->sd_hilimit = 0;
  sd->sd_gran = 0;
  sd->sd_hibase = ((uint64_t)&tss) >> 24;

  _x86_64_asm_lgdt(&gdtr, 8, 16);
  _x86_64_asm_ltr(0x30);
}

void set_tss_rsp(void *rsp) {
  tss.rsp0 = rsp;
}
