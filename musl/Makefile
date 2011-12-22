version=0.8.2
build=1
desc=minimalist libc
source=http://www.etalabs.net/musl/releases/$(name)-$(version).tar.gz

include ../make.pkg.inc

build:
	cp ./config.mak $w/config.mak
	cd $w && make clean all install DESTDIR=$d

