#pragma once

#include "kernel.h"

//	PostProcess を有効にする (RGBA to BGRA)
#define ENABLE_POSTPROCESS

//	Linear → sRGB 変換をする
//#define ENABLE_POSTPROCESS_LINEAR_TO_SRGB

//	レンダリング関数に渡す時間値をタイマーで計測した現在値にする
//	未定義時は時間を無視して 1 フレームずつ進める
#define REALTIME_MODE
