cyan = /bin/echo -e "\x1b[36m\#\# $1\x1b[0m"

# Project Root
override HOME = ../..

# Project Resources
SYSROOT = $(HOME)/../../../../sysroot
INCLUDE = $(SYSROOT)/usr/include
LIBRARIES = $(SYSROOT)/usr/lib
SOURCE = $(HOME)/source
TOOLS = $(HOME)/../../tools
BIN = bin/usr/bin
LIB = lib

# Tools Config
CFLAGS = -Werror 

LDFLAGS = -Wall \
		  -lc

# Recursive Wild Card
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Source Files
C_SRC = $(call rwildcard,$(SOURCE),*.c)

OBJS = $(patsubst $(SOURCE)/%.c,$(LIB)/%_c.o,$(C_SRC))

# Target
$(LIB)/%_c.o: $(SOURCE)/%.c
	@ mkdir -m 777 -p $(@D)
	@ $(call cyan,"$(subst ../,,$^)")
	@ $(CC) $(CFLAGS) -c $^ -o $@

link:
	@ mkdir -m 777 -p $(BIN)
	$(CC) $(LDFLAGS) -o $(BIN)/weather-ui $(OBJS) $(LIBRARIES)/libcurl.a $(LIBRARIES)/libtls.a $(LIBRARIES)/libssl.a $(LIBRARIES)/libkot-graphics.a $(LIBRARIES)/libfreetype.a $(LIBRARIES)/libz.a $(LIBRARIES)/libturbojpeg.a $(LIBRARIES)/libcjson.a

copy_res:
	@ mkdir -m 777 -p $(BIN)/res/weather-ui
	@ cp -r $(HOME)/res/. $(BIN)/res/weather-ui

build: $(OBJS) link copy_res
