# you can set this to 1 to see all commands that are being run
VERBOSE ?= 0

ifeq ($(VERBOSE),1)
export Q :=
export VERBOSE := 1
else
export Q := @
export VERBOSE := 0
endif

BUILDRESULTS ?= buildresults
CONFIGURED_BUILD_DEP = $(BUILDRESULTS)/build.ninja

# Override to provide your own settings to the shim
OPTIONS ?=
LTO ?= 0
CROSS ?=
NATIVE ?=
DEBUG ?= 0
SANITIZER ?= none
INTERNAL_OPTIONS =

ifeq ($(LTO),1)
	INTERNAL_OPTIONS += -Db_lto=true
endif

ifneq ($(CROSS),)
	# Split into two strings, first is arch, second is chip
	CROSS_2 := $(subst :, ,$(CROSS))
	INTERNAL_OPTIONS += $(foreach FILE,$(CROSS_2),--cross-file=build/cross/$(FILE).txt)
endif

ifneq ($(NATIVE),)
	# Split into words delimited by :
	NATIVE_2 := $(subst :, ,$(NATIVE))
	INTERNAL_OPTIONS += $(foreach FILE,$(NATIVE_2),--native-file=build/native/$(FILE).txt)
endif

ifeq ($(DEBUG),1)
	INTERNAL_OPTIONS += -Ddebug=true -Doptimization=g
endif

ifneq ($(SANITIZER),none)
	INTERNAL_OPTIONS += -Db_sanitize=$(SANITIZER) -Db_lundef=false
endif

all: default

.PHONY: default
default: | $(CONFIGURED_BUILD_DEP)
	$(Q)ninja -C $(BUILDRESULTS)

# Manually Reconfigure a target, esp. with new options
.PHONY: reconfig
reconfig:
	$(Q) meson $(BUILDRESULTS) --reconfigure $(INTERNAL_OPTIONS) $(OPTIONS)

# Runs whenever the build has not been configured successfully
$(CONFIGURED_BUILD_DEP):
	$(Q) meson $(BUILDRESULTS) $(INTERNAL_OPTIONS) $(OPTIONS)

.PHONY: clean
clean:
	$(Q) if [ -d "$(BUILDRESULTS)" ]; then ninja -C buildresults clean; fi

.PHONY: distclean
distclean:
	$(Q) rm -rf $(BUILDRESULTS)

### Help Output ###
.PHONY : help
help :
	@echo "usage: make [OPTIONS] <target>"
	@echo "  Options:"
	@echo "    > VERBOSE Show verbose output for Make rules. Default 0. Enable with 1."
	@echo "    > BUILDRESULTS Directory for build results. Default buildresults."
	@echo "    > OPTIONS Configuration options to pass to a build. Default empty."
	@echo "    > LTO Enable LTO builds. Default 0. Enable with 1."
	@echo "    > DEBUG Enable a debug build. Default 0 (release). Enable with 1."
	@echo "    > CROSS Enable a Cross-compilation build. Default format is arch:chip."
	@echo "         - Example: make CROSS=arm:cortex-m3"
	@echo "         - For supported chips, see build/cross/"
	@echo "         - Additional files can be layered by adding additional"
	@echo "           args separated by ':'"
	@echo "    > NATIVE Supply an alternative native toolchain by name."
	@echo "         - Example: make NATIVE=gcc-9"
	@echo "         - Additional files can be layered by adding additional"
	@echo "           args separated by ':'"
	@echo "         - Example: make NATIVE=gcc-9:gcc-gold"
	@echo "    > SANITIZER Compile with support for a Clang/GCC Sanitizer."
	@echo "         Options are: none (default), address, thread, undefined, memory,"
	@echo "         and address,undefined' as a combined option"
	@echo "Targets:"
	@echo "  default: Builds all default targets ninja knows about"
	@echo "  clean: cleans build artifacts, keeping build files in place"
	@echo "  distclean: removes the configured build output directory"
	@echo "  reconfig: Reconfigure an existing build output folder with new settings"
