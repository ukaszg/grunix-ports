.PHONY: clean nuke help ls list clean_old
.DEFAULT_GOAL=all
SHELL=/bin/ash

W=$(CURDIR)/work
d=$(CURDIR)/out
part=.part
# Most tarballs have "name-version" directory packed inside.
# It's nice to type less, most of the time. Do not use it in this file.
w=$W/$(name)-$(version)

.PHONY: clean nuke ls list
$W $d:
	@install -m 700 -d "$@"

##############
#   PACKAGE  #
pkg=$(name)-$(version).$(build).pkg

all pkg package:: $(pkg)

$(pkg):
	@-mv build.log build.log.1
	$(MAKE) clean build 1>build.log
	cd "$d" && tar -c -p -f - ./* | gzip > "$(CURDIR)/$(pkg).part"
	@-rm -rf "$d" "$W"
	@mv "$(pkg).part" "$(pkg)"
	@-echo "# created: $(pkg)"

ls list: $(pkg)
	@gzip -d -c "$(pkg)" | tar tf -

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
