#pragma once

#define DEFINED(v) (v == 1)

#define USE_HIGH_RES 0
#define ADD_RAW_STREAM 1
#define SHOW_PREVIEW 1
#define CAMERA_LOG 0
#define PREVIEW_LOG 0

#ifndef NDEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

#if DEFINED(USE_HIGH_RES)
    #define IMAGE_WIDTH 1280
    #define IMAGE_HEIGHT 960
#else
    #define IMAGE_WIDTH 640
    #define IMAGE_HEIGHT 480
#endif

#define WINDOW_WIDTH IMAGE_WIDTH
#define WINDOW_HEIGHT IMAGE_HEIGHT
#define WINDOW_X 0
#define WINDOW_Y 0
#define WINDOW_BORDER 1

#define IMAGE_BUFFER_COUNT 3

#define FRAMERATE 60
