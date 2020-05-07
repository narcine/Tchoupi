#!/bin/bash

BASE_DIR=$(dirname "$(readlink -f "$0")")
export LD_LIBRARY_PATH="$BASE_DIR":"$LD_LIBRARY_PATH"
export QTDIR="$BASE_DIR"
export TRANSLATION_PATH=../translation
export VIDEO_TRANSLATION_PATH=../translation/video

$BASE_DIR/Tchoupi &
