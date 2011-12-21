.PHONY: clean nuke help ls list clean_old
.DEFAULT_GOAL=all
SHELL=/bin/sh

W=$(CURDIR)/work
d=$(CURDIR)/out

.PHONY: clean nuke ls list
.SUFFIXES:
$W $d:
	@install -m 700 -d "$@"

##############
#   PACKAGE  #
pkg=$(name)-$(version).$(build).pkg

all pkg package:: $(pkg)

# since this file will be included from pkg dir
include ../make.ea.inc

$(pkg):
	@-echo "# building: $@"
	@$(MAKE) clean build 1> build.log
	@-echo "# writing metadata: $@"
	@cd "$d" && for f in `find -H -type f`; do \
		$(call seteanam_f_n,$${f},$(name)); \
		$(call seteaver_f_v,$${f},$(version)); \
		$(call seteasum_f,$${f}); \
	done
# bsdtar rocks, it saves all ea info, and restores it with -p
	@cd "$d" && bsdtar -cpzf "$(CURDIR)/$@.part" ./*
	@-rm -rf "$d" "$W"
	@$(call seteasum_f,$@.part)
	@mv "$@.part" "$@"
	@-echo "# created: $@"

ls list: $(pkg)
	@bsdtar -tpf "$<"

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
	@-rm -rf *.tar.gz.part *.tar.bz2.part *.tar.xz.part *.zip.part "$d" "$W" build.log*

cleanold:
	@-rm -rf $(OLD_PKG_VERSIONS)

nuke: clean cleanold
	@-rm -rf $(files) $(pkg)

### Variables for use in pkg makefile
# Most tarballs have "name-version" directory packed inside.
# It's nice to type less, most of the time. Do not use it in this file.
w=$W/$(name)-$(version)
