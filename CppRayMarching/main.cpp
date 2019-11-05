#include "pch.h"

#ifdef _DEBUG
#pragma comment(lib, "opencv_world411d.lib")
#else
#pragma comment(lib, "opencv_world411.lib")
#endif

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(void)
{
	cv::Mat img;

	cv::namedWindow("Window", cv::WINDOW_AUTOSIZE | cv::WINDOW_FREERATIO);
	//cv::imshow("Window", img);
	cv::waitKey(0);
}