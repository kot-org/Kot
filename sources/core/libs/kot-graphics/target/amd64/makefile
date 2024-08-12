cyan = /bin/echo -e "\x1b[36m\#\# $1\x1b[0m"

# Project Root
override HOME = ../..

# Project Resources
SYSROOT = $(HOME)/../../../../sysroot
INCLUDE = $(SYSROOT)/usr/include
LIBRARIES = $(SYSROOT)/usr/lib
SOURCE = $(HOME)/source
TOOLS = $(HOME)/../../tools
BIN = bin/system/welcome
LIB = lib
INCDIR = $(DESTDIR)$(PREFIX)/include
BUILDDIR = $(DESTDIR)$(PREFIX)/lib

# Tools Config
CFLAGS = -Werror 

LDFLAGS = -Wall \
		  -lc

# Recursive Wild Card
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Source Files
C_SRC = $(call rwildcard,$(SOURCE),*.c)
OBJS = $(patsubst $(SOURCE)/%.c,$(LIB)/%_c.o,$(C_SRC))

INC = $(call rwildcard,$(SOURCE),*.h)
OBJINC += $(patsubst $(SOURCE)/%.h, $(INCDIR)/kot-graphics/%.h, $(INC))

copy: $(OBJINC)

$(INCDIR)/kot-graphics/%.h: $(SOURCE)/%.h
	@ mkdir -p $(@D)
	@ cp -p $^ $(@D)

# Target
$(LIB)/%_c.o: $(SOURCE)/%.c
	@ mkdir -m 777 -p $(@D)
	@ $(call cyan,"$(subst ../,,$^)")
	$(CC) $(CFLAGS) -c $^ -o $@ -I$(INCDIR) -I$(SYSROOT)/usr/include/freetype2

link:
	@ mkdir -m 777 -p $(BIN)
	@ mkdir -m 777 -p $(BUILDDIR)
	@ $(AR) rcs $(BUILDDIR)/libkot-graphics.a $(OBJS)

build: copy $(OBJS) link
