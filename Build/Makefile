OSNAME = kot
BUILDDIR = ../../bin
LDS = ../Kernel/kernel.ld
CC = gcc
LD = ld

CFLAGS = -ffreestanding -fshort-wchar
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

SRCDIR := ../Kernel
OBJDIR := ../Lib
BUILDDIR = ../Bin

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.c)          
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
DIRS = $(wildcard $(SRCDIR)/*)

kernel: $(OBJS) link
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@
	
link:
	@ echo !==== LINKING
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS)