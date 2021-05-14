CC = gcc
ASMC = nasm
AR = ar

override CFLAGS := -I include -fno-stack-protector -fno-builtin -I ../kernel/src -nostdinc -g $(CFLAGS)
ASMFLAGS = -g -F dwarf
ARFLAGS = rcs

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.c)
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

libc:	$(OBJS) crt0 crti crtn link

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !== Compiling $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo !== Assembling $^
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@

crt%: crt%.asm
	$(ASMC) $^ -f elf64 -o $(BUILDDIR)/$@.o

link:
	@ echo !== Linking $^
	$(AR) $(ARFLAGS) $(BUILDDIR)/libc.a $(OBJS)

setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(OBJDIR)

strip:
	@objcopy --only-keep-debug $(BUILDDIR)/libc.a $(BUILDDIR)/libc.a.sym
	@strip --strip-debug --strip-unneeded $(BUILDDIR)/libc.a

clean:
	rm -f $(BUILDDIR)/*
	rm -rf $(OBJDIR)