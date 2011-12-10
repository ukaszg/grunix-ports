PHONY=

.DEFAULT_GOAL=all
SHELL=/bin/mksh
BUILD_ROOT?=$(CURDIR)
LOCAL_INSTALL_TO?="${HOME}/.local"
INSTALL_TO?=$(shell test `id -u` -eq 0 && echo "/" || echo $(LOCAL_INSTALL_TO))

##############
#    HELP    #
PHONY+=help
help:

mkdir.%:
	@install -m 700 -d "$@"

##############
#  DOWNLOAD  #
part=.part
filenames=$(foreach f, $(source), $(notdir $(f)) )
files=$(addprefix $(BUILD_ROOT)/, $(filenames) )

dl download:: $(files)

%.tar.bz2 %.tar.gz %.tar.xz %.zip:
	@wget -c -O "$@$(part)" "$(filter %$(notdir $@),$(source))"
	@mv "$@$(part)" "$@"

##############
#   PREPARE  #
# build directory
W=$(BUILD_ROOT)/work
unpack=$(addprefix $W/.unpack.,$(filenames))

prepare:: $(unpack)

$W/.unpack.%.tar.bz2: $(BUILD_ROOT)/%.tar.bz2 mkdir.$W
	@bzip2 -d -c "$<" | tar -x -C "$W" -f -
	@touch "$@"

$W/.unpack.%.tar.gz: $(BUILD_ROOT)/%.tar.gz mkdir.$W
	@gzip  -d -c "$<" | tar -x -C "$W" -f -
	@touch "$@"

$W/.unpack.%.tar.xz: $(BUILD_ROOT)/%.tar.xz mkdir.$W
	@xz    -d -c "$<" | tar -x -C "$W" -f -
	@touch "$@"

$W/.unpack.%.zip: $(BUILD_ROOT)/%.zip mkdir.$W
	@unzip -d "$W" "$<"
	@touch "$@"

##############
#    BUILD   #
# Most tarballs have "name-version" directory packed inside.
# It's nice to type less, most of the time. Do not use it in this file.
w=$W/$(name)-$(version)
d=$(BUILD_ROOT)/dest

build: prepare mkdir.$d

##############
#   PACKAGE  #
pkgext=pkg
packagename=$(name)-$(version).$(build).$(pkgext)
pkg_path=$(BUILD_ROOT)/$(packagename)

all pkg package:: $(pkg_path)

$(pkg_path):
	@-echo "# building: $(packagename)"
	@$(MAKE) build
	@cd "$d" && tar -cpf - ./* | gzip > "$(pkg_path)$(part)"
	@-rm -rf "$d" "$W"
	@mv "$(pkg_path)$(part)" "$(pkg_path)"
	@-echo "# created: $(packagename)"

PHONY+=ls list
ls list: $(pkg_path)
	@gzip -d -c "$(pkg_path)" | tar tf - | grep -e '.*[^/]$$'

##############
#   INSTALL  #
inst_idx=$(BUILD_ROOT)/.installed
idir=$(BUILD_ROOT)/install_tmp_dir
attr=user.package
checksum=sha256sum

inst install:: $(inst_idx)

$(inst_idx): $(inst_idx)$(part)
	@for i in `cat "$<" | sed -e "s#$(idir)#$(INSTALL_TO)#"`;do \
		if test -f "$$i"; then \
			echo "# sorry, can't install: $(packagename) " >&2; \
			echo "\t doing so would overwrite existing file [$$i]" >&2; \
			exit 1; \
		fi; \
	done
	@for i in `cat "$<"`;do \
		setfattr -n $(attr).$(checksum) -v `$(checksum) "$$i"|cut -d' ' -f1` "$$i"; \
		setfattr -n $(attr).name -v $(name) "$$i"; \
		setfattr -n $(attr).version -v $(version) "$$i"; \
	done
	@setfattr -n $(attr).version -v $(version) "$<"
	@-mkdir -p "$(INSTALL_TO)"
	@mv "$(idir)/"* "$(INSTALL_TO)/"
	@sed -i -e "s#$(idir)##" "$<"
	@-rm -rf "$(idir)"
	@mv "$<" "$@"
	@-echo "# installed: $(packagename)\n"

$(inst_idx)$(part): mkdir.$(idir) $(pkg_path)
	@gzip -d -c "$(pkg_path)" | tar -x -p -C "$(idir)" -f -
	@find -H "$(idir)" -type f > "$@$(part)"
	@mv "$@$(part)" "$@"

##############
#   REMOVE   #
u_force?=0
# uforce!=0 changes all errors into warnings. uninstall will error on any
# inconsistency (except /etc), it is meant to be naggy.
PHONY+=remove uninstall
remove uninstall: 
	@test -f $(inst_idx) || { \
		echo "# package $(packagename) is not installed" >&2; \
		exit 1; \
	}
	@for i in `cat $(inst_idx)`; do \
		test "$(name)"="`getfattr -n $(attr).name \"$$i\"`" && { \
			echo "# file [$$i] is not marked as belonging to the $(packagename)"; \
			ifeq (0, $(u_force)) exit 1; endif \
		}; \
	done


##############
#    CLEAN   #
PHONY+=clean nuke
clean:
	@-rm -rf *.tar.gz$(part) *.tar.bz2$(part) *.tar.xz$(part) *.zip$(part)
	@-rm -rf "$d" "$d$(part)" "$W" "$W$(part)" "$(idir)" "$(idir)$(part)" \
		"$(inst_idx)$(part)"

nuke: clean
	@-rm -rf $(files) $(pkg_path)

.PHONY: $(PHONY)
