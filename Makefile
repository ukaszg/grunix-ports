ifeq (${PORTS},) 
    PORTS:=/home/uki/dev/ports 
    $(warning PORTS variable is unset, using default: "$(PORTS)" )
endif
ifeq ($(MAKE),)
    MAKE:=make
endif

.DEFAULT_GOAL=package
P=${PORTS}/$(name)
files=$(foreach f, $(source), $(notdir $(f)) )
unpack=$(addprefix unpack., $(files) )
work=$(P)/work

.PHONY: help download prepare build package clean install $(unpack)


help:
	@echo todo

download: $(files)

%.tar.bz2 %.tar.gz %tar.xz %.zip:
	wget -c -O "$(P)/$(@).tmp" $(filter %$(@),$(source)) \ 
	&& mv "$(P)/$(@).tmp" "$(P)/$(@)"


prepare: $(unpack)

unpack.%.tar.bz2: %.tar.bz2 "$(work)"
	bzip2 -d -c "$<" | tar -x -C "$(work)" -f -

unpack.%.tar.gz: %.tar.gz "$(work)"
	gzip  -d -c "$<" | tar -x -C "$(work)" -f -

unpack.%.tar.xz: %.tar.xz "$(work)"
	xz    -d -c "$(subst unpack,,$(@) )" | tar -x -C "$(work)" -f -

unpack.%.zip: %.zip "$(work)" 
	@unzip -d "$(work)" "$<"

$(work):
	mkdir -p "$(work)"


build: prepare

package: build

install: package

clean:
	@rm -rf $(work)

