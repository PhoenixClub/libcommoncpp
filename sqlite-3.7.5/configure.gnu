#!/bin/sh
# Wrapper script to invoke configure with adjusted options.

./configure "$@" --disable-shared --with-pic --prefix=$PWD/noinstall CFLAGS=-DSQLITE_ENABLE_COLUMN_METADATA

