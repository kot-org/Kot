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

git:
	@ if [ ! -d $(SOURCE) ]; then \
		rm -rf $(SOURCE); \
		cd $(HOME) && git clone https://github.com/kot-org/lua source; \
	fi

compile:
	@ cd $(SOURCE) && make all

copy_to_sysroot:
	@ mkdir -m 777 -p $(BIN)
	@ cp $(SOURCE)/lua $(BIN)

build: git compile copy_to_sysroot
