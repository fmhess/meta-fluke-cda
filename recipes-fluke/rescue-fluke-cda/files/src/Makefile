prefix ?= /usr/local
exec_prefix ?= $(prefix)
sbindir ?= $(exec_prefix)/sbin

CFLAGS ?= -Wall

INSTALL ?= install
INSTALL_PROGRAM ?= $(INSTALL)

.PHONY: all
all: rescue_fluke_cda

.PHONY: clean
clean:
	$(RM) rescue_fluke_cda

.PHONY: install
install: rescue_fluke_cda
	$(INSTALL_PROGRAM) -D rescue_fluke_cda $(DESTDIR)$(sbindir)/rescue_fluke_cda
