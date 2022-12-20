_HOME = ../../../../

INCDIR = $(_HOME)Sysroot/Include
BUILDDIR := $(_HOME)Sysroot/Lib
BINDIR = $(_HOME)Bin/Modules

OBJDIR := ../Lib
SRCDIR := ../Src

CC = clang-14
AR = llvm-ar-14

CFLAGS = -I$(INCDIR) --target=x86_64-unknown-none-elf -fno-rtti -ffreestanding -fshort-wchar -fno-exceptions -fPIC -O3

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
   
INC = $(call rwildcard,$(SRCDIR),*.h)      
SRCC = $(call rwildcard,$(SRCDIR),*.c)
OBJS += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCC))

OBJINC += $(patsubst $(SRCDIR)/%.h, $(INCDIR)/kot-ui/%.h, $(INC))

DIRS = $(wildcard $(SRCDIR)/*)         
	
main: copy $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING C $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

copy: $(OBJINC)

$(INCDIR)/kot-ui/%.h : $(SRCDIR)/%.h
	@ mkdir -p $(@D)
	cp -p $^ $(@D)

link:
	@ echo !==== LINKING
	@ mkdir -p $(BUILDDIR)
	$(AR) rcs $(BUILDDIR)/kot-ui.a $(OBJS)