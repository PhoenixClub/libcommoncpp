#!/bin/sh
# Wrapper script to invoke configure with adjusted options.

./configure "$@" --disable-cpp --disable-shared --with-pic --prefix=$PWD/noinstall
