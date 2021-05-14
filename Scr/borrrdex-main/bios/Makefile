ASMC = nasm
ASMFLAGS = -g -F dwarf

bootloader: boot.o

%.o: %.asm
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@

clean: rm *.o