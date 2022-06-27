# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
# Copyright (C) 2001-2020 Free Software Foundation, Inc.
#
# This file may be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

PACKAGE = bfd
VERSION = 2.35

SHELL = /bin/sh


srcdir = ../../../bfd/po
top_srcdir = ../../../bfd
VPATH = ../../../bfd/po
top_builddir = ..

prefix = /home/sebastien/Bureau/Kot/Toolchain/Local
exec_prefix = ${prefix}
datadir = ${datarootdir}
localedir = ${datarootdir}/locale
gnulocaledir = ${prefix}/share/locale
gettextsrcdir = ${prefix}/share/gettext/po
subdir = po

DESTDIR =

INSTALL = /usr/bin/install -c
INSTALL_DATA = /usr/bin/install -c -m 644
MKINSTALLDIRS = $(top_builddir)/../../bfd/../mkinstalldirs

CC = gcc
GENCAT = 
GMSGFMT = PATH=../src:$$PATH /usr/bin/msgfmt
MSGFMT = /usr/bin/msgfmt
XGETTEXT = PATH=../src:$$PATH /usr/bin/xgettext
MSGMERGE = PATH=../src:$$PATH msgmerge

DEFS = -DHAVE_CONFIG_H
CFLAGS = -g -O2
CPPFLAGS = 

INCLUDES = -I.. -I$(top_srcdir)/intl

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)

SOURCES = cat-id-tbl.c
POFILES =  $(srcdir)/da.po $(srcdir)/es.po $(srcdir)/fi.po $(srcdir)/fr.po $(srcdir)/hr.po $(srcdir)/id.po $(srcdir)/ja.po $(srcdir)/ro.po $(srcdir)/ru.po $(srcdir)/rw.po $(srcdir)/sr.po $(srcdir)/sv.po $(srcdir)/tr.po $(srcdir)/uk.po $(srcdir)/vi.po $(srcdir)/zh_CN.po $(srcdir)/pt.po
GMOFILES =  $(srcdir)/da.gmo $(srcdir)/es.gmo $(srcdir)/fi.gmo $(srcdir)/fr.gmo $(srcdir)/hr.gmo $(srcdir)/id.gmo $(srcdir)/ja.gmo $(srcdir)/ro.gmo $(srcdir)/ru.gmo $(srcdir)/rw.gmo $(srcdir)/sr.gmo $(srcdir)/sv.gmo $(srcdir)/tr.gmo $(srcdir)/uk.gmo $(srcdir)/vi.gmo $(srcdir)/zh_CN.gmo $(srcdir)/pt.gmo
DISTFILES = ChangeLog Makefile.in.in SRC-POTFILES.in BLD-POTFILES.in $(PACKAGE).pot \
stamp-cat-id $(POFILES) $(GMOFILES) $(SOURCES)

