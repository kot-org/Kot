_HOME = ../../../../

INCDIR = $(_HOME)Sysroot/Include/kot
OBJDIR := $(_HOME)Sysroot/Lib

SRCDIR := ../src

CC = clang-14
ASMC = nasm

ASMFLAGS = 

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
       
ASSRC = $(call rwildcard,$(SRCDIR),*.asm)   
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%.o, $(ASSRC))

DIRS = $(wildcard $(SRCDIR)/*)         

	
main: $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.asm
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@

