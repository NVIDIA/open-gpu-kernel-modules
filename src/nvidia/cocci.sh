#!/bin/sh
#
# Helper to invoke spatch with optional pre- and post-processing filters and
# optional additional arguments..
#
# Not meant to be used directly but invoked via Kbuild!
#
# (c) 2025,2026 Open Source Security, Inc. All Rights Reserved.

pfunc_filter() {
	# What a hack!
	#
	# coccinelle doesn't evaluate both sides of a #if ...  #else ... #endif
	# block and even worse, it doesn't even implement enough preprocessor
	# logic to actually understand the #if expression and always only
	# handles the #if branch for non-trivial expressions.
	# Work around that by modifying the expression to '#if 0' and use
	# --noif0-passing to get the #else branch. *sigh!*
	case "$1" in
		pre)	sed 's|\(#if\) \(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG\)|\1 0//\2|' -i generated/*.[ch]; ;;
		diff)	sed 's|\(#if\) 0//\(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG\)|\1 \2|'; ;;
		post)	sed 's|\(#if\) 0//\(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG\)|\1 \2|' -i generated/*.[ch]; ;;
	esac
}

rpc_hal_init_filter() {
	# What a hack, take 2!
	#
	# To modify structure initialization to use designated initializers, we
	# post-process a cocci patch that simply removes the init to inject
	# lines that add the correct initialization, based on the fact that the
	# field member is mentioned in a trailing comment. To make that a valid
	# diff, we fix it up using recountdiff.
	case "$1" in
		pre)	;;
		diff)	sed 's|^-\( *\)\([^ ]\+\), *// *\([^ ]\+\)$|&\n+\1.\3 = \2,|' | recountdiff; ;;
		post)	;;
	esac
}

null_filter() {
	case "$1" in
		pre)	;;
		diff)	cat; ;;
		post)	;;
	esac
}

filter() {
	${1:-null}_filter "$2"
}

check_prog() {
	BIN="$1"
	PKG="$2"

	if [ -z "$(command -v $BIN 2>/dev/null)" ]; then
		echo >&2 "error: $BIN not found, please install $PKG!"
		return 1
	fi

	return 0
}

if [ $# -lt 2 ]; then
	echo >&2 "error: spatch file and program missing!"
	exit 1
fi

SCRIPT=$1; shift
SPATCH=$1; shift

if ! check_prog "$SPATCH" coccinelle || \
   ! check_prog recountdiff patchutils;
then
	echo >&2 "error: missing required programs!"
	exit 2
fi

FILTER=$(echo "$SCRIPT" | sed -n 's|.*:||p')
SCRIPT=${SCRIPT%:*}
EXTRA_ARGS=$(sed -n 's|// options: ||p' $SCRIPT)

filter "$FILTER" pre
$SPATCH --sp-file "$SCRIPT" "$@" $EXTRA_ARGS | filter "$FILTER" diff
filter "$FILTER" post
