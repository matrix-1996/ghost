#!/bin/sh
. ../../ghost.sh

# Define build setup
SRC=src
OBJ=obj
ARTIFACT_NAME=windowserver.bin
CFLAGS="-std=c++11 -I$SRC -I$SYSROOT/system/include/freetype2"
LDFLAGS="-lfreetype"

# Include application build tasks
. ../applications.sh