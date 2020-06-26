#!/bin/bash

BASE_DIR=$(dirname "$(readlink -f "$0")")
export LD_LIBRARY_PATH="$BASE_DIR":"$LD_LIBRARY_PATH"
export QTDIR="$BASE_DIR"
export TRANSLATION_PATH=../translation
export VIDEO_TRANSLATION_PATH=../translation/video
export QT_PLUGIN_PATH="$BASE_DIR"/plugins:"$QT_PLUGIN_PATH"
export QT_QPA_PLATFORM_PLUGIN_PATH="$BASE_DIR"/plugins/platforms:"$QT_QPA_PLATFORM_PLUGIN_PATH"
export QTWEBENGINEPROCESS_PATH="$BASE_DIR"/QtWebEngineProcess

$BASE_DIR/Tchoupi &
