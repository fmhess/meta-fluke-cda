# Makefile

EXT_MODULE_SRCDIR ?= $(CURDIR)
LINUX_SRCDIR ?= /lib/modules/`uname -r`/build
VERBOSE ?= 0

all:
	$(MAKE) -C $(LINUX_SRCDIR) V=$(VERBOSE) modules \
		M="$(EXT_MODULE_SRCDIR)/drivers"

clean:
	-$(MAKE) -C $(LINUX_SRCDIR) V=$(VERBOSE) clean \
		M="$(EXT_MODULE_SRCDIR)/drivers"

install:
	-$(MAKE) -C $(LINUX_SRCDIR) V=$(VERBOSE) modules_install \
		M="$(EXT_MODULE_SRCDIR)/drivers"
