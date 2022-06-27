# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
# Copyright (C) 2003-2020 Free Software Foundation, Inc.
#
# This file may be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

PACKAGE = gas
VERSION = 2.35

SHELL = /bin/sh


srcdir = ../../../gas/po
top_srcdir = ../../../gas
VPATH = ../../../gas/po
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
MKINSTALLDIRS = $(top_builddir)/../../gas/../mkinstalldirs

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
POFILES =  $(srcdir)/es.po $(srcdir)/fi.po $(srcdir)/fr.po $(srcdir)/id.po $(srcdir)/ja.po $(srcdir)/ru.po $(srcdir)/rw.po $(srcdir)/sv.po $(srcdir)/tr.po $(srcdir)/uk.po $(srcdir)/zh_CN.po
GMOFILES =  $(srcdir)/es.gmo $(srcdir)/fi.gmo $(srcdir)/fr.gmo $(srcdir)/id.gmo $(srcdir)/ja.gmo $(srcdir)/ru.gmo $(srcdir)/rw.gmo $(srcdir)/sv.gmo $(srcdir)/tr.gmo $(srcdir)/uk.gmo $(srcdir)/zh_CN.gmo
DISTFILES = ChangeLog Makefile.in.in POTFILES.in $(PACKAGE).pot \
stamp-cat-id $(POFILES) $(GMOFILES) $(SOURCES)