# Note - the following line gets processed by bfd/configure and amended
# to contain the full list of source dir POTFILES.
SRC-POTFILES = \
	../../../bfd/aix386-core.c \
	../../../bfd/aix5ppc-core.c \
	../../../bfd/aout-cris.c \
	../../../bfd/aout-ns32k.c \
	../../../bfd/aout-target.h \
	../../../bfd/aout32.c \
	../../../bfd/aout64.c \
	../../../bfd/aoutx.h \
	../../../bfd/arc-got.h \
	../../../bfd/arc-plt.h \
	../../../bfd/archive.c \
	../../../bfd/archive64.c \
	../../../bfd/archures.c \
	../../../bfd/bfd.c \
	../../../bfd/bfdio.c \
	../../../bfd/bfdwin.c \
	../../../bfd/binary.c \
	../../../bfd/cache.c \
	../../../bfd/cf-i386lynx.c \
	../../../bfd/cisco-core.c \
	../../../bfd/coff-alpha.c \
	../../../bfd/coff-arm.h \
	../../../bfd/coff-bfd.c \
	../../../bfd/coff-bfd.h \
	../../../bfd/coff-go32.c \
	../../../bfd/coff-i386.c \
	../../../bfd/coff-mips.c \
	../../../bfd/coff-rs6000.c \
	../../../bfd/coff-sh.c \
	../../../bfd/coff-stgo32.c \
	../../../bfd/coff-tic30.c \
	../../../bfd/coff-tic4x.c \
	../../../bfd/coff-tic54x.c \
	../../../bfd/coff-x86_64.c \
	../../../bfd/coff-z80.c \
	../../../bfd/coff-z8k.c \
	../../../bfd/coff64-rs6000.c \
	../../../bfd/coffcode.h \
	../../../bfd/coffgen.c \
	../../../bfd/cofflink.c \
	../../../bfd/coffswap.h \
	../../../bfd/compress.c \
	../../../bfd/corefile.c \
	../../../bfd/cpu-aarch64.c \
	../../../bfd/cpu-aarch64.h \
	../../../bfd/cpu-alpha.c \
	../../../bfd/cpu-arc.c \
	../../../bfd/cpu-arm.c \
	../../../bfd/cpu-arm.h \
	../../../bfd/cpu-avr.c \
	../../../bfd/cpu-bfin.c \
	../../../bfd/cpu-bpf.c \
	../../../bfd/cpu-cr16.c \
	../../../bfd/cpu-cris.c \
	../../../bfd/cpu-crx.c \
	../../../bfd/cpu-csky.c \
	../../../bfd/cpu-d10v.c \
	../../../bfd/cpu-d30v.c \
	../../../bfd/cpu-dlx.c \
	../../../bfd/cpu-epiphany.c \
	../../../bfd/cpu-fr30.c \
	../../../bfd/cpu-frv.c \
	../../../bfd/cpu-ft32.c \
	../../../bfd/cpu-h8300.c \
	../../../bfd/cpu-h8300.h \
	../../../bfd/cpu-hppa.c \
	../../../bfd/cpu-i386.c \
	../../../bfd/cpu-ia64.c \
	../../../bfd/cpu-iamcu.c \
	../../../bfd/cpu-ip2k.c \
	../../../bfd/cpu-iq2000.c \
	../../../bfd/cpu-k1om.c \
	../../../bfd/cpu-l1om.c \
	../../../bfd/cpu-lm32.c \
	../../../bfd/cpu-m10200.c \
	../../../bfd/cpu-m10300.c \
	../../../bfd/cpu-m32c.c \
	../../../bfd/cpu-m32r.c \
	../../../bfd/cpu-m68hc11.c \
	../../../bfd/cpu-m68hc12.c \
	../../../bfd/cpu-m68k.c \
	../../../bfd/cpu-m68k.h \
	../../../bfd/cpu-m9s12x.c \
	../../../bfd/cpu-m9s12xg.c \
	../../../bfd/cpu-mcore.c \
	../../../bfd/cpu-mep.c \
	../../../bfd/cpu-metag.c \
	../../../bfd/cpu-microblaze.c \
	../../../bfd/cpu-mips.c \
	../../../bfd/cpu-mmix.c \
	../../../bfd/cpu-moxie.c \
	../../../bfd/cpu-msp430.c \
	../../../bfd/cpu-mt.c \
	../../../bfd/cpu-nds32.c \
	../../../bfd/cpu-nfp.c \
	../../../bfd/cpu-nios2.c \
	../../../bfd/cpu-ns32k.c \
	../../../bfd/cpu-or1k.c \
	../../../bfd/cpu-pdp11.c \
	../../../bfd/cpu-pj.c \
	../../../bfd/cpu-powerpc.c \
	../../../bfd/cpu-pru.c \
	../../../bfd/cpu-riscv.c \
	../../../bfd/cpu-rl78.c \
	../../../bfd/cpu-rs6000.c \
	../../../bfd/cpu-rx.c \
	../../../bfd/cpu-s12z.c \
	../../../bfd/cpu-s390.c \
	../../../bfd/cpu-score.c \
	../../../bfd/cpu-sh.c \
	../../../bfd/cpu-sparc.c \
	../../../bfd/cpu-spu.c \
	../../../bfd/cpu-tic30.c \
	../../../bfd/cpu-tic4x.c \
	../../../bfd/cpu-tic54x.c \
	../../../bfd/cpu-tic6x.c \
	../../../bfd/cpu-tilegx.c \
	../../../bfd/cpu-tilepro.c \
	../../../bfd/cpu-v850.c \
	../../../bfd/cpu-v850_rh850.c \
	../../../bfd/cpu-vax.c \
	../../../bfd/cpu-visium.c \
	../../../bfd/cpu-wasm32.c \
	../../../bfd/cpu-xc16x.c \
	../../../bfd/cpu-xgate.c \
	../../../bfd/cpu-xstormy16.c \
	../../../bfd/cpu-xtensa.c \
	../../../bfd/cpu-z80.c \
	../../../bfd/cpu-z8k.c \
	../../../bfd/dwarf1.c \
	../../../bfd/dwarf2.c \
	../../../bfd/ecoff-bfd.h \
	../../../bfd/ecoff.c \
	../../../bfd/ecofflink.c \
	../../../bfd/ecoffswap.h \
	../../../bfd/elf-attrs.c \
	../../../bfd/elf-bfd.h \
	../../../bfd/elf-eh-frame.c \
	../../../bfd/elf-hppa.h \
	../../../bfd/elf-ifunc.c \
	../../../bfd/elf-linker-x86.h \
	../../../bfd/elf-linux-core.h \
	../../../bfd/elf-m10200.c \
	../../../bfd/elf-m10300.c \
	../../../bfd/elf-nacl.c \
	../../../bfd/elf-nacl.h \
	../../../bfd/elf-properties.c \
	../../../bfd/elf-s390.h \
	../../../bfd/elf-strtab.c \
	../../../bfd/elf-vxworks.c \
	../../../bfd/elf-vxworks.h \
	../../../bfd/elf.c \
	../../../bfd/elf32-am33lin.c \
	../../../bfd/elf32-arc.c \
	../../../bfd/elf32-arm.c \
	../../../bfd/elf32-arm.h \
	../../../bfd/elf32-avr.c \
	../../../bfd/elf32-avr.h \
	../../../bfd/elf32-bfin.c \
	../../../bfd/elf32-bfin.h \
	../../../bfd/elf32-cr16.c \
	../../../bfd/elf32-cr16.h \
	../../../bfd/elf32-cris.c \
	../../../bfd/elf32-crx.c \
	../../../bfd/elf32-csky.c \
	../../../bfd/elf32-csky.h \
	../../../bfd/elf32-d10v.c \
	../../../bfd/elf32-d30v.c \
	../../../bfd/elf32-dlx.c \
	../../../bfd/elf32-dlx.h \
	../../../bfd/elf32-epiphany.c \
	../../../bfd/elf32-fr30.c \
	../../../bfd/elf32-frv.c \
	../../../bfd/elf32-ft32.c \
	../../../bfd/elf32-gen.c \
	../../../bfd/elf32-h8300.c \
	../../../bfd/elf32-hppa.c \
	../../../bfd/elf32-hppa.h \
	../../../bfd/elf32-i386.c \
	../../../bfd/elf32-ip2k.c \
	../../../bfd/elf32-iq2000.c \
	../../../bfd/elf32-lm32.c \
	../../../bfd/elf32-m32c.c \
	../../../bfd/elf32-m32r.c \
	../../../bfd/elf32-m68hc11.c \
	../../../bfd/elf32-m68hc12.c \
	../../../bfd/elf32-m68hc1x.c \
	../../../bfd/elf32-m68hc1x.h \
	../../../bfd/elf32-m68k.c \
	../../../bfd/elf32-m68k.h \
	../../../bfd/elf32-mcore.c \
	../../../bfd/elf32-mep.c \
	../../../bfd/elf32-metag.c \
	../../../bfd/elf32-metag.h \
	../../../bfd/elf32-microblaze.c \
	../../../bfd/elf32-mips.c \
	../../../bfd/elf32-moxie.c \
	../../../bfd/elf32-msp430.c \
	../../../bfd/elf32-mt.c \
	../../../bfd/elf32-nds32.c \
	../../../bfd/elf32-nds32.h \
	../../../bfd/elf32-nios2.c \
	../../../bfd/elf32-nios2.h \
	../../../bfd/elf32-or1k.c \
	../../../bfd/elf32-pj.c \
	../../../bfd/elf32-ppc.c \
	../../../bfd/elf32-ppc.h \
	../../../bfd/elf32-pru.c \
	../../../bfd/elf32-rl78.c \
	../../../bfd/elf32-rx.c \
	../../../bfd/elf32-rx.h \
	../../../bfd/elf32-s12z.c \
	../../../bfd/elf32-s390.c \
	../../../bfd/elf32-score.c \
	../../../bfd/elf32-score.h \
	../../../bfd/elf32-score7.c \
	../../../bfd/elf32-sh-relocs.h \
	../../../bfd/elf32-sh.c \
	../../../bfd/elf32-sparc.c \
	../../../bfd/elf32-spu.c \
	../../../bfd/elf32-spu.h \
	../../../bfd/elf32-tic6x.c \
	../../../bfd/elf32-tic6x.h \
	../../../bfd/elf32-tilegx.c \
	../../../bfd/elf32-tilegx.h \
	../../../bfd/elf32-tilepro.c \
	../../../bfd/elf32-tilepro.h \
	../../../bfd/elf32-v850.c \
	../../../bfd/elf32-v850.h \
	../../../bfd/elf32-vax.c \
	../../../bfd/elf32-visium.c \
	../../../bfd/elf32-wasm32.c \
	../../../bfd/elf32-xc16x.c \
	../../../bfd/elf32-xgate.c \
	../../../bfd/elf32-xstormy16.c \
	../../../bfd/elf32-xtensa.c \
	../../../bfd/elf32-z80.c \
	../../../bfd/elf32.c \
	../../../bfd/elf64-alpha.c \
	../../../bfd/elf64-bpf.c \
	../../../bfd/elf64-gen.c \
	../../../bfd/elf64-hppa.c \
	../../../bfd/elf64-hppa.h \
	../../../bfd/elf64-ia64-vms.c \
	../../../bfd/elf64-mips.c \
	../../../bfd/elf64-mmix.c \
	../../../bfd/elf64-nfp.c \
	../../../bfd/elf64-ppc.c \
	../../../bfd/elf64-ppc.h \
	../../../bfd/elf64-s390.c \
	../../../bfd/elf64-sparc.c \
	../../../bfd/elf64-tilegx.c \
	../../../bfd/elf64-tilegx.h \
	../../../bfd/elf64-x86-64.c \
	../../../bfd/elf64.c \
	../../../bfd/elfcode.h \
	../../../bfd/elfcore.h \
	../../../bfd/elflink.c \
	../../../bfd/elfn32-mips.c \
	../../../bfd/elfxx-aarch64.c \
	../../../bfd/elfxx-aarch64.h \
	../../../bfd/elfxx-ia64.c \
	../../../bfd/elfxx-ia64.h \
	../../../bfd/elfxx-mips.c \
	../../../bfd/elfxx-mips.h \
	../../../bfd/elfxx-riscv.c \
	../../../bfd/elfxx-riscv.h \
	../../../bfd/elfxx-sparc.c \
	../../../bfd/elfxx-sparc.h \
	../../../bfd/elfxx-tilegx.c \
	../../../bfd/elfxx-tilegx.h \
	../../../bfd/elfxx-x86.c \
	../../../bfd/elfxx-x86.c \
	../../../bfd/elfxx-x86.h \
	../../../bfd/format.c \
	../../../bfd/genlink.h \
	../../../bfd/go32stub.h \
	../../../bfd/hash.c \
	../../../bfd/hpux-core.c \
	../../../bfd/i386aout.c \
	../../../bfd/i386bsd.c \
	../../../bfd/i386lynx.c \
	../../../bfd/i386msdos.c \
	../../../bfd/ihex.c \
	../../../bfd/init.c \
	../../../bfd/irix-core.c \
	../../../bfd/libaout.h \
	../../../bfd/libbfd.c \
	../../../bfd/libbfd.h \
	../../../bfd/libcoff.h \
	../../../bfd/libecoff.h \
	../../../bfd/libhppa.h \
	../../../bfd/libpei.h \
	../../../bfd/libxcoff.h \
	../../../bfd/linker.c \
	../../../bfd/lynx-core.c \
	../../../bfd/mach-o-aarch64.c \
	../../../bfd/mach-o-arm.c \
	../../../bfd/mach-o-i386.c \
	../../../bfd/mach-o-x86-64.c \
	../../../bfd/mach-o.c \
	../../../bfd/mach-o.h \
	../../../bfd/merge.c \
	../../../bfd/mmo.c \
	../../../bfd/netbsd-core.c \
	../../../bfd/netbsd.h \
	../../../bfd/ns32k.h \
	../../../bfd/ns32knetbsd.c \
	../../../bfd/opncls.c \
	../../../bfd/osf-core.c \
	../../../bfd/pc532-mach.c \
	../../../bfd/pdp11.c \
	../../../bfd/pe-arm-wince.c \
	../../../bfd/pe-arm.c \
	../../../bfd/pe-i386.c \
	../../../bfd/pe-mcore.c \
	../../../bfd/pe-ppc.c \
	../../../bfd/pe-sh.c \
	../../../bfd/pe-x86_64.c \
	../../../bfd/pef-traceback.h \
	../../../bfd/pef.c \
	../../../bfd/pef.h \
	../../../bfd/pei-arm-wince.c \
	../../../bfd/pei-arm.c \
	../../../bfd/pei-i386.c \
	../../../bfd/pei-ia64.c \
	../../../bfd/pei-mcore.c \
	../../../bfd/pei-ppc.c \
	../../../bfd/pei-sh.c \
	../../../bfd/pei-x86_64.c \
	../../../bfd/peicode.h \
	../../../bfd/plugin.c \
	../../../bfd/plugin.h \
	../../../bfd/ppcboot.c \
	../../../bfd/reloc.c \
	../../../bfd/reloc16.c \
	../../../bfd/rs6000-core.c \
	../../../bfd/sco5-core.c \
	../../../bfd/section.c \
	../../../bfd/simple.c \
	../../../bfd/som.c \
	../../../bfd/som.h \
	../../../bfd/srec.c \
	../../../bfd/stab-syms.c \
	../../../bfd/stabs.c \
	../../../bfd/syms.c \
	../../../bfd/sysdep.h \
	../../../bfd/targets.c \
	../../../bfd/tekhex.c \
	../../../bfd/trad-core.c \
	../../../bfd/vax1knetbsd.c \
	../../../bfd/vaxnetbsd.c \
	../../../bfd/verilog.c \
	../../../bfd/version.h \
	../../../bfd/vms-alpha.c \
	../../../bfd/vms-lib.c \
	../../../bfd/vms-misc.c \
	../../../bfd/vms.h \
	../../../bfd/wasm-module.c \
	../../../bfd/wasm-module.h \
	../../../bfd/xcofflink.c \
	../../../bfd/xcofflink.h \
	../../../bfd/xsym.c \
	../../../bfd/xsym.h \
	../../../bfd/xtensa-isa.c \
	../../../bfd/xtensa-modules.c 

