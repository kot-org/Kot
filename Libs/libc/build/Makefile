PREFIX = ../../../Toolchain/Local/bin/x86_64-kot-
INCDIR = ../../../Sysroot/include/kot
OBJDIR := ../lib
BUILDDIR := ../../../Sysroot/lib
SRCDIR := ../src
BINDIR = ../../../Bin/Modules

CC = $(PREFIX)gcc
LD = $(PREFIX)ld
AS = $(PREFIX)as
AR = $(PREFIX)ar

CFLAGS = -ffreestanding -fshort-wchar -fno-exceptions -fPIC
ASMFLAGS = 


rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
   
INC = $(call rwildcard,$(SRCDIR),*.h)      
SRCCPP = $(call rwildcard,$(SRCDIR),*.c)      
ASSRC = $(call rwildcard,$(SRCDIR),*.s)   
OBJS += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCCPP))
OBJS += $(patsubst $(SRCDIR)/%.s, $(OBJDIR)/%.o, $(ASSRC))

OBJINC += $(patsubst $(SRCDIR)/%.h, $(INCDIR)/%.h, $(INC))

DIRS = $(wildcard $(SRCDIR)/*)         

	
main: copy $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@
   
$(OBJDIR)/%.o: $(SRCDIR)/%.s
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(AS) $(ASMFLAGS) -c $^ -o $@

copy: $(OBJINC)

$(INCDIR)/%.h : $(SRCDIR)/%.h
	@ mkdir -p $(@D)
	cp -p $^ $(@D)

link:
	@ echo !==== LINKING
	$(AR) rcs $(BUILDDIR)/libc.a $(OBJS) $(OBJSCPP)

