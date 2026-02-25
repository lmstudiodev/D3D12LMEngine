#pragma once
#include <support/WinInclude.h>
#include <support/ComPointer.h>
#include <algorithm>
#include <vector>
#include <filesystem>

class ImageLoader
{
public:
	struct ImageData  
	{
		std::vector<char> dataByte;
		uint32_t width;
		uint32_t height;
		uint32_t bpp;
		uint32_t cc;

		GUID wicPixelFormat;
		DXGI_FORMAT giPixelFormat;
	};

public:
	static bool LoadImageFromFile(const std::filesystem::path& imagePath, ImageData& data);

private:
	struct GUID_to_DXGI
	{
		GUID wic;
		DXGI_FORMAT gi;
	};

	static const std::vector<GUID_to_DXGI> s_lookupTable;

private:
	ImageLoader() = default;
	ImageLoader(const ImageLoader&) = default;
	ImageLoader& operator=(const ImageLoader&) = default;

};
