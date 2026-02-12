#!/bin/sh
#
# Helper to invoke spatch with optional pre- and post-processing filters and
# optional additional arguments..
#
# Not meant to be used directly but invoked via Kbuild!
#
# (c) 2025,2026 Open Source Security, Inc. All Rights Reserved.

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

if ! check_prog "$SPATCH" coccinelle; then
	echo >&2 "error: missing required programs!"
	exit 2
fi

FILTER=$(echo "$SCRIPT" | sed -n 's|.*:||p')
SCRIPT=${SCRIPT%:*}
EXTRA_ARGS=$(sed -n 's|// options: ||p' $SCRIPT)

filter "$FILTER" pre
$SPATCH --sp-file "$SCRIPT" "$@" $EXTRA_ARGS | filter "$FILTER" diff
filter "$FILTER" post