POTFILES = \
     ../../../gas/app.c \
     ../../../gas/as.c \
     ../../../gas/as.h \
     ../../../gas/asintl.h \
     ../../../gas/atof-generic.c \
     ../../../gas/bignum.h \
     ../../../gas/bit_fix.h \
     ../../../gas/cgen.c \
     ../../../gas/cgen.h \
     ../../../gas/compress-debug.c \
     ../../../gas/compress-debug.h \
     ../../../gas/cond.c \
     ../../../gas/config/atof-ieee.c \
     ../../../gas/config/atof-vax.c \
     ../../../gas/config/e-crisaout.c \
     ../../../gas/config/e-criself.c \
     ../../../gas/config/e-i386aout.c \
     ../../../gas/config/e-i386coff.c \
     ../../../gas/config/e-i386elf.c \
     ../../../gas/config/e-mipself.c \
     ../../../gas/config/obj-aout.c \
     ../../../gas/config/obj-aout.h \
     ../../../gas/config/obj-coff.c \
     ../../../gas/config/obj-coff.h \
     ../../../gas/config/obj-ecoff.c \
     ../../../gas/config/obj-ecoff.h \
     ../../../gas/config/obj-elf.c \
     ../../../gas/config/obj-elf.h \
     ../../../gas/config/obj-evax.c \
     ../../../gas/config/obj-evax.h \
     ../../../gas/config/obj-fdpicelf.c \
     ../../../gas/config/obj-fdpicelf.h \
     ../../../gas/config/obj-macho.c \
     ../../../gas/config/obj-macho.h \
     ../../../gas/config/obj-multi.c \
     ../../../gas/config/obj-multi.h \
     ../../../gas/config/obj-som.c \
     ../../../gas/config/obj-som.h \
     ../../../gas/config/tc-aarch64.c \
     ../../../gas/config/tc-aarch64.h \
     ../../../gas/config/tc-alpha.c \
     ../../../gas/config/tc-alpha.h \
     ../../../gas/config/tc-arc.c \
     ../../../gas/config/tc-arc.h \
     ../../../gas/config/tc-arm.c \
     ../../../gas/config/tc-arm.h \
     ../../../gas/config/tc-avr.c \
     ../../../gas/config/tc-avr.h \
     ../../../gas/config/tc-bfin.c \
     ../../../gas/config/tc-bfin.h \
     ../../../gas/config/tc-bpf.c \
     ../../../gas/config/tc-bpf.h \
     ../../../gas/config/tc-cr16.c \
     ../../../gas/config/tc-cr16.h \
     ../../../gas/config/tc-cris.c \
     ../../../gas/config/tc-cris.h \
     ../../../gas/config/tc-crx.c \
     ../../../gas/config/tc-crx.h \
     ../../../gas/config/tc-csky.c \
     ../../../gas/config/tc-csky.h \
     ../../../gas/config/tc-d10v.c \
     ../../../gas/config/tc-d10v.h \
     ../../../gas/config/tc-d30v.c \
     ../../../gas/config/tc-d30v.h \
     ../../../gas/config/tc-dlx.c \
     ../../../gas/config/tc-dlx.h \
     ../../../gas/config/tc-epiphany.c \
     ../../../gas/config/tc-epiphany.h \
     ../../../gas/config/tc-fr30.c \
     ../../../gas/config/tc-fr30.h \
     ../../../gas/config/tc-frv.c \
     ../../../gas/config/tc-frv.h \
     ../../../gas/config/tc-ft32.c \
     ../../../gas/config/tc-ft32.h \
     ../../../gas/config/tc-h8300.c \
     ../../../gas/config/tc-h8300.h \
     ../../../gas/config/tc-hppa.c \
     ../../../gas/config/tc-hppa.h \
     ../../../gas/config/tc-i386.c \
     ../../../gas/config/tc-i386.h \
     ../../../gas/config/tc-ia64.c \
     ../../../gas/config/tc-ia64.h \
     ../../../gas/config/tc-ip2k.c \
     ../../../gas/config/tc-ip2k.h \
     ../../../gas/config/tc-iq2000.c \
     ../../../gas/config/tc-iq2000.h \
     ../../../gas/config/tc-lm32.c \
     ../../../gas/config/tc-lm32.h \
     ../../../gas/config/tc-m32c.c \
     ../../../gas/config/tc-m32c.h \
     ../../../gas/config/tc-m32r.c \
     ../../../gas/config/tc-m32r.h \
     ../../../gas/config/tc-m68hc11.c \
     ../../../gas/config/tc-m68hc11.h \
     ../../../gas/config/tc-m68k.c \
     ../../../gas/config/tc-m68k.h \
     ../../../gas/config/tc-mcore.c \
     ../../../gas/config/tc-mcore.h \
     ../../../gas/config/tc-mep.c \
     ../../../gas/config/tc-mep.h \
     ../../../gas/config/tc-metag.c \
     ../../../gas/config/tc-metag.h \
     ../../../gas/config/tc-microblaze.c \
     ../../../gas/config/tc-microblaze.h \
     ../../../gas/config/tc-mips.c \
     ../../../gas/config/tc-mips.h \
     ../../../gas/config/tc-mmix.c \
     ../../../gas/config/tc-mmix.h \
     ../../../gas/config/tc-mn10200.c \
     ../../../gas/config/tc-mn10200.h \
     ../../../gas/config/tc-mn10300.c \
     ../../../gas/config/tc-mn10300.h \
     ../../../gas/config/tc-moxie.c \
     ../../../gas/config/tc-msp430.c \
     ../../../gas/config/tc-msp430.h \
     ../../../gas/config/tc-mt.c \
     ../../../gas/config/tc-mt.h \
     ../../../gas/config/tc-nds32.c \
     ../../../gas/config/tc-nds32.h \
     ../../../gas/config/tc-nios2.c \
     ../../../gas/config/tc-nios2.h \
     ../../../gas/config/tc-ns32k.c \
     ../../../gas/config/tc-ns32k.h \
     ../../../gas/config/tc-or1k.c \
     ../../../gas/config/tc-or1k.h \
     ../../../gas/config/tc-pdp11.c \
     ../../../gas/config/tc-pdp11.h \
     ../../../gas/config/tc-pj.c \
     ../../../gas/config/tc-pj.h \
     ../../../gas/config/tc-ppc.c \
     ../../../gas/config/tc-ppc.h \
     ../../../gas/config/tc-pru.c \
     ../../../gas/config/tc-pru.h \
     ../../../gas/config/tc-riscv.c \
     ../../../gas/config/tc-riscv.h \
     ../../../gas/config/tc-rl78.c \
     ../../../gas/config/tc-rl78.h \
     ../../../gas/config/tc-rx.c \
     ../../../gas/config/tc-rx.h \
     ../../../gas/config/tc-s12z.c \
     ../../../gas/config/tc-s12z.h \
     ../../../gas/config/tc-s390.c \
     ../../../gas/config/tc-s390.h \
     ../../../gas/config/tc-score.c \
     ../../../gas/config/tc-score.h \
     ../../../gas/config/tc-sh.c \
     ../../../gas/config/tc-sh.h \
     ../../../gas/config/tc-sparc.c \
     ../../../gas/config/tc-sparc.h \
     ../../../gas/config/tc-spu.c \
     ../../../gas/config/tc-spu.h \
     ../../../gas/config/tc-tic30.c \
     ../../../gas/config/tc-tic30.h \
     ../../../gas/config/tc-tic4x.c \
     ../../../gas/config/tc-tic4x.h \
     ../../../gas/config/tc-tic54x.c \
     ../../../gas/config/tc-tic54x.h \
     ../../../gas/config/tc-tic6x.c \
     ../../../gas/config/tc-tic6x.h \
     ../../../gas/config/tc-tilegx.c \
     ../../../gas/config/tc-tilegx.h \
     ../../../gas/config/tc-tilepro.c \
     ../../../gas/config/tc-tilepro.h \
     ../../../gas/config/tc-v850.c \
     ../../../gas/config/tc-v850.h \
     ../../../gas/config/tc-vax.c \
     ../../../gas/config/tc-vax.h \
     ../../../gas/config/tc-visium.c \
     ../../../gas/config/tc-visium.h \
     ../../../gas/config/tc-wasm32.c \
     ../../../gas/config/tc-wasm32.h \
     ../../../gas/config/tc-xc16x.c \
     ../../../gas/config/tc-xc16x.h \
     ../../../gas/config/tc-xgate.c \
     ../../../gas/config/tc-xgate.h \
     ../../../gas/config/tc-xstormy16.c \
     ../../../gas/config/tc-xstormy16.h \
     ../../../gas/config/tc-xtensa.c \
     ../../../gas/config/tc-xtensa.h \
     ../../../gas/config/tc-z80.c \
     ../../../gas/config/tc-z80.h \
     ../../../gas/config/tc-z8k.c \
     ../../../gas/config/tc-z8k.h \
     ../../../gas/config/te-386bsd.h \
     ../../../gas/config/te-aix5.h \
     ../../../gas/config/te-armeabi.h \
     ../../../gas/config/te-armfbsdeabi.h \
     ../../../gas/config/te-armfbsdvfp.h \
     ../../../gas/config/te-armlinuxeabi.h \
     ../../../gas/config/te-csky_abiv1.h \
     ../../../gas/config/te-csky_abiv1_linux.h \
     ../../../gas/config/te-csky_abiv2.h \
     ../../../gas/config/te-csky_abiv2_linux.h \
     ../../../gas/config/te-freebsd.h \
     ../../../gas/config/te-generic.h \
     ../../../gas/config/te-gnu.h \
     ../../../gas/config/te-go32.h \
     ../../../gas/config/te-hppa.h \
     ../../../gas/config/te-hppa64.h \
     ../../../gas/config/te-hppalinux64.h \
     ../../../gas/config/te-ia64aix.h \
     ../../../gas/config/te-interix.h \
     ../../../gas/config/te-lynx.h \
     ../../../gas/config/te-macos.h \
     ../../../gas/config/te-nbsd.h \
     ../../../gas/config/te-nbsd532.h \
     ../../../gas/config/te-pc532mach.h \
     ../../../gas/config/te-pe.h \
     ../../../gas/config/te-solaris.h \
     ../../../gas/config/te-svr4.h \
     ../../../gas/config/te-symbian.h \
     ../../../gas/config/te-tmips.h \
     ../../../gas/config/te-vms.c \
     ../../../gas/config/te-vxworks.h \
     ../../../gas/config/te-wince-pe.h \
     ../../../gas/config/xtensa-relax.c \
     ../../../gas/config/xtensa-relax.h \
     ../../../gas/depend.c \
     ../../../gas/dw2gencfi.c \
     ../../../gas/dw2gencfi.h \
     ../../../gas/dwarf2dbg.c \
     ../../../gas/dwarf2dbg.h \
     ../../../gas/ecoff.c \
     ../../../gas/ecoff.h \
     ../../../gas/ehopt.c \
     ../../../gas/emul-target.h \
     ../../../gas/emul.h \
     ../../../gas/expr.c \
     ../../../gas/expr.h \
     ../../../gas/flonum-copy.c \
     ../../../gas/flonum-konst.c \
     ../../../gas/flonum-mult.c \
     ../../../gas/flonum.h \
     ../../../gas/frags.c \
     ../../../gas/frags.h \
     ../../../gas/hash.c \
     ../../../gas/hash.h \
     ../../../gas/input-file.c \
     ../../../gas/input-file.h \
     ../../../gas/input-scrub.c \
     ../../../gas/itbl-lex.h \
     ../../../gas/itbl-ops.c \
     ../../../gas/itbl-ops.h \
     ../../../gas/listing.c \
     ../../../gas/listing.h \
     ../../../gas/literal.c \
     ../../../gas/macro.c \
     ../../../gas/macro.h \
     ../../../gas/messages.c \
     ../../../gas/obj.h \
     ../../../gas/output-file.c \
     ../../../gas/output-file.h \
     ../../../gas/read.c \
     ../../../gas/read.h \
     ../../../gas/remap.c \
     ../../../gas/sb.c \
     ../../../gas/sb.h \
     ../../../gas/stabs.c \
     ../../../gas/subsegs.c \
     ../../../gas/subsegs.h \
     ../../../gas/symbols.c \
     ../../../gas/symbols.h \
     ../../../gas/tc.h \
     ../../../gas/write.c \
     ../../../gas/write.h

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

