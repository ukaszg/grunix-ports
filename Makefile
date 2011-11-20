.PHONY: help build clean install nuke list

.DEFAULT_GOAL=list

##############
#    HELP    #
help:
	@echo todo

##############
#  DOWNLOAD  #
files=$(foreach f, $(source), $(notdir $(f)))

download:: $(files)

%.tar.bz2 %.tar.gz %tar.xz %.zip:
	@wget -c -O "$@.part" "$(filter %$@,$(source))"
	@mv "$@.part" "$@"
	@touch "$@"

##############
#   PREPARE  #
# build directory
W=$(CURDIR)/work
unpack=$(addprefix $W/.unpack.,$(files))

prepare:: $(unpack)

$W:
	@mkdir -p $@

$W/.unpack.%.tar.bz2: %.tar.bz2 $W
	bzip2 -d -c $< | tar -x -C $W -f -
	@touch "$@"

$W/.unpack.%.tar.gz: %.tar.gz $W
	gzip  -d -c $< | tar -x -C $W -f -
	@touch "$@"

$W/.unpack.%.tar.xz: %.tar.xz $W
	xz    -d -c $< | tar -x -C $W -f -
	@touch "$@"

$W/.unpack.%.zip: %.zip $W
	unzip -d $W $<
	@touch "$@"

##############
#    BUILD   #
# Most tarballs have "name-version" directory packed inside.
# It's nice to type less, most of the time. Do not use it in this file.
w=$W/$(name)-$(version)

d=$(CURDIR)/dest
# advanced anti-fingerfart measures
D=d

build: prepare $d

$d:
	@mkdir -p $@

##############
#   PACKAGE  #
pkgext=pkg.tar.gz
packagename=$(name)-$(version)-$(build).$(pkgext)

package:: $(packagename)

$(packagename):
	@$(MAKE) build
	cd $d && tar -cpf - * | gzip > $(CURDIR)/$(packagename)

list: $(packagename)
# I'd like to see it, when it pops out after a wall of text.
	@$(info  )
	@$(info  )
	@$(info # $(packagename) was built successfully.)
	@$(info # ====== Package contents ====== )
	@$(info  )
	@tar tf "$(packagename)" | grep -e '.*[^/]$$'

##############
#   INSTALL  #
package_installed=$(CURDIR)/.installed

install: is_it_already_installed?

is_it_already_installed?:
# how do you break that line, while not breaking the script?
	@test ! -f $(package_installed) || echo 'Package already installed. Did you mean `update`?' && exit 1


##############
#    CLEAN   #
clean:
	@-rm -rf $W
	@-rm -rf $d

nuke: clean
	@-rm -rf $(files) $(packagename)