# Note - the following line gets processed by bfd/configure and amended
# to contain the full list of build dir POTFILES.
BLD-POTFILES = \
	../bfd_stdint.h \
	../bfdver.h \
	../elf32-aarch64.c \
	../elf32-ia64.c \
	../elf32-riscv.c \
	../elf32-target.h \
	../elf64-aarch64.c \
	../elf64-ia64.c \
	../elf64-riscv.c \
	../elf64-target.h \
	../peigen.c \
	../pepigen.c \
	../pex64igen.c \
	../targmatch.h 

CATALOGS = 
CATOBJEXT = 
INSTOBJEXT = 

.SUFFIXES:
.SUFFIXES: .c .o .po .pox .gmo .mo .msg .cat

.c.o:
	$(COMPILE) $<

.po.pox:
	$(MAKE) $(PACKAGE).pot
	$(MSGMERGE) $< $(srcdir)/$(PACKAGE).pot -o $*.pox

.po.mo:
	$(MSGFMT) -o $@ $<

.po.gmo:
	file=`echo $* | sed 's,.*/,,'`.gmo \
	  && rm -f $$file && $(GMSGFMT) -o $$file $<

.po.cat:
	sed -f ../intl/po2msg.sed < $< > $*.msg \
	  && rm -f $@ && $(GENCAT) $@ $*.msg


