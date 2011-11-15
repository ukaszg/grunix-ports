name=musl
version=0.7.12
build=1
desc=minimalist libc
source=http://www.etalabs.net/musl/releases/$(name)-$(version).tar.gz

include ../Makefile

build:
	cp ./config.mak $w/config.mak
	cd $w && make && make install DESTDIR=$d

