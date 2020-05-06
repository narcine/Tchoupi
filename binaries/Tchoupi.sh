#!/bin/bash

BASE_DIR=$(dirname "$(readlink -f "$0")")
export LD_LIBRARY_PATH="$BASE_DIR":"$LD_LIBRARY_PATH"
export QTDIR="$BASE_DIR"
export TRANSLATION_PATH=../translation

$BASE_DIR/Tchoupi &
