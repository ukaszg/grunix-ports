name=foo
version=666
build=1
desc=Tiny bar for a baz.
source=http://example.net/$(name)-$(version).tar.gz

include ../make.pkg.inc

build:
	cd $w && ./configure \
		--prefix=/ \
		--disable-shared \
		--disable-nls \
		--mandir=/share/man
	cd $w && make && make install DESTDIR=$d 

