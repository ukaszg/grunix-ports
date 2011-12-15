.PHONY: clean nuke ls list
.DEFAULT_GOAL=all
.SUFFIXES:
MAKEFLAGS=-j 1
SHELL=/bin/ash

W=$(CURDIR)/work
d=$(CURDIR)/out
part=.part

$W $d:
	@install -m 700 -d "$@"

##############
#   PACKAGE  #
pkg=$(name)-$(version).$(build).pkg

all pkg package:: $(pkg)

$(pkg):
	$(MAKE) build
	@cd "$d" && tar -c -p -f - ./* | gzip > "$(CURDIR)/$(pkg)$(part)"
	@-rm -rf "$d" "$W"
	@mv "$(pkg)$(part)" "$(pkg)"
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

$(unpack_prefix)%.tar.bz2: %.tar.bz2 $W
	@bzip2 -d -c "$<" | tar -x -C "$W" -f -
	@touch "$@"

$(unpack_prefix)%.tar.gz: %.tar.gz $W
	@gzip  -d -c "$<" | tar -x -C "$W" -f -
	@touch "$@"

$(unpack_prefix)%.tar.xz: %.tar.xz $W
	@xz    -d -c "$<" | tar -x -C "$W" -f -
	@touch "$@"

$(unpack_prefix)%.zip: %.zip $W
	@unzip -d "$W" "$<"
	@touch "$@"

##############
#  DOWNLOAD  #
dl download:: $(files)

%.tar.bz2 %.tar.gz %.tar.xz %.zip:
	@wget -c -O "$@$(part)" "$(filter %$(notdir $@),$(source))"
	@mv "$@$(part)" "$@"

##############
#    CLEAN   #
clean:
	@-rm -rf *.tar.gz$(part) *.tar.bz2$(part) *.tar.xz$(part) *.zip$(part) "$d" "$W"

nuke: clean
	@-rm -rf $(files) $(pkg)

##############
# BUILD VARS #
w=$W/$(name)-$(version) 
