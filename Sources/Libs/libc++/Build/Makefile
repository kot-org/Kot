_HOME = ../../../../

INCDIR = $(_HOME)Sysroot/usr/include
BUILDDIR := $(_HOME)Sysroot/usr/lib

ASMC = nasm
CC = kot-x86_64-clang
AR = llvm-ar-14

CFLAGS = -O3 -fno-exceptions

SRCDIR = ../Src
OBJDIR = ../Lib

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

INC = $(call rwildcard,$(SRCDIR),*.h)    
SRCC = $(call rwildcard,$(SRCDIR),*.cpp)
OBJS += $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCC))

OBJINC += $(patsubst $(SRCDIR)/%.h, $(INCDIR)/kot++/%.h, $(INC))

DIRS = $(wildcard $(SRCDIR)/*)   

main: copy $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@
	
copy: $(OBJINC)

$(INCDIR)/kot++/%.h : $(SRCDIR)/%.h
	@ mkdir -p $(@D)
	cp -p $^ $(@D)

link:
	@ echo !==== LINKING
	$(AR) rcs $(BUILDDIR)/libkot-c++.a $(OBJS)
