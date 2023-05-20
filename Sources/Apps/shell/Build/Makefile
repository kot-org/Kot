_HOME = ../../../../

CC = kot-x86_64-clang

CFLAGS = -O3 -I../Src
LINKERFLAGS = -nostartfiles -Wall -static

SRCDIR := ../Src
OBJDIR := ../Lib
BINDIR := ../Bin

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCCPP = $(call rwildcard,$(SRCDIR),*.cpp)
OBJS += $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCCPP))

main: $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@


	@ echo !==== LINKING
	@ mkdir -p $(BINDIR)

	@ mkdir -p $(BINDIR)/core
	@ mkdir -p $(BINDIR)/utils

	$(CC) $(LINKERFLAGS) -o $(BINDIR)/$*.elf $@ $(_HOME)Sysroot/lib/crt0.o