all: all-no

all-yes: $(CATALOGS) # $(PACKAGE).pot
all-no:

$(srcdir)/$(PACKAGE).pot: $(SRC-POTFILES) $(BLD-POTFILES)
	$(XGETTEXT) --default-domain=$(PACKAGE) \
	  --directory=$(top_srcdir) \
	  --add-comments --keyword=_ --keyword=N_ \
	  --msgid-bugs-address=bug-binutils@gnu.org \
	  --files-from=$(srcdir)/SRC-POTFILES.in 
	$(XGETTEXT) --default-domain=$(PACKAGE) \
	  --directory=.. \
	  --directory=. \
	  --add-comments --keyword=_ --keyword=N_ \
	  --join-existing \
	  --msgid-bugs-address=bug-binutils@gnu.org \
	  --files-from=$(srcdir)/BLD-POTFILES.in
	rm -f $(srcdir)/$(PACKAGE).pot
	mv $(PACKAGE).po $(srcdir)/$(PACKAGE).pot

$(srcdir)/cat-id-tbl.c: stamp-cat-id; @:
$(srcdir)/stamp-cat-id: $(PACKAGE).pot
	rm -f cat-id-tbl.tmp
	sed -f ../intl/po2tbl.sed $(srcdir)/$(PACKAGE).pot \
		| sed -e "s/@PACKAGE NAME@/$(PACKAGE)/" > cat-id-tbl.tmp
	if cmp -s cat-id-tbl.tmp $(srcdir)/cat-id-tbl.c; then \
	  rm cat-id-tbl.tmp; \
	else \
	  echo cat-id-tbl.c changed; \
	  rm -f $(srcdir)/cat-id-tbl.c; \
	  mv cat-id-tbl.tmp $(srcdir)/cat-id-tbl.c; \
	fi
	cd $(srcdir) && rm -f stamp-cat-id && echo timestamp > stamp-cat-id


