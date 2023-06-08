_HOME = ../../../../

CC = kot-x86_64-clang

CFLAGS = -O3
LINKERFLAGS = -Wall -lc

SRCDIR := ../Src
BINDIR := ../Bin

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCCPP = $(call rwildcard,$(SRCDIR)/utils,*.cpp)
OBJS += $(patsubst $(SRCDIR)/utils/%.cpp, $(BINDIR)/utils/%.elf, $(SRCCPP))

all: utils main

utils: $(OBJS)

$(BINDIR)/utils/%.elf: $(SRCDIR)/utils/%.cpp
	@ mkdir -p $(@D)
	$(CC) -I$(@D) $(LINKERFLAGS) -o $(BINDIR)/utils/$*.elf $^

main:
	$(CC) -I$(SRCDIR) $(LINKERFLAGS) -o $(BINDIR)/Shell.elf $(SRCDIR)/main.cpp