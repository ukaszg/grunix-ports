.DEFAULT_GOAL=package
.PHONY: help prepare build package clean install distclean

##############
#    HELP    #
help:
	@echo todo

##############
#  DOWNLOAD  #
files=$(foreach f, $(source), $(notdir $(f)))

download:: $(files)

%.tar.bz2 %.tar.gz %tar.xz %.zip:
	@wget -c -O "$(@).part" "$(filter %$(@),$(source))"
	@mv "$(@).part" "$(@)"
	@touch "$(@)"

##############
#   PREPARE  #
W=./work
unpack=$(addprefix $(W)/.unpack.,$(files))

prepare: $(unpack)

$(W)/.unpack.%.tar.bz2: %.tar.bz2 $(W)
	bzip2 -d -c $< | tar -x -C $(W) -f -
	@touch "$(@)"

$(W)/.unpack.%.tar.gz: %.tar.gz $(W)
	gzip  -d -c $< | tar -x -C $(W) -f -
	@touch "$(@)"

$(W)/.unpack.%.tar.xz: %.tar.xz $(W)
	xz    -d -c $< | tar -x -C $(W) -f -
	@touch "$(@)"

$(W)/.unpack.%.zip: %.zip $(W)
	unzip -d $(W) $<
	@touch "$(@)"

$(W):
	@mkdir -p $(W)


##############
#    BUILD   #
d=dest
w=$W/$(name)-$(version)

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

