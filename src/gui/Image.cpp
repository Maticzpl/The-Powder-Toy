#include "Image.h"

#include "bzip2/bz2wrap.h"
#include "graphics/Pix.h"
#include "common/tpt-compat.h"
#include "SDLAssert.h"

#include <stdexcept>
#include <cmath>

namespace gui
{
	Image Image::FromPTI(const char *data, size_t size)
	{
		auto *u8 = reinterpret_cast<const uint8_t *>(data);
		if (size < 16)
		{
			throw std::runtime_error("image empty");
		}
		if (!(u8[0] == 'P' && u8[1] == 'T' && u8[2] == 'i' && u8[3] == '\1'))
		{
			throw std::runtime_error("image header invalid");
		}
		auto width  = int(uint32_t(u8[4]) | (uint32_t(u8[5]) << 8));
		auto height = int(uint32_t(u8[6]) | (uint32_t(u8[7]) << 8));
		auto pixels = width * height;
		Image image;
		image.size = { width, height };
		image.pixels.resize(pixels);
		std::vector<char> decompressed;
		if (BZ2WDecompress(decompressed, &data[8], size - 8, width * height * 3) != BZ2WDecompressOk)
		{
			throw std::runtime_error("failed to decompress image");
		}
		for (auto i = 0; i < pixels; ++i)
		{
			image.pixels[i] = PixRGBA(decompressed[i], decompressed[i + pixels * 1], decompressed[i + pixels * 2], 255);
		}
		return image;
	}

	Image Image::FromBMP(const char *data, size_t size)
	{
		SDL_Surface *raw = SDLASSERTPTR(SDL_LoadBMP_RW(SDL_RWFromConstMem(data, size), 1));
		SDL_PixelFormat *format = SDLASSERTPTR(SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888));
		SDL_Surface *surface = SDLASSERTPTR(SDL_ConvertSurface(raw, format, 0));
		SDL_FreeSurface(raw);
		SDL_FreeFormat(format);
		Image image;
		image.size = { surface->w, surface->h };
		image.pixels.resize(image.size.x * image.size.y);
		SDLASSERTZERO(SDL_LockSurface(surface));
		for (auto y = 0; y < image.size.y; ++y)
		{
			for (auto x = 0; x < image.size.x; ++x)
			{
				image.pixels[y * image.size.x + x] = *reinterpret_cast<uint32_t *>(reinterpret_cast<char *>(surface->pixels) + y * surface->pitch + x * sizeof(uint32_t));
			}
		}
		SDL_UnlockSurface(surface);
		SDL_FreeSurface(surface);
		return image;
	}

	Image Image::FromSize(Point size)
	{
		Image image;
		image.size = size;
		image.pixels = std::vector<uint32_t>(size.x * size.y);
		return image;
	}

	Image Image::FromColor(uint32_t color, Point size)
	{
		Image image;
		image.size = size;
		image.pixels = std::vector<uint32_t>(size.x * size.y, color);
		return image;
	}

	void Image::Size(Point newSize, ResizeMode resizeMode)
	{
		if (newSize.x < 0) newSize.x = 0;
		if (newSize.y < 0) newSize.y = 0;
		if (newSize == size)
		{
			return;
		}
		auto newPixels = std::vector<uint32_t>(newSize.x * newSize.y);
		switch (resizeMode)
		{
		case resizeModeLinear:
			for (auto y = 0; y < newSize.y; ++y)
			{
				for (auto x = 0; x < newSize.x; ++x)
				{
					auto tl = Point{  x      * size.x / newSize.x,  y      * size.y / newSize.y };
					auto br = Point{ (x + 1) * size.x / newSize.x, (y + 1) * size.y / newSize.y };
					auto sz = br - tl;
					if (sz.x < 1) sz.x = 1;
					if (sz.y < 1) sz.y = 1;
					uint32_t r = 0;
					uint32_t g = 0;
					uint32_t b = 0;
					uint32_t a = 0;
					for (auto yy = 0; yy < sz.y; ++yy)
					{
						for (auto xx = 0; xx < sz.x; ++xx)
						{
							auto pix = pixels[(tl.y + yy) * size.x + (tl.x + xx)];
							r += PixR(pix);
							g += PixG(pix);
							b += PixB(pix);
							a += PixA(pix);
						}
					}
					uint32_t chunkSize = sz.x * sz.y;
					newPixels[y * newSize.x + x] = PixRGBA(r / chunkSize, g / chunkSize, b / chunkSize, a / chunkSize);
				}
			}
			break;

		case resizeModeLanczos:
			{
				constexpr auto kernelSize = 2;
				struct KernelEntry
				{
					int lo, hi;
					std::array<float, kernelSize * 2 + 1> weights;
				};
				auto sampleKernel = [](float x) -> float {
					if (x == 0)
					{
						return 1;
					}
					if (x > kernelSize || x < -kernelSize)
					{
						return 0;
					}
					return kernelSize * sinf(M_PI * x) * sinf(M_PI * x / kernelSize) / M_PI / M_PI / x / x;
				};
				auto generateKernel = [&sampleKernel](int outputSize, int inputSize) -> std::vector<KernelEntry> {
					std::vector<KernelEntry> kernel(outputSize);
					for (auto i = 0; i < outputSize; ++i)
					{
						auto mapped = float(i) * inputSize / outputSize;
						auto discrete = int(floorf(mapped));
						kernel[i].lo = discrete - kernelSize + 1;
						kernel[i].hi = discrete + kernelSize + 1;
						if (kernel[i].lo <             0) kernel[i].lo =             0;
						if (kernel[i].lo > inputSize - 1) kernel[i].lo = inputSize - 1;
						if (kernel[i].hi <             0) kernel[i].hi =             0;
						if (kernel[i].hi > inputSize - 1) kernel[i].hi = inputSize - 1;
						for (auto ii = kernel[i].lo; ii <= kernel[i].hi; ++ii)
						{
							kernel[i].weights[ii - kernel[i].lo] = sampleKernel(mapped - ii);
						}
					}
					return kernel;
				};
				auto kernelX = generateKernel(newSize.x, size.x);
				auto kernelY = generateKernel(newSize.y, size.y);

				for (auto y = 0; y < newSize.y; ++y)
				{
					for (auto x = 0; x < newSize.x; ++x)
					{
						auto &entryX = kernelX[x];
						auto &entryY = kernelY[y];
						auto resample = [this, &entryX, &entryY](uint8_t (accessor)(uint32_t)) -> float {
							float sum = 0;
							for (auto yy = entryY.lo; yy <= entryY.hi; ++yy)
							{
								for (auto xx = entryX.lo; xx <= entryX.hi; ++xx)
								{
									sum += accessor(pixels[yy * size.x + xx]) * entryX.weights[xx - entryX.lo] * entryY.weights[yy - entryY.lo];
								}
							}
							return sum;
						};
						float r = resample(PixR);
						float g = resample(PixG);
						float b = resample(PixB);
						float a = resample(PixA);
						if (r <   0) r =   0;
						if (r > 255) r = 255;
						if (g <   0) g =   0;
						if (g > 255) g = 255;
						if (b <   0) b =   0;
						if (b > 255) b = 255;
						if (a <   0) a =   0;
						if (a > 255) a = 255;
						newPixels[y * newSize.x + x] = PixRGBA(uint8_t(r), uint8_t(g), uint8_t(b), uint8_t(a));
					}
				}
			}
			break;

		default:
			break;
		}
		size = newSize;
		pixels = std::move(newPixels);
	}
}
