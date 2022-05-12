#!/bin/bash
# SPDX-License-Identifier: GPL-2.0
# (c) 2014, Sasha Levin <sasha.levin@oracle.com>
#set -x

if [[ $# < 1 ]]; then
	echo "Usage:"
	echo "	$0 -r <release> | <vmlinux> [base path] [modules path]"
	exit 1
fi

if [[ $1 == "-r" ]] ; then
	vmlinux=""
	basepath="auto"
	modpath=""
	release=$2

	for fn in {,/usr/lib/debug}/boot/vmlinux-$release{,.debug} /lib/modules/$release{,/build}/vmlinux ; do
		if [ -e "$fn" ] ; then
			vmlinux=$fn
			break
		fi
	done

	if [[ $vmlinux == "" ]] ; then
		echo "ERROR! vmlinux image for release $release is not found" >&2
		exit 2
	fi
else
	vmlinux=$1
	basepath=${2-auto}
	modpath=$3
	release=""
fi

declare -A cache
declare -A modcache

find_module() {
	if [[ "$modpath" != "" ]] ; then
		for fn in $(find "$modpath" -name "${module//_/[-_]}.ko*") ; do
			if readelf -WS "$fn" | grep -qwF .debug_line ; then
				echo $fn
				return
			fi
		done
		return 1
	fi

	modpath=$(dirname "$vmlinux")
	find_module && return

	if [[ $release == "" ]] ; then
		release=$(gdb -ex 'print init_uts_ns.name.release' -ex 'quit' -quiet -batch "$vmlinux" | sed -n 's/\$1 = "\(.*\)".*/\1/p')
	fi

	for dn in {/usr/lib/debug,}/lib/modules/$release ; do
		if [ -e "$dn" ] ; then
			modpath="$dn"
			find_module && return
		fi
	done

	modpath=""
	return 1
}

parse_symbol() {
	# The structure of symbol at this point is:
	#   ([name]+[offset]/[total length])
	#
	# For example:
	#   do_basic_setup+0x9c/0xbf

	if [[ $module == "" ]] ; then
		local objfile=$vmlinux
	elif [[ "${modcache[$module]+isset}" == "isset" ]]; then
		local objfile=${modcache[$module]}
	else
		local objfile=$(find_module)
		if [[ $objfile == "" ]] ; then
			echo "WARNING! Modules path isn't set, but is needed to parse this symbol" >&2
			return
		fi
		modcache[$module]=$objfile
	fi

	# Remove the englobing parenthesis
	symbol=${symbol#\(}
	symbol=${symbol%\)}

	# Strip segment
	local segment
	if [[ $symbol == *:* ]] ; then
		segment=${symbol%%:*}:
		symbol=${symbol#*:}
	fi

	# Strip the symbol name so that we could look it up
	local name=${symbol%+*}

	# Use 'nm vmlinux' to figure out the base address of said symbol.
	# It's actually faster to call it every time than to load it
	# all into bash.
	if [[ "${cache[$module,$name]+isset}" == "isset" ]]; then
		local base_addr=${cache[$module,$name]}
	else
		local base_addr=$(nm "$objfile" | awk '$3 == "'$name'" && ($2 == "t" || $2 == "T") {print $1; exit}')
		if [[ $base_addr == "" ]] ; then
			# address not found
			return
		fi
		cache[$module,$name]="$base_addr"
	fi
	# Let's start doing the math to get the exact address into the
	# symbol. First, strip out the symbol total length.
	local expr=${symbol%/*}

	# Now, replace the symbol name with the base address we found
	# before.
	expr=${expr/$name/0x$base_addr}

	# Evaluate it to find the actual address
	expr=$((expr))
	local address=$(printf "%x\n" "$expr")

	# Pass it to addr2line to get filename and line number
	# Could get more than one result
	if [[ "${cache[$module,$address]+isset}" == "isset" ]]; then
		local code=${cache[$module,$address]}
	else
		local code=$(${CROSS_COMPILE}addr2line -i -e "$objfile" "$address")
		cache[$module,$address]=$code
	fi

	# addr2line doesn't return a proper error code if it fails, so
	# we detect it using the value it prints so that we could preserve
	# the offset/size into the function and bail out
	if [[ $code == "??:0" ]]; then
		return
	fi

	# Strip out the base of the path on each line
	code=$(while read -r line; do echo "${line#$basepath/}"; done <<< "$code")

	# In the case of inlines, move everything to same line
	code=${code//$'\n'/' '}

	# Replace old address with pretty line numbers
	symbol="$segment$name ($code)"
}

decode_code() {
	local scripts=`dirname "${BASH_SOURCE[0]}"`

	echo "$1" | $scripts/decodecode
}

handle_line() {
	local words

	# Tokenize
	read -a words <<<"$1"

	# Remove hex numbers. Do it ourselves until it happens in the
	# kernel

	# We need to know the index of the last element before we
	# remove elements because arrays are sparse
	local last=$(( ${#words[@]} - 1 ))

	for i in "${!words[@]}"; do
		# Remove the address
		if [[ ${words[$i]} =~ \[\<([^]]+)\>\] ]]; then
			unset words[$i]
		fi

		# Format timestamps with tabs
		if [[ ${words[$i]} == \[ && ${words[$i+1]} == *\] ]]; then
			unset words[$i]
			words[$i+1]=$(printf "[%13s\n" "${words[$i+1]}")
		fi
	done

	if [[ ${words[$last]} =~ \[([^]]+)\] ]]; then
		module=${words[$last]}
		module=${module#\[}
		module=${module%\]}
		symbol=${words[$last-1]}
		unset words[$last-1]
	else
		# The symbol is the last element, process it
		symbol=${words[$last]}
		module=
	fi

	unset words[$last]
	parse_symbol # modifies $symbol

	# Add up the line number to the symbol
	echo "${words[@]}" "$symbol $module"
}

if [[ $basepath == "auto" ]] ; then
	module=""
	symbol="kernel_init+0x0/0x0"
	parse_symbol
	basepath=${symbol#kernel_init (}
	basepath=${basepath%/init/main.c:*)}
fi

while read line; do
	# Let's see if we have an address in the line
	if [[ $line =~ \[\<([^]]+)\>\] ]] ||
	   [[ $line =~ [^+\ ]+\+0x[0-9a-f]+/0x[0-9a-f]+ ]]; then
		# Translate address to line numbers
		handle_line "$line"
	# Is it a code line?
	elif [[ $line == *Code:* ]]; then
		decode_code "$line"
	else
		# Nothing special in this line, show it as is
		echo "$line"
	fi
done
