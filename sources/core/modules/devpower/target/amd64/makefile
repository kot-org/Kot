cyan = /bin/echo -e "\x1b[36m\#\# $1\x1b[0m"

# Project Root
override HOME = ../..

# Project Resources
SOURCE = $(HOME)/source
KERNEL_SOURCE = $(HOME)/../../kernel/source
BIN = bin/system/modules

# Tools Config
CFLAGS = -I$(SOURCE) \
 		 -I$(KERNEL_SOURCE) \
		 -ffreestanding \
		 -nostdlib \
		 -fno-stack-protector \
		 -mno-avx \
		 -mno-sse \
		 -mno-mmx \
		 -mno-3dnow \
		 -mno-red-zone \
		 -g \
		 -fshort-wchar \
		 -Werror \
		 -Wstrict-prototypes \
		 -static \
		 -mcmodel=large


# Recursive Wild Card
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Source Files
C_SRC = $(filter-out $(SOURCE)/main.c,$(call rwildcard,$(SOURCE),*.c))

# Target
build:
	@ mkdir -m 777 -p $(BIN)
	@ $(CC) $(CFLAGS) -c $(SOURCE)/main.c -o $(BIN)/devpower.ksys

