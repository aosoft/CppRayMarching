#include "pch.h"

#ifdef _DEBUG
#pragma comment(lib, "opencv_world411d.lib")
#else
#pragma comment(lib, "opencv_world411.lib")
#endif

#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "kernel.h"
#include "RayMarching.h"

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

inline float LinearToSRGB(float value)
{
	if (value < 0.0f)
	{
		return 0.0f;
	}
	return value < 0.0031308f ? 12.92f * value : 1.055f * pow(value, 1.0f / 2.4f) - 0.055f;
}

int main(void)
{
	cv::Mat imgRGB(768, 1024, CV_32FC4);
	cv::Mat imgBGR(imgRGB.rows, imgRGB.cols, imgRGB.type());

	cv::namedWindow("Window", cv::WINDOW_AUTOSIZE | cv::WINDOW_FREERATIO);

	Timer timer;
	while (cv::waitKey(1) < 0)
	{
		Timer sw;
#if _DEBUG
		RunKernelSingle(
#else
		RunKernelParallel(
#endif
			imgRGB.cols, imgRGB.rows, imgRGB.ptr(), imgRGB.step, timer.Current(), Sphere);
		char tmp[256];
		sprintf(tmp, "%f [sec]", sw.Current());

		//cv::cvtColor(imgRGB, imgBGR, cv::COLOR_RGBA2BGRA);
		cv::parallel_for_(cv::Range(0, imgRGB.rows), [&imgRGB, &imgBGR](const cv::Range& range)
		{
				for (std::int32_t y = range.start; y < range.end; y++)
				{
					auto src = reinterpret_cast<const glm::vec4*>(static_cast<std::uint8_t*>(imgRGB.ptr()) + y * imgRGB.step);
					auto dst = reinterpret_cast<glm::vec4*>(static_cast<std::uint8_t*>(imgBGR.ptr()) + y * imgBGR.step);
					for (std::int32_t x = 0; x < imgRGB.cols; x++)
					{
						//*dst = src->zyxw();
						dst->x = LinearToSRGB(src->z);
						dst->y = LinearToSRGB(src->y);
						dst->z = LinearToSRGB(src->x);
						dst->w = src->w;
						src++;
						dst++;
					}
				}
			});

		cv::putText(imgBGR, tmp, cv::Point(0, 32), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(1.0, 1.0, 1.0, 1.0));
		cv::imshow("Window", imgBGR);
	}
}