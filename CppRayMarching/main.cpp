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

void kernel(glm::vec4& gl_FragColor, const glm::vec4& gl_FragCoord, float time, const glm::vec2 resolution)
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

int main(void)
{
	cv::Mat imgRGB(768, 1024, CV_32FC4);
	cv::Mat imgBGR(imgRGB);


	cv::namedWindow("Window", cv::WINDOW_AUTOSIZE | cv::WINDOW_FREERATIO);

	Timer timer;
	while (cv::waitKey(1) < 0)
	{
		Timer sw;
		RunKernelParallel(
			imgRGB.cols, imgRGB.rows, imgRGB.ptr(), imgRGB.step, timer.Current(), Sphere);
		char tmp[256];
		sprintf(tmp, "%f [sec]", sw.Current());

		cv::cvtColor(imgRGB, imgBGR, cv::COLOR_RGBA2BGRA);
		cv::putText(imgBGR, tmp, cv::Point(0, 32), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(1.0, 1.0, 1.0, 1.0));
		cv::imshow("Window", imgBGR);
	}
}