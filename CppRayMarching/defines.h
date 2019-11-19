#pragma once

#include "kernel.h"

//	PostProcess を有効にする (Linear RGBA to sRGB BGRA)
#define ENABLE_POSTPROCESS

//	レンダリング関数に渡す時間値をタイマーで計測した現在値にする
//	未定義時は時間を無視して 1 フレームずつ進める
#define REALTIME_MODE
