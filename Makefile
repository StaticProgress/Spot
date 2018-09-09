TOPDIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)

all:
	cd src; $(MAKE) "TOPDIR=$(TOPDIR)"

clean:
	rm -rf build obj
