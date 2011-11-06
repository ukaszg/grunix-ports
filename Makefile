.DEFAULT_GOAL=package
.PHONY: help download prepare build package clean install distclean

##############
#    HELP    #
help:
	@echo todo

##############
#  DOWNLOAD  #
files=$(foreach f, $(source), $(notdir $(f)))

download: $(files)

%.tar.bz2 %.tar.gz %tar.xz %.zip:
	wget -c -O "$(@).tmp" "$(filter %$(@),$(source))"
	@mv "$(@).tmp" "$(@)"

##############
#   PREPARE  #
unpack=$(addprefix unpack.,$(files))
W=work

prepare: $(unpack)

unpack.%.tar.bz2: %.tar.bz2 $(W)
	bzip2 -d -c $< | tar -x -C $(W) -f -

unpack.%.tar.gz: %.tar.gz $(W)
	gzip  -d -c $< | tar -x -C $(W) -f -

unpack.%.tar.xz: %.tar.xz $(W)
	xz    -d -c $< | tar -x -C $(W) -f -

unpack.%.zip: %.zip $(W)
	unzip -d $(W) $<

$(W):
	@mkdir -p $(W)


##############
#    BUILD   #
D=dest

build: prepare $(D)

$(D):
	@mkdir -p $(D)

##############
#   PACKAGE  #
pkgext=pkg.tar.xz
packagename=$(name)-$(version)-$(build).$(pkgext)

package: $(packagename)

$(packagename): build
	@tar -cpf - $(D)/* | xz > $(packagename)

##############
#   INSTALL  #
install: package
	@echo todo

##############
#    CLEAN   #
clean:
	@rm -rf $(W)
	@rm -rf $(D)

distclean: clean
	@-rm -rf $(files) $(packagename)

