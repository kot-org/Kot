# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
# Copyright (C) 2003-2020 Free Software Foundation, Inc.
#
# This file may be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

PACKAGE = opcodes
VERSION = 2.35

SHELL = /bin/sh


srcdir = ../../../opcodes/po
top_srcdir = ../../../opcodes
VPATH = ../../../opcodes/po
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
MKINSTALLDIRS = $(top_builddir)/../../opcodes/../mkinstalldirs

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
POFILES =  $(srcdir)/da.po $(srcdir)/de.po $(srcdir)/es.po $(srcdir)/fi.po $(srcdir)/fr.po $(srcdir)/ga.po $(srcdir)/id.po $(srcdir)/it.po $(srcdir)/nl.po $(srcdir)/pt_BR.po $(srcdir)/ro.po $(srcdir)/sr.po $(srcdir)/sv.po $(srcdir)/tr.po $(srcdir)/uk.po $(srcdir)/vi.po $(srcdir)/zh_CN.po
GMOFILES =  $(srcdir)/da.gmo $(srcdir)/de.gmo $(srcdir)/es.gmo $(srcdir)/fi.gmo $(srcdir)/fr.gmo $(srcdir)/ga.gmo $(srcdir)/id.gmo $(srcdir)/it.gmo $(srcdir)/nl.gmo $(srcdir)/pt_BR.gmo $(srcdir)/ro.gmo $(srcdir)/sr.gmo $(srcdir)/sv.gmo $(srcdir)/tr.gmo $(srcdir)/uk.gmo $(srcdir)/vi.gmo $(srcdir)/zh_CN.gmo
DISTFILES = ChangeLog Makefile.in.in POTFILES.in $(PACKAGE).pot \
stamp-cat-id $(POFILES) $(GMOFILES) $(SOURCES)

