# macros that help with ea-related stuff
attr?=user.package
checksum?=sha256sum
print_val=sed -n -e 's/.*="\(.*\)"/\1/p' -

# call'ables
# _[fnv] in names are $(1) $(2)... etc variables to $(call )
# _f file path
# _n pkg name
# _v pkg version
#
counthash_f=$(checksum) $(1)|cut -d' ' -f1

geteasum_f=getfattr -n $(attr).$(checksum) "$(1)" 2>/dev/null|$(print_val)
seteasum_f=setfattr -n $(attr).$(checksum) -v `$(counthash_f)` "$(1)" 2>/dev/null

geteanam_f=getfattr -n $(attr).name "$(1)" 2>/dev/null| $(print_val)
seteanam_f_n=setfattr -n $(attr).name -v $(2) "$(1)" 2>/dev/null

geteaver_f=getfattr -n $(attr).version "$(1)" 2>/dev/null|$(print_val)
seteaver_f_v=setfattr -n $(attr).version -v $(2) "$(1)" 2>/dev/null
