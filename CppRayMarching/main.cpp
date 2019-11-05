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
	cv::Mat img(768, 1024, CV_32FC4);

	for (int y = 0; y < img.rows; y++)
	{
		for (int x = 0; x < img.cols; x++)
		{
			*img.ptr <glm::vec4>(y, x) = glm::vec4((float)x / (float)img.cols, (float)y / (float)img.rows, 0.0f, 1.0f);
		}
	}

	cv::namedWindow("Window", cv::WINDOW_AUTOSIZE | cv::WINDOW_FREERATIO);
	cv::imshow("Window", img);
	cv::waitKey(0);
}