$(srcdir)/$(PACKAGE).pot: $(POTFILES)
	$(XGETTEXT) --default-domain=$(PACKAGE) --directory=$(top_srcdir) \
	  --add-comments --keyword=_ --keyword=N_ \
	  --msgid-bugs-address=bug-binutils@gnu.org \
	  --files-from=$(srcdir)/POTFILES.in
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
	rm -f Makefile Makefile.in POTFILES *.mo *.msg *.cat *.cat.m

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f $(GMOFILES)

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

POTFILES: POTFILES.in
	( if test 'x$(srcdir)' != 'x.'; then \
	    posrcprefix='$(top_srcdir)/'; \
	  else \
	    posrcprefix="../"; \
	  fi; \
	  rm -f $@-t $@ \
	    && (sed -e '/^#/d' -e '/^[ 	]*$$/d' \
		    -e "s@.*@	$$posrcprefix& \\\\@" < $(srcdir)/$@.in \
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

POTFILES.in: # ../Makefile
	cd .. && $(MAKE) po/POTFILES.in

Makefile: Make-in ../config.status POTFILES
	cd .. \
	  && CONFIG_FILES=$(subdir)/Makefile.in:$(subdir)/Make-in \
	     CONFIG_HEADERS= $(SHELL) ./config.status

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
