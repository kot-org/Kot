CC = gcc
LD = ld
ASM = nasm

override CFLAGS := -I ../libc/include -fno-builtin -nostdlib -nostdinc -g $(CFLAGS)
LDFLAGS = -static -nostdlib

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
DIRS = $(wildcard $(SRCDIR)/*)

all: setup $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !== Compiling $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

link:
	@ echo !== Linking $^
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/usertest ../libc/bin/crt0.o ../libc/bin/crti.o $(OBJS) ../libc/bin/libc.a ../libc/bin/crtn.o

setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(OBJDIR)

clean:
	rm -f $(BUILDDIR)/*
	rm -rf $(OBJDIR)