POTFILES = \
     ../../../opcodes/aarch64-asm-2.c \
     ../../../opcodes/aarch64-asm.c \
     ../../../opcodes/aarch64-asm.h \
     ../../../opcodes/aarch64-dis-2.c \
     ../../../opcodes/aarch64-dis.c \
     ../../../opcodes/aarch64-dis.h \
     ../../../opcodes/aarch64-gen.c \
     ../../../opcodes/aarch64-opc-2.c \
     ../../../opcodes/aarch64-opc.c \
     ../../../opcodes/aarch64-opc.h \
     ../../../opcodes/aarch64-tbl.h \
     ../../../opcodes/alpha-dis.c \
     ../../../opcodes/alpha-opc.c \
     ../../../opcodes/arc-dis.c \
     ../../../opcodes/arc-ext.c \
     ../../../opcodes/arc-opc.c \
     ../../../opcodes/arm-dis.c \
     ../../../opcodes/avr-dis.c \
     ../../../opcodes/bfin-dis.c \
     ../../../opcodes/bpf-asm.c \
     ../../../opcodes/bpf-desc.c \
     ../../../opcodes/bpf-desc.h \
     ../../../opcodes/bpf-dis.c \
     ../../../opcodes/bpf-ibld.c \
     ../../../opcodes/bpf-opc.c \
     ../../../opcodes/bpf-opc.h \
     ../../../opcodes/cgen-asm.c \
     ../../../opcodes/cgen-bitset.c \
     ../../../opcodes/cgen-dis.c \
     ../../../opcodes/cgen-opc.c \
     ../../../opcodes/cr16-dis.c \
     ../../../opcodes/cr16-opc.c \
     ../../../opcodes/cris-dis.c \
     ../../../opcodes/cris-opc.c \
     ../../../opcodes/crx-dis.c \
     ../../../opcodes/crx-opc.c \
     ../../../opcodes/csky-dis.c \
     ../../../opcodes/d10v-dis.c \
     ../../../opcodes/d10v-opc.c \
     ../../../opcodes/d30v-dis.c \
     ../../../opcodes/d30v-opc.c \
     ../../../opcodes/dis-buf.c \
     ../../../opcodes/dis-init.c \
     ../../../opcodes/disassemble.c \
     ../../../opcodes/dlx-dis.c \
     ../../../opcodes/epiphany-asm.c \
     ../../../opcodes/epiphany-desc.c \
     ../../../opcodes/epiphany-desc.h \
     ../../../opcodes/epiphany-dis.c \
     ../../../opcodes/epiphany-ibld.c \
     ../../../opcodes/epiphany-opc.c \
     ../../../opcodes/epiphany-opc.h \
     ../../../opcodes/fr30-asm.c \
     ../../../opcodes/fr30-desc.c \
     ../../../opcodes/fr30-desc.h \
     ../../../opcodes/fr30-dis.c \
     ../../../opcodes/fr30-ibld.c \
     ../../../opcodes/fr30-opc.c \
     ../../../opcodes/fr30-opc.h \
     ../../../opcodes/frv-asm.c \
     ../../../opcodes/frv-desc.c \
     ../../../opcodes/frv-desc.h \
     ../../../opcodes/frv-dis.c \
     ../../../opcodes/frv-ibld.c \
     ../../../opcodes/frv-opc.c \
     ../../../opcodes/frv-opc.h \
     ../../../opcodes/ft32-dis.c \
     ../../../opcodes/ft32-opc.c \
     ../../../opcodes/h8300-dis.c \
     ../../../opcodes/hppa-dis.c \
     ../../../opcodes/i386-dis.c \
     ../../../opcodes/i386-gen.c \
     ../../../opcodes/i386-init.h \
     ../../../opcodes/i386-opc.c \
     ../../../opcodes/i386-opc.h \
     ../../../opcodes/i386-tbl.h \
     ../../../opcodes/ia64-asmtab.c \
     ../../../opcodes/ia64-asmtab.h \
     ../../../opcodes/ia64-dis.c \
     ../../../opcodes/ia64-gen.c \
     ../../../opcodes/ia64-opc-a.c \
     ../../../opcodes/ia64-opc-b.c \
     ../../../opcodes/ia64-opc-d.c \
     ../../../opcodes/ia64-opc-f.c \
     ../../../opcodes/ia64-opc-i.c \
     ../../../opcodes/ia64-opc-m.c \
     ../../../opcodes/ia64-opc.c \
     ../../../opcodes/ia64-opc.h \
     ../../../opcodes/ip2k-asm.c \
     ../../../opcodes/ip2k-desc.c \
     ../../../opcodes/ip2k-desc.h \
     ../../../opcodes/ip2k-dis.c \
     ../../../opcodes/ip2k-ibld.c \
     ../../../opcodes/ip2k-opc.c \
     ../../../opcodes/ip2k-opc.h \
     ../../../opcodes/iq2000-asm.c \
     ../../../opcodes/iq2000-desc.c \
     ../../../opcodes/iq2000-desc.h \
     ../../../opcodes/iq2000-dis.c \
     ../../../opcodes/iq2000-ibld.c \
     ../../../opcodes/iq2000-opc.c \
     ../../../opcodes/iq2000-opc.h \
     ../../../opcodes/lm32-asm.c \
     ../../../opcodes/lm32-desc.c \
     ../../../opcodes/lm32-desc.h \
     ../../../opcodes/lm32-dis.c \
     ../../../opcodes/lm32-ibld.c \
     ../../../opcodes/lm32-opc.c \
     ../../../opcodes/lm32-opc.h \
     ../../../opcodes/lm32-opinst.c \
     ../../../opcodes/m10200-dis.c \
     ../../../opcodes/m10200-opc.c \
     ../../../opcodes/m10300-dis.c \
     ../../../opcodes/m10300-opc.c \
     ../../../opcodes/m32c-asm.c \
     ../../../opcodes/m32c-desc.c \
     ../../../opcodes/m32c-desc.h \
     ../../../opcodes/m32c-dis.c \
     ../../../opcodes/m32c-ibld.c \
     ../../../opcodes/m32c-opc.c \
     ../../../opcodes/m32c-opc.h \
     ../../../opcodes/m32r-asm.c \
     ../../../opcodes/m32r-desc.c \
     ../../../opcodes/m32r-desc.h \
     ../../../opcodes/m32r-dis.c \
     ../../../opcodes/m32r-ibld.c \
     ../../../opcodes/m32r-opc.c \
     ../../../opcodes/m32r-opc.h \
     ../../../opcodes/m32r-opinst.c \
     ../../../opcodes/m68hc11-dis.c \
     ../../../opcodes/m68hc11-opc.c \
     ../../../opcodes/m68k-dis.c \
     ../../../opcodes/m68k-opc.c \
     ../../../opcodes/mcore-dis.c \
     ../../../opcodes/mcore-opc.h \
     ../../../opcodes/mep-asm.c \
     ../../../opcodes/mep-desc.c \
     ../../../opcodes/mep-desc.h \
     ../../../opcodes/mep-dis.c \
     ../../../opcodes/mep-ibld.c \
     ../../../opcodes/mep-opc.c \
     ../../../opcodes/mep-opc.h \
     ../../../opcodes/metag-dis.c \
     ../../../opcodes/microblaze-dis.c \
     ../../../opcodes/microblaze-opc.h \
     ../../../opcodes/micromips-opc.c \
     ../../../opcodes/mips-dis.c \
     ../../../opcodes/mips-opc.c \
     ../../../opcodes/mips16-opc.c \
     ../../../opcodes/mmix-dis.c \
     ../../../opcodes/mmix-opc.c \
     ../../../opcodes/moxie-dis.c \
     ../../../opcodes/moxie-opc.c \
     ../../../opcodes/msp430-decode.c \
     ../../../opcodes/msp430-dis.c \
     ../../../opcodes/mt-asm.c \
     ../../../opcodes/mt-desc.c \
     ../../../opcodes/mt-desc.h \
     ../../../opcodes/mt-dis.c \
     ../../../opcodes/mt-ibld.c \
     ../../../opcodes/mt-opc.c \
     ../../../opcodes/mt-opc.h \
     ../../../opcodes/nds32-asm.c \
     ../../../opcodes/nds32-dis.c \
     ../../../opcodes/nfp-dis.c \
     ../../../opcodes/nios2-dis.c \
     ../../../opcodes/nios2-opc.c \
     ../../../opcodes/ns32k-dis.c \
     ../../../opcodes/or1k-asm.c \
     ../../../opcodes/or1k-desc.c \
     ../../../opcodes/or1k-dis.c \
     ../../../opcodes/or1k-ibld.c \
     ../../../opcodes/or1k-opc.c \
     ../../../opcodes/pdp11-dis.c \
     ../../../opcodes/pdp11-opc.c \
     ../../../opcodes/pj-dis.c \
     ../../../opcodes/pj-opc.c \
     ../../../opcodes/ppc-dis.c \
     ../../../opcodes/ppc-opc.c \
     ../../../opcodes/pru-dis.c \
     ../../../opcodes/pru-opc.c \
     ../../../opcodes/riscv-dis.c \
     ../../../opcodes/riscv-opc.c \
     ../../../opcodes/rl78-decode.c \
     ../../../opcodes/rl78-dis.c \
     ../../../opcodes/rx-decode.c \
     ../../../opcodes/rx-dis.c \
     ../../../opcodes/s12z-dis.c \
     ../../../opcodes/s12z-opc.c \
     ../../../opcodes/s390-dis.c \
     ../../../opcodes/s390-mkopc.c \
     ../../../opcodes/s390-opc.c \
     ../../../opcodes/score-dis.c \
     ../../../opcodes/score-opc.h \
     ../../../opcodes/score7-dis.c \
     ../../../opcodes/sh-dis.c \
     ../../../opcodes/sh-opc.h \
     ../../../opcodes/sparc-dis.c \
     ../../../opcodes/sparc-opc.c \
     ../../../opcodes/spu-dis.c \
     ../../../opcodes/spu-opc.c \
     ../../../opcodes/sysdep.h \
     ../../../opcodes/tic30-dis.c \
     ../../../opcodes/tic4x-dis.c \
     ../../../opcodes/tic54x-dis.c \
     ../../../opcodes/tic54x-opc.c \
     ../../../opcodes/tic6x-dis.c \
     ../../../opcodes/tilegx-dis.c \
     ../../../opcodes/tilegx-opc.c \
     ../../../opcodes/tilepro-dis.c \
     ../../../opcodes/tilepro-opc.c \
     ../../../opcodes/v850-dis.c \
     ../../../opcodes/v850-opc.c \
     ../../../opcodes/vax-dis.c \
     ../../../opcodes/visium-dis.c \
     ../../../opcodes/visium-opc.c \
     ../../../opcodes/wasm32-dis.c \
     ../../../opcodes/xc16x-asm.c \
     ../../../opcodes/xc16x-desc.c \
     ../../../opcodes/xc16x-desc.h \
     ../../../opcodes/xc16x-dis.c \
     ../../../opcodes/xc16x-ibld.c \
     ../../../opcodes/xc16x-opc.c \
     ../../../opcodes/xc16x-opc.h \
     ../../../opcodes/xgate-dis.c \
     ../../../opcodes/xgate-opc.c \
     ../../../opcodes/xstormy16-asm.c \
     ../../../opcodes/xstormy16-desc.c \
     ../../../opcodes/xstormy16-desc.h \
     ../../../opcodes/xstormy16-dis.c \
     ../../../opcodes/xstormy16-ibld.c \
     ../../../opcodes/xstormy16-opc.c \
     ../../../opcodes/xstormy16-opc.h \
     ../../../opcodes/xtensa-dis.c \
     ../../../opcodes/z80-dis.c \
     ../../../opcodes/z8k-dis.c \
     ../../../opcodes/z8k-opc.h \
     ../../../opcodes/z8kgen.c

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
