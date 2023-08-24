_END=$'\x1b[0m
_BOLD=$'\x1b[1m
_PURPLE=$'\x1b[35m

info_limine = /bin/echo -e "$(_PURPLE)[extern/limine]$(_END) $(_BOLD)$1$(_END)"

limine:
ifeq ($(wildcard ./limine/.*),)
	@ mkdir -m 777 -p limine
	@ $(call info_limine,"Downloading...")
	@ git clone https://github.com/limine-bootloader/limine.git --branch=v5.x-branch-binary --depth=1 
	@ $(call info_limine,"Compiling...")
	@ cd limine && make
	@ $(call info_limine,"done.")
endif

info_ovmf = /bin/echo -e "$(_PURPLE)[extern/ovmf]$(_END) $(_BOLD)$1$(_END)"

ovmf-aarch64:
ifeq ($(wildcard ./ovmf-aarch64/.*),)
	@ mkdir -m 777 -p ovmf-aarch64
	@ $(call info_ovmf,"Downloading for aarch64...")
	@ cd ovmf-aarch64 && curl -Lo OVMF-AA64.zip https://efi.akeo.ie/OVMF/OVMF-AA64.zip && unzip OVMF-AA64.zip
	@ $(call info_ovmf,"done.")
endif

ovmf-amd64:
ifeq ($(wildcard ./ovmf-amd64/.*),)
	@ mkdir -m 777 -p ovmf-amd64
	@ $(call info_ovmf,"Downloading for amd64...")
	@ cd ovmf-amd64 && curl -Lo OVMF-X64.zip https://efi.akeo.ie/OVMF/OVMF-X64.zip && unzip OVMF-X64.zip
	@ $(call info_ovmf,"done.")
endif
