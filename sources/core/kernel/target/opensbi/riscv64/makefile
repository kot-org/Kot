cyan = /bin/echo -e "\x1b[36m\#\# $1\x1b[0m"

# Project Root
override HOME = ../../..

# Project Resources
SOURCE = $(HOME)/source
BIN = bin
LIB = lib

LDS = kernel.ld

prepare:
	@ rm -rf $(BIN)
	@ rm -rf $(LIB)
	@ mkdir -m 777 -p $(BIN)
	@ mkdir -m 777 -p $(LIB)

# Tools
CC = clang-16
LD = ld.lld-16

# Tools Config
CFLAGS = -I$(SOURCE) \
		 -ffreestanding \
		 --target=riscv64-unknown-none-elf \
		 -nostdlib \
		 -Werror \
		 -Wstrict-prototypes

ASMFLAGS = -I$(SOURCE) \
		   --target=riscv64-unknown-none-elf  \
		   -nostdlib

LDFLAGS = -T $(LDS) \
		 -nostdlib \
		 -static \
		 --no-dynamic-linker \
		 -Bsymbolic \
		 --no-relax \
		 -z max-page-size=0x1000

# Recursive Wild Card
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Source Files
C_SRC = $(SOURCE)/kernel.c # kernel starting point
C_SRC += $(SOURCE)/boot/opensbi.c # bootprotocol specific
C_SRC += $(wildcard $(SOURCE)/boot/opensbi/*.c) # bootprotocol specific
C_SRC += $(call rwildcard,$(SOURCE)/boot/opensbi/riscv64,*.c) # bootprotocol specific & arch specific
C_SRC += $(call rwildcard,$(SOURCE)/arch/riscv64,*.c) # arch specific
C_SRC += $(call rwildcard,$(SOURCE)/global,*.c) # global implemented
C_SRC += $(call rwildcard,$(SOURCE)/lib,*.c) # library

ASM_SRC = $(call rwildcard,$(SOURCE)/boot/opensbi/riscv64,*.s) # bootprotocol specific & arch specific
ASM_SRC += $(call rwildcard,$(SOURCE)/arch/riscv64,*.s) # arch specific

OBJS = $(patsubst $(SOURCE)/%.c,$(LIB)/%_c.o,$(C_SRC))
OBJS += $(patsubst $(SOURCE)/%.s,$(LIB)/%_s.o,$(ASM_SRC))

# Target
$(LIB)/%_c.o: $(SOURCE)/%.c
	@ mkdir -m 777 -p $(@D)
	@ $(call cyan,"$(subst ../,,$^)")
	@ $(CC) $(CFLAGS) -c $^ -o $@

$(LIB)/%_s.o: $(SOURCE)/%.s
	@ mkdir -m 777 -p $(@D)
	@ $(call cyan,"$(subst ../,,$^)")
	@ $(CC) $(ASMFLAGS) -c $^ -o $@

link:
	@ $(LD) $(LDFLAGS) -o $(BIN)/kernel.elf $(OBJS)

kernel.elf: prepare $(OBJS) link
