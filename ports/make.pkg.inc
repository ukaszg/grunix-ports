.DEFAULT_GOAL=all
SHELL=/bin/sh

attr?=user.package
checksum?=sha256sum

W=$(CURDIR)/work
d=$(CURDIR)/out
db_dir=$(CURDIR)/../../db/$(name)
ifeq ($(shell id -u),0)
root=/
else
root=$(shell echo "${HOME}/.local/")
endif

name=$(shell basename $(CURDIR))
pkg=$(name)-$(version).$(build).pkg

# macros that help with ea-related stuff
counthash_f=$(checksum) $(1)|cut -d' ' -f1
seteasum_f=setfattr -n $(attr).$(checksum) -v `$(counthash_f)` "$(1)" 2>/dev/null

.PHONY: clean nuke ls list
.SUFFIXES:

$W $d:
	@install -m 700 -d "$@"

$(root):
	@install -m 755 -d "$@"

installed_version=$(shell test -f "$(db_dir)/version" && cat "$(db_dir)/version" || echo "not installed")
is_upgradeable=$(shell test "x$(installed_version)" = "x$(version).$(build)" && echo "no" || echo "yes")

##############
#    REMOVE  #
remove uninstall:
	@echo "# removing $(name)" >&2

##############
#   INSTALL  #
ifneq "$(installed_version)" "not installed"
ifeq "$(is_upgradeable)" "yes" 
install: remove
endif
endif

install: $(pkg) $(root)
	@test ! -d $(db_dir) || { \
		echo "# $(name) is already installed" >&2; exit 1; }
	@-echo "# installing: $<" >&2
	@bsdtar -t -f "$(pkg)" | sed -e 's#^\./##' > filelist.part
	@echo "$(version).$(build)" > version.part
	@$(call seteasum_f,filelist.part)
	@$(call seteasum_f,version.part)
	@bsdtar -x -p -C "$(root)" -f "$(pkg)"
	@mv filelist.part filelist
	@mv version.part version
	@mkdir -p $(db_dir)
	@install -m 600 version filelist $(db_dir)/ && rm -f filelist version

##############
#   PACKAGE  #
all pkg package:: $(pkg)

$(pkg):
	@-echo "# building: $@" >&2
	@$(MAKE) clean
	@$(MAKE) build 2>&1 1>build.log | tee build.err 
	@-echo "# writing metadata: $@" >&2
	@cd "$d" && for f in `find -H -type f`; do \
		setfattr -n $(attr).name -v $(name) "$${f}" 2>/dev/null; \
		setfattr -n $(attr).version -v $(version).$(build) "$${f}" 2>/dev/null; \
		$(call seteasum_f,$${f}); \
	done
# bsdtar rocks, it saves all ea info, and restores it with -p
	@cd "$d" && bsdtar -cpzf "$(CURDIR)/$@.part" ./*
	@-rm -rf "$d" "$W"
	@$(call seteasum_f,$@.part)
	@mv "$@.part" "$@"
	@-echo "# created: $@" >&2

ls list: $(pkg)
	@bsdtar -t -v -f "$<" | grep -v '/$$' | sed -e 's#\./##'

##############
#    BUILD   #
build: prepare $d

##############
#   PREPARE  #
files=$(foreach f, $(source), $(notdir $(f)) )
unpack_prefix=$W/.ugly.prefix.to.make.make.track.unpack.target.
unpack=$(addprefix $(unpack_prefix),$(files))

prepare:: $(unpack)

$(unpack_prefix)%.zip: %.zip $W
	@unzip -d "$W" "$<"
	@touch "$@"

$(unpack_prefix)%: % $W
	@bsdtar -x -p -C "$W" -f "$<" 
	@touch "$@"

##############
#  DOWNLOAD  #
dl download:: $(files)

%.tar.bz2 %.tar.gz %.tar.xz %.zip:
	@wget -c -O "$@.part" "$(filter %$(notdir $@),$(source))"
	@mv "$@.part" "$@"

##############
#    CLEAN   #
clean:
	@-rm -rf *.part "$d" "$W" build.log build.err

cleanold:
	@-rm -rf $(filter-out $(pkg),$(wildcard *.pkg) )

nuke: clean cleanold
	@-rm -rf $(files) $(pkg)

### Variables for use in pkg makefile
# Most tarballs have "name-version" directory packed inside.
# It's nice to type less, most of the time. Do not use it in this file.
w=$W/$(name)-$(version)
