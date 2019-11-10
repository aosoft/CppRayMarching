#include "pch.h"
#include "kernel.h"
#include <atomic>
#include <thread>
#include <future>

void RunKernelSingle(std::int32_t width, std::int32_t height, void* p, std::int32_t stride, float time, FnKernel fnKernel)
{
	auto resolution = glm::vec2(static_cast<float>(width), static_cast<float>(height));

	//#pragma omp parallel for
	for (std::int32_t y = 0; y < height; y++)
	{
		auto p2 = reinterpret_cast<glm::vec4*>(static_cast<std::uint8_t*>(p) + y * stride);
		for (std::int32_t x = 0; x < width; x++)
		{
			fnKernel(*p2, glm::vec4(static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f), time, resolution);
			p2++;
		}
	}
}

static const auto hc = std::thread::hardware_concurrency();

void RunKernelParallel(std::int32_t width, std::int32_t height, void* p, std::int32_t stride, float time, FnKernel fnKernel)
{
	std::array<std::future<void>, 64> futures;
	auto parallelCount = std::min<size_t>(futures.size(), hc);

	std::atomic_int32_t y = 0;
	for (size_t i = 0; i < parallelCount; i++)
	{
		futures[i] = std::async(std::launch::async, [&y, width, height, p, stride, time, fnKernel]()
			{
				auto resolution = glm::vec2(static_cast<float>(width), static_cast<float>(height));
				while (true)
				{
					auto yy = y++;
					if (yy >= height)
					{
						break;
					}
					auto p2 = reinterpret_cast<glm::vec4*>(static_cast<std::uint8_t*>(p) + yy * stride);
					for (std::int32_t x = 0; x < width; x++)
					{
						fnKernel(*p2, glm::vec4(static_cast<float>(x), static_cast<float>(yy), 0.0f, 0.0f), time, resolution);
						p2++;
					}
				}
			});
	}

	for (size_t i = 0; i < parallelCount; i++)
	{
		futures[i].wait();
	}
}

void RunKernelParallelOpenCV(std::int32_t width, std::int32_t height, void* p, std::int32_t stride, float time, FnKernel fnKernel)
{
	cv::parallel_for_(cv::Range(0, height), [width, height, p, stride, time, fnKernel](const cv::Range& range)
		{
			auto resolution = glm::vec2(static_cast<float>(width), static_cast<float>(height));
			for (std::int32_t y = range.start; y < range.end; y++)
			{
				auto p2 = reinterpret_cast<glm::vec4*>(static_cast<std::uint8_t*>(p) + y * stride);
				for (std::int32_t x = 0; x < width; x++)
				{
					fnKernel(*p2, glm::vec4(static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f), time, resolution);
					p2++;
				}
			}
		});
}