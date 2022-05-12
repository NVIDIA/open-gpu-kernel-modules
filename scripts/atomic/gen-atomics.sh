#!/bin/sh
# SPDX-License-Identifier: GPL-2.0
#
# Generate atomic headers

ATOMICDIR=$(dirname $0)
ATOMICTBL=${ATOMICDIR}/atomics.tbl
LINUXDIR=${ATOMICDIR}/../..

cat <<EOF |
gen-atomic-instrumented.sh      asm-generic/atomic-instrumented.h
gen-atomic-long.sh              asm-generic/atomic-long.h
gen-atomic-fallback.sh          linux/atomic-arch-fallback.h		arch_
gen-atomic-fallback.sh          linux/atomic-fallback.h
EOF
while read script header args; do
	/bin/sh ${ATOMICDIR}/${script} ${ATOMICTBL} ${args} > ${LINUXDIR}/include/${header}
	HASH="$(sha1sum ${LINUXDIR}/include/${header})"
	HASH="${HASH%% *}"
	printf "// %s\n" "${HASH}" >> ${LINUXDIR}/include/${header}
done
