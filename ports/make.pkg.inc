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

$W $d $(db_dir):
	@install -m 700 -d "$@"

$(root):
	@install -m 755 -d "$@"

##############
#   INSTALL  #
install: $(pkg) $(db_dir) $(root)
	@test -f $(db_dir)/filelist && { \
		echo "# looks like $(name) is already installed:" >&2; \
		test "x`cat $(db_dir)/version`" != "x$(version).$(build)" && { \
			echo "# an upgrade is avaliable: `cat $(db_dir)/version` -> $(version).$(build)" >&2; \
			echo "# run $(MAKE) upgrade " >&2; \
		} \
		exit 1; \
	}
	@-echo "# installing: $<" >&2
	@$(MAKE) ls > filelist.part
	@echo "$(version).$(build)" > version.part
	$(call seteasum_f,filelist)
	$(call seteasum_f,version)
	@bsdtar -x -p -C "$(root)" -f "$@"
	@mv filelist.part $(db_dir)/filelist
	@mv version.part $(db_dir)/version

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
	@bsdtar -tpf "$<" | grep -v '/$$' | sed -e 's#\./##'

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
OLD_PKG_VERSIONS=$(filter-out $(pkg),$(wildcard *.pkg) )
clean:
	@-rm -rf *.part "$d" "$W" build.log* 

cleanold:
	@-rm -rf $(OLD_PKG_VERSIONS)

nuke: clean cleanold
	@-rm -rf $(files) $(pkg)

### Variables for use in pkg makefile
# Most tarballs have "name-version" directory packed inside.
# It's nice to type less, most of the time. Do not use it in this file.
w=$W/$(name)-$(version)
