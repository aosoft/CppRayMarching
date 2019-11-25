#include "pch.h"

#include "defines.h"
#include "kernel.h"
#include <atomic>
#include <thread>
#include <future>


#ifdef ENABLE_POSTPROCESS

inline float PostProcess(float value)
{
#ifdef ENABLE_POSTPROCESS_LINEAR_TO_SRGB
	if (value < 0.0f)
	{
		return 0.0f;
	}
	return value < 0.0031308f ? 12.92f * value : 1.055f * pow(value, 1.0f / 2.4f) - 0.055f;
#else
	return std::max(0.0f, value);
#endif
}

#endif

void RunKernelSingle(std::int32_t width, std::int32_t height, void* p, std::int32_t stride, float time, FnKernel fnKernel)
{
	auto resolution = glm::vec2(static_cast<float>(width), static_cast<float>(height));

	//#pragma omp parallel for
	for (std::int32_t y = 0; y < height; y++)
	{
		auto p2 = reinterpret_cast<glm::vec4*>(static_cast<std::uint8_t*>(p) + (height - y - 1) * stride);
		for (std::int32_t x = 0; x < width; x++)
		{
#ifdef ENABLE_POSTPROCESS
			glm::vec4 tmp;
			fnKernel(tmp, glm::vec4(static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f), time, resolution);
			*p2 = glm::vec4(PostProcess(tmp.z), PostProcess(tmp.y), PostProcess(tmp.x), tmp.w);
#else
			fnKernel(*p2, glm::vec4(static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f), time, resolution);
#endif
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
					auto p2 = reinterpret_cast<glm::vec4*>(static_cast<std::uint8_t*>(p) + (height - yy - 1) * stride);
					for (std::int32_t x = 0; x < width; x++)
					{
#ifdef ENABLE_POSTPROCESS
						glm::vec4 tmp;
						fnKernel(tmp, glm::vec4(static_cast<float>(x), static_cast<float>(yy), 0.0f, 0.0f), time, resolution);
						*p2 = glm::vec4(PostProcess(tmp.z), PostProcess(tmp.y), PostProcess(tmp.x), tmp.w);
#else
						fnKernel(*p2, glm::vec4(static_cast<float>(x), static_cast<float>(yy), 0.0f, 0.0f), time, resolution);
#endif
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
				auto p2 = reinterpret_cast<glm::vec4*>(static_cast<std::uint8_t*>(p) + (height - y - 1) * stride);
				for (std::int32_t x = 0; x < width; x++)
				{
#ifdef ENABLE_POSTPROCESS
					glm::vec4 tmp;
					fnKernel(tmp, glm::vec4(static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f), time, resolution);
					*p2 = glm::vec4(PostProcess(tmp.z), PostProcess(tmp.y), PostProcess(tmp.x), tmp.w);
#else
					fnKernel(*p2, glm::vec4(static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f), time, resolution);
#endif
					p2++;
				}
			}
		});
}