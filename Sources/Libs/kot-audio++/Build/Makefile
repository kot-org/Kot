_HOME = ../../../../

INCDIR = $(_HOME)Sysroot/usr/include
BUILDDIR := $(_HOME)Sysroot/usr/lib

OBJDIR := ../Lib
SRCDIR := ../Src

CC = kot-x86_64-clang
AR = llvm-ar-14

CFLAGS = -O3 -I../Src -fno-exceptions

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
   
INC = $(call rwildcard,$(SRCDIR),*.h)      
SRCCPP = $(call rwildcard,$(SRCDIR),*.cpp)
OBJS += $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCCPP))

OBJINC += $(patsubst $(SRCDIR)/%.h, $(INCDIR)/kot-audio++/%.h, $(INC))

DIRS = $(wildcard $(SRCDIR)/*)         
	
main: copy $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !==== COMPILING C $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

copy: $(OBJINC)

$(INCDIR)/kot-audio++/%.h : $(SRCDIR)/%.h
	@ mkdir -p $(@D)
	cp -p $^ $(@D)

link:
	@ echo !==== LINKING
	@ mkdir -p $(BUILDDIR)
	$(AR) rcs $(BUILDDIR)/libkot-audio++.a $(OBJS)