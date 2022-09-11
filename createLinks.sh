#!/bin/sh
dir=$(pwd)
ln -s "$dir/libcamera/src/libcamera/libcamera.so.0.0.0" "$dir/libcamera/src/libcamera/libcamera.so.0"
ln -s "$dir/libcamera/src/libcamera/libcamera.so.0" "$dir/libcamera/src/libcamera/libcamera.so"

ln -s "$dir/libcamera/src/libcamera/base/libcamera-base.so.0.0.0" "$dir/libcamera/src/libcamera/base/libcamera-base.so.0"
ln -s "$dir/libcamera/src/libcamera/base/libcamera-base.so.0" "$dir/libcamera/src/libcamera/base/libcamera-base.so"
