_HOME = ../../../../

INCDIR = $(_HOME)Sysroot/include/kot
OBJDIR := $(_HOME)Sysroot/lib

SRCDIR := ../src

CC = clang
ASMC = nasm

LD = ld.lld
AS = as
AR = ar

CFLAGS = -Wall -Werror -fpic --target=x86_64-unknown-none-elf
LDFLAGS = -shared 
ASMFLAGS = 

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
   
INC = $(call rwildcard,$(SRCDIR),*.h)      
SRCCPP = $(call rwildcard,$(SRCDIR),*.c)      
ASSRC = $(call rwildcard,$(SRCDIR),*.s)   
OBJS += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCCPP))
OBJS += $(patsubst $(SRCDIR)/%.s, $(OBJDIR)/%.o, $(ASSRC))

OBJINC += $(patsubst $(SRCDIR)/%.h, $(INCDIR)/%.h, $(INC))

DIRS = $(wildcard $(SRCDIR)/*)         

	
main: $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.s
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(AS) $(ASMFLAGS) -c $^ -o $@

