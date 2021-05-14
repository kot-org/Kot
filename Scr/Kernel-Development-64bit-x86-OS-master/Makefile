include Makefile.config

CC=gcc
CFLAGS=-O1 -std=c99 -Wall -Werror -fPIC -march=amdfam10 -g3 -Wno-deprecated-declarations -D_XOPEN_SOURCE=600
CFLAGS_FULL=-nostdinc -Iinclude -msoft-float -mno-sse -mno-red-zone -fno-builtin -fno-stack-protector
LD=ld
LDLAGS=-nostdlib
AR=ar

ROOTFS=rootfs
ROOTBIN=$(ROOTFS)/bin
ROOTLIB=$(ROOTFS)/lib
ROOTBOOT=$(ROOTFS)/boot

KERN_SRCS:=$(wildcard sys/*.c sys/*.s sys/*/*.c sys/*/*.s)
BIN_SRCS:=$(wildcard bin/*/*.c)
LIBC_SRCS:=$(wildcard libc/*.c libc/*.s libc/*/*.c libc/*/*.s)
LD_SRCS:=$(wildcard ld/*.c)
BINS:=$(addprefix $(ROOTFS)/,$(wildcard bin/*))

.PHONY: all binary wp1p1 wp1 wp2 project

all: $(ASSIGNMENT)

project: $(USER).img $(USER)-data.img

wp1p1:
	@$(MAKE) --no-print-directory BIN=rootfs/bin/sbush binary
ifneq "$(ASSIGNMENT)" "wp1p1"
CFLAGS+= $(CFLAGS_FULL)
endif

wp1: $(BINS)

wp2: project

wp3: project

$(USER).iso: kernel $(ROOTBOOT)/large-file-padding
	cp kernel $(ROOTBOOT)/kernel/kernel
	mkisofs -r -no-emul-boot -input-charset utf-8 -b boot/cdboot -o $@ $(ROOTFS)/

$(USER).img: $(USER).iso
	rm -f $@
	mkfs.vfat -n SBUNIX -I -C $@ 65536
	syslinux -i $@
	mcopy -i $@ /usr/lib/syslinux/memdisk $(ROOTBOOT)/syslinux.cfg ::
	mcopy -i $@ $(USER).iso ::sbunix.iso

$(ROOTBOOT)/large-file-padding:
	dd if=/dev/zero of=$(ROOTBOOT)/large-file-padding seek=30 bs=1M count=0

$(USER)-data.img:
	qemu-img create -f raw $@ 16M

kernel: $(patsubst %.s,obj/%.asm.o,$(KERN_SRCS:%.c=obj/%.o)) obj/tarfs.o
	$(LD) $(LDLAGS) -o $@ -T sys/linker.script $^

obj/tarfs.o: $(BINS)
	tar --format=ustar -cvf obj/tarfs --no-recursion -C $(ROOTFS) $(shell find $(ROOTFS)/ -name boot -prune -o ! -name .gitkeep -printf "%P\n")
	cd obj && objcopy --input binary --binary-architecture i386 --output elf64-x86-64 tarfs tarfs.o

$(ROOTLIB)/libc.a: $(patsubst %.s,obj/%.asm.o,$(LIBC_SRCS:%.c=obj/%.o))
	$(AR) rcs $@ $^

#$(ROOTLIB)/libc.so: $(LIBC_SRCS:%.c=obj/%.o) $(ROOTLIB)/ld.so
#	$(LD) $(LDLAGS) -shared -soname=$@ --dynamic-linker=/lib/ld.so --rpath-link=/lib -o $@ $^
#
#$(ROOTLIB)/ld.so: $(LD_SRCS:%.c=obj/%.o)
#	$(LD) $(LDLAGS) -shared -o $@ $^

$(ROOTLIB)/crt1.o: obj/crt/crt1.o
	cp $^ $@

$(BINS): $(ROOTLIB)/crt1.o $(ROOTLIB)/libc.a $(shell find bin/ -type f -name *.c)
	@$(MAKE) --no-print-directory BIN=$@ binary

binary: $(patsubst %.c,obj/%.o,$(wildcard $(BIN:rootfs/%=%)/*.c))
ifneq "$(ASSIGNMENT)" "wp1p1"
	$(LD) $(LDLAGS) -o $(BIN) $(ROOTLIB)/crt1.o $^ $(ROOTLIB)/libc.a
else
	$(CC) -o $(BIN) $^
endif

obj/%.o: %.c $(wildcard include/*.h include/*/*.h)
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

obj/%.asm.o: %.s
	@mkdir -p $(dir $@)
	$(CC) -c -x assembler-with-cpp $(CFLAGS) $(CFLAGS_FULL) -o $@ $<

.PHONY: submit clean

clean:
	find $(ROOTLIB) $(ROOTBIN) -type f ! -name .gitkeep -print -delete
	rm -rfv obj kernel tarfs $(USER).iso $(USER).img $(ROOTBOOT)/kernel/kernel

SUBMITTO=/submit
submit: clean $(USER)-data.img
	tar -czvf $(USER).tgz --exclude=.gitkeep --exclude=.*.sw? --exclude=*~ --ignore-failed-read LICENSE README Makefile Makefile.config sys bin crt libc include $(ROOTFS) $(USER)-data.img
	mv -v $(USER).tgz $(SUBMITTO)/$(USER)-$(ASSIGNMENT)=`date +%F=%T`.tgz
