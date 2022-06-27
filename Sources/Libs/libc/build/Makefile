_HOME = ../../../../

INCDIR = $(_HOME)Sysroot/include
BUILDDIR := $(_HOME)Sysroot/lib
BINDIR = $(_HOME)Bin/Modules

OBJDIR := ../lib
SRCDIR := ../src

CC = clang
LD = ld.lld
AS = as
AR = ar

CFLAGS = -I$(INCDIR) -ffreestanding -fshort-wchar -fno-exceptions -fPIC -O2
ASMFLAGS = 

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
   
INC = $(call rwildcard,$(SRCDIR),*.h)      
SRCCPP = $(call rwildcard,$(SRCDIR),*.c)      
ASSRC = $(call rwildcard,$(SRCDIR),*.s)   
OBJS += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCCPP))
OBJS += $(patsubst $(SRCDIR)/%.s, $(OBJDIR)/%.o, $(ASSRC))

OBJINC += $(patsubst $(SRCDIR)/%.h, $(INCDIR)/kot/%.h, $(INC))

DIRS = $(wildcard $(SRCDIR)/*)         

	
main: copy $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@
   
$(OBJDIR)/%.o: $(SRCDIR)/%.s
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(AS) $(ASMFLAGS) $^ -o $@

copy: $(OBJINC)

$(INCDIR)/kot/%.h : $(SRCDIR)/%.h
	@ mkdir -p $(@D)
	cp -p $^ $(@D)

link:
	@ echo !==== LINKING
	@ mkdir -p $(BUILDDIR)
	$(AR) rcs $(BUILDDIR)/libc.a $(OBJS) $(OBJSCPP)

