_HOME = ../../../../

CC = clang-14
AR = llvm-ar-14

INCDIR = $(_HOME)Sysroot/Include/
BUILDDIR = $(_HOME)Sysroot/Lib/

CFLAGS = -O3 --target=x86_64-unknown-none-elf -fno-rtti -ffreestanding -fshort-wchar -fno-exceptions -fPIC -I$(INCDIR)

SRCDIR := ../Src
OBJDIR := ../Lib

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

INC = $(call rwildcard,$(SRCDIR),*.h)    
SRCC = $(call rwildcard,$(SRCDIR),*.c)
OBJS += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCC))

OBJINC += $(patsubst $(SRCDIR)/%.h, $(INCDIR)/kot-graphics/%.h, $(INC))

DIRS = $(wildcard $(SRCDIR)/*)   

main: copy $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@
	
copy: $(OBJINC)

$(INCDIR)/kot-graphics/%.h : $(SRCDIR)/%.h
	@ mkdir -p $(@D)
	cp -p $^ $(@D)

link:
	@ echo !==== LINKING
	$(AR) rcs $(BUILDDIR)kot-graphics.a $(OBJS)
