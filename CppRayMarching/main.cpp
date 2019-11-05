#include "pch.h"

#ifdef _DEBUG
#pragma comment(lib, "opencv_world411d.lib")
#else
#pragma comment(lib, "opencv_world411.lib")
#endif

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "kernel.h"

void kernel(glm::vec4& gl_FragColor, const glm::vec2 gl_FragCoord, float time, const glm::vec2 resolution)
{
	gl_FragColor = glm::vec4(gl_FragCoord, 0.0f, 1.0f);
}

int main(void)
{
	cv::Mat imgRGB(768, 1024, CV_32FC4);
	cv::Mat imgBGR(imgRGB);

	RunKernel(imgRGB.cols, imgRGB.rows, imgRGB.ptr(), imgRGB.step, kernel);

	cv::cvtColor(imgRGB, imgBGR, cv::COLOR_RGBA2BGRA);

	cv::namedWindow("Window", cv::WINDOW_AUTOSIZE | cv::WINDOW_FREERATIO);
	cv::imshow("Window", imgBGR);
	cv::waitKey(0);
}