install: install-exec install-data
install-exec:
install-info:
install-html:
install-pdf:
install-data: install-data-no
install-data-no: all
install-data-yes: all
	if test -r $(MKINSTALLDIRS); then \
	  $(MKINSTALLDIRS) $(DESTDIR)$(datadir); \
	else \
	  $(top_srcdir)/mkinstalldirs $(DESTDIR)$(datadir); \
	fi
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  case "$$cat" in \
	    *.gmo) destdir=$(gnulocaledir);; \
	    *)     destdir=$(localedir);; \
	  esac; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  dir=$(DESTDIR)$$destdir/$$lang/LC_MESSAGES; \
	  if test -r $(MKINSTALLDIRS); then \
	    $(MKINSTALLDIRS) $$dir; \
	  else \
	    $(top_srcdir)/mkinstalldirs $$dir; \
	  fi; \
	  if test -r $$cat; then \
	    $(INSTALL_DATA) $$cat $$dir/$(PACKAGE)$(INSTOBJEXT); \
	    echo "installing $$cat as $$dir/$(PACKAGE)$(INSTOBJEXT)"; \
	  else \
	    $(INSTALL_DATA) $(srcdir)/$$cat $$dir/$(PACKAGE)$(INSTOBJEXT); \
	    echo "installing $(srcdir)/$$cat as" \
		 "$$dir/$(PACKAGE)$(INSTOBJEXT)"; \
	  fi; \
	  if test -r $$cat.m; then \
	    $(INSTALL_DATA) $$cat.m $$dir/$(PACKAGE)$(INSTOBJEXT).m; \
	    echo "installing $$cat.m as $$dir/$(PACKAGE)$(INSTOBJEXT).m"; \
	  else \
	    if test -r $(srcdir)/$$cat.m ; then \
	      $(INSTALL_DATA) $(srcdir)/$$cat.m \
		$$dir/$(PACKAGE)$(INSTOBJEXT).m; \
	      echo "installing $(srcdir)/$$cat as" \
		   "$$dir/$(PACKAGE)$(INSTOBJEXT).m"; \
	    else \
	      true; \
	    fi; \
	  fi; \
	done
	if test "$(PACKAGE)" = "gettext"; then \
	  if test -r $(MKINSTALLDIRS); then \
	    $(MKINSTALLDIRS) $(DESTDIR)$(gettextsrcdir); \
	  else \
	    $(top_srcdir)/mkinstalldirs $(DESTDIR)$(gettextsrcdir); \
	  fi; \
	  $(INSTALL_DATA) $(srcdir)/Makefile.in.in \
			  $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	else \
	  : ; \
	fi

