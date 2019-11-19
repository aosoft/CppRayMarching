#include "pch.h"


#ifdef _DEBUG
#pragma comment(lib, "opencv_world411d.lib")
#else
#pragma comment(lib, "opencv_world411.lib")
#endif

#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "defines.h"
#include "kernel.h"
#include "RayMarching.h"

const int ImageWidth = 640;
const int ImageHeight = 480;

void TestKernel(glm::vec4& gl_FragColor, const glm::vec4& gl_FragCoord, float time, const glm::vec2 resolution)
{
	gl_FragColor = glm::vec4(gl_FragCoord.xy() / resolution, sin(3.14 * time), 1.0f);
}

class Timer
{
private:
	std::chrono::time_point<std::chrono::system_clock> _start;

public:
	Timer() : _start(std::chrono::system_clock::now())
	{
	}

	float Current() const
	{
		return std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1>>>(std::chrono::system_clock::now() - _start).count();
	}
};

class FrameCounter
{
private:
	const std::int32_t fps = 60;
	std::int32_t _counter;

public:
	FrameCounter() : _counter(0)
	{
	}

	float Current()
	{
		auto current = ++_counter;
		return static_cast<float>(current / fps) + static_cast<float>(current % fps) / fps;
	}
};

inline float LinearToSRGB(float value)
{
	if (value < 0.0f)
	{
		return 0.0f;
	}	
	return value < 0.0031308f ? 12.92f * value : 1.055f * pow(value, 1.0f / 2.4f) - 0.055f;
}

int main(int argc, const char** argv)
{
	FnKernel fnKernel = Sphere;
	if (argc > 1)
	{
		switch (atoi(argv[1]))
		{
		case 0: fnKernel = Sphere; break;
		case 1: fnKernel = KaleidoscopeTunnel; break;
		case 2: fnKernel = HologramBoxes; break;
		}
	}

	cv::Mat img(ImageHeight, ImageWidth, CV_32FC4);

	cv::namedWindow("Window", cv::WINDOW_AUTOSIZE | cv::WINDOW_FREERATIO);

#ifdef REALTIME_MODE
	Timer timer;
#else
	FrameCounter timer;
#endif
	while (cv::waitKey(1) < 0)
	{
		Timer sw;
#if _DEBUG
		RunKernelSingle(
#else
		RunKernelParallel(
#endif
			img.cols, img.rows, img.ptr(), img.step, timer.Current(), fnKernel);
		char tmp[256];
		sprintf(tmp, "%f [sec]", sw.Current());

		cv::putText(img, tmp, cv::Point(0, 32), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(1.0, 1.0, 1.0, 1.0));
		cv::imshow("Window", img);
	}
}