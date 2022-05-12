#!/bin/bash

autoreconf -fiv
[[ $? -eq 0 ]] || exit $?
rm -Rf autom4te.cache
