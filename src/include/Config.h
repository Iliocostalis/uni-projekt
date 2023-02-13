#pragma once

#define DEFINED(v) (v == 1)

#define ADD_RAW_STREAM 1
#define CAMERA_LOG 0
#define SAVE_VIDEO_FRAME_DELAY_MS 500

#ifndef NDEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

#define IMAGE_BUFFER_COUNT 6

#define FRAMERATE 60
