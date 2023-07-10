_HOME = ../../../../

INCDIR = $(_HOME)Sysroot/usr/include/
BUILDDIR := $(_HOME)Sysroot/usr/lib/

ASMC = nasm
CC = kot-x86_64-clang
AR = llvm-ar-14

CFLAGS = -O3 -I../Src -fno-exceptions

SRCDIR = ../Src
OBJDIR = ../Lib

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

INC = $(call rwildcard,$(SRCDIR),*.h)    
SRCC = $(call rwildcard,$(SRCDIR),*.c)
ASSRC = $(call rwildcard,$(SRCDIR),*.asm)   
OBJS += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASSRC))

OBJINC += $(patsubst $(SRCDIR)/%.h, $(INCDIR)/kot-ui/%.h, $(INC))

DIRS = $(wildcard $(SRCDIR)/*)   

main: copy $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@

copy: $(OBJINC)

$(INCDIR)/kot-ui/%.h : $(SRCDIR)/%.h
	@ mkdir -p $(@D)
	cp -p $^ $(@D)

link:
	@ echo !==== LINKING
	$(AR) rcs $(BUILDDIR)libkot-ui.a $(OBJS)