# Define this as empty until I found a useful application.
installcheck:

uninstall:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT).m; \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT).m; \
	done
	rm -f $(DESTDIR)$(gettextsrcdir)/po-Makefile.in.in

check: all

cat-id-tbl.o: ../intl/libgettext.h

html dvi pdf ps info tags TAGS ID:

mostlyclean:
	rm -f core core.* *.pox $(PACKAGE).po *.old.po cat-id-tbl.tmp
	rm -fr *.o

clean: mostlyclean

distclean: clean
	rm -f Makefile Makefile.in *.mo *.msg *.cat *.cat.m
	rm -f SRC-POTFILES BLD-POTFILES 

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f $(GMOFILES) SRC-POTFILES.in BLD-POTFILES.in

distdir = ../$(PACKAGE)-$(VERSION)/$(subdir)
dist distdir: update-po $(DISTFILES)
	dists="$(DISTFILES)"; \
	for file in $$dists; do \
	  ln $(srcdir)/$$file $(distdir) 2> /dev/null \
	    || cp -p $(srcdir)/$$file $(distdir); \
	done

update-po: Makefile
	$(MAKE) $(PACKAGE).pot
	PATH=`pwd`/../src:$$PATH; \
	cd $(srcdir); \
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  mv $$lang.po $$lang.old.po; \
	  echo "$$lang:"; \
	  if $(MSGMERGE) $$lang.old.po $(PACKAGE).pot -o $$lang.po; then \
	    rm -f $$lang.old.po; \
	  else \
	    echo "msgmerge for $$cat failed!"; \
	    rm -f $$lang.po; \
	    mv $$lang.old.po $$lang.po; \
	  fi; \
	done

