_HOME = ../../../../

INCDIR = $(_HOME)Sysroot/Include
BUILDDIR := $(_HOME)Sysroot/Lib
BINDIR = $(_HOME)Bin/Modules

OBJDIR := ../Lib
SRCDIR := ../Src

CC = clang-14
ASMC = nasm
AR = llvm-ar-14

CFLAGS = -I$(INCDIR) --target=x86_64-unknown-none-elf -fno-rtti -ffreestanding -fshort-wchar -fno-exceptions -fPIC -O3
ASMFLAGS = 

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
   
INC = $(call rwildcard,$(SRCDIR),*.h)      
SRCC = $(call rwildcard,$(SRCDIR),*.c)      
ASSRC = $(call rwildcard,$(SRCDIR),*.asm)   
OBJS += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASSRC))

OBJINC += $(patsubst $(SRCDIR)/%.h, $(INCDIR)/kot/%.h, $(INC))

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

$(INCDIR)/kot/%.h : $(SRCDIR)/%.h
	@ mkdir -p $(@D)
	cp -p $^ $(@D)

link:
	@ echo !==== LINKING
	@ mkdir -p $(BUILDDIR)
	$(AR) rcs $(BUILDDIR)/libc.a $(OBJS) $(OBJSCPP)

