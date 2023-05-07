_HOME = ../../../../../

CC = clang-14
ASMC = nasm

CFLAGS = -O3 -fno-rtti -ffreestanding -fshort-wchar -fno-exceptions -fPIC -no-pie -I../Src -I$(_HOME)Sysroot/include
LINKERFLAGS = -nostartfiles -Wall -static -L$(_HOME)Sysroot/lib -lc
ASMFLAGS = 

SRCDIR := ../Src
OBJDIR := ../Lib
BINDIR := ../Bin

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCC = $(call rwildcard,$(SRCDIR),*.cpp)
OBJS += $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCC))
DIRS = $(wildcard $(SRCDIR)/*)         

main: $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

link:
	@ echo !==== LINKING
	@ mkdir -p $(BINDIR)
	$(CC) $(LINKERFLAGS) -o $(BINDIR)/Shell.elf $(OBJS) $(_HOME)Sysroot/lib/crt0.o