SRC-POTFILES: SRC-POTFILES.in
	( if test 'x$(srcdir)' != 'x.'; then \
	    posrcprefix='$(top_srcdir)/'; \
	  else \
	    posrcprefix="../"; \
	  fi; \
	  rm -f $@-t $@ \
	    && (sed -e '/^#/d' \
	            -e '/^[ 	]*$$/d' \
		    -e "s@.*@	$$posrcprefix& \\\\@" < $(srcdir)/$@.in \
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

BLD-POTFILES: BLD-POTFILES.in
	  ( rm -f $@-t $@ \
	    && (sed -e '/^#/d' \
	            -e '/^[ 	]*$$/d' \
		    -e "s@.*@	../& \\\\@" < $(srcdir)/$@.in \
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

SRC-POTFILES.in: # ../Makefile
	cd .. && $(MAKE) po/SRC-POTFILES.in

BLD-POTFILES.in: # ../Makefile
	cd .. && $(MAKE) po/BLD-POTFILES.in

# Note - The presence of SRC-POTFILES and BLD-POTFILES as dependencies
# here breaks the implementation of the 'distclean' rule for maintainers.
# This is because if 'make distclean' is run in the BFD directory, the
# Makefile there will be deleted before 'distclean' is made here, and so
# the dependency SRC-POTFILES -> SRC-POTFILES.in -> ../Makefile cannot
# be satisfied.
#
# The SRC-POTFILES and BLD-POTFILES dependencies cannot be removed,
# however since it is necessary that these files be built during
# *configure* time, so that configure can insert them into the
# po/Makefile that it is creating, so that the Makefile will have
# the correct dependencies.
Makefile: Make-in ../config.status SRC-POTFILES BLD-POTFILES
	cd .. \
	  && CONFIG_FILES=$(subdir)/Makefile.in:$(subdir)/Make-in \
	     CONFIG_HEADERS= $(SHELL) ./config.status

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
