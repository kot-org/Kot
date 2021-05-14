# Makefile
target    :=BOOTX64.EFI
CC        :=gcc -Wall -Werror -m64 -mabi=ms
CFLAGS    :=-ffreestanding
LFLAGS    :=-nostdlib -shared -Wl,-dll -Wl,--subsystem,10 -e efi_main -o 

all: $(target)

$(target): main.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $^ $(LFLAGS) $@

clean:
	@echo "Cleaning files...."
	rm -f *.o *.EFI
	@echo "Done."
