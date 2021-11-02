LDS = main.ld
CC = gcc
LD = ld

CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-exceptions
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

SRCDIR := ../Scr
OBJDIR := ../Lib
BUILDDIR = ../Bin

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCCPP = $(call rwildcard,$(SRCDIR),*.cpp)      
OBJS += $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCCPP))
DIRS = $(wildcard $(SRCDIR)/*)         



main: $(OBJS) link 

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@
	
link:
	@ echo !==== LINKING
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/main.elf $(OBJS) $(OBJSCPP)

