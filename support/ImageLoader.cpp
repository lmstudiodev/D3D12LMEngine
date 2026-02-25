#include "ImageLoader.h"
#include <cstdlib>

const std::vector<ImageLoader::GUID_to_DXGI> ImageLoader::s_lookupTable =
{
	{ GUID_WICPixelFormat32bppBGRA , DXGI_FORMAT_B8G8R8A8_UNORM },
	{ GUID_WICPixelFormat32bppRGBA , DXGI_FORMAT_R8G8B8A8_UNORM },
	{ GUID_WICPixelFormat24bppBGR , DXGI_FORMAT_B8G8R8A8_UNORM },
	{ GUID_WICPixelFormat24bppRGB , DXGI_FORMAT_R8G8B8A8_UNORM },
};

bool ImageLoader::LoadImageFromFile(const std::filesystem::path& imagePath, ImageData& data)
{
	ComPointer<IWICImagingFactory> wicFactory;
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory))))
	{
		std::cout << "[D3D12_WIC] WIC Factory creation failed !!!" << std::endl;
		return false;
	}

	ComPointer<IWICStream> wicFileStream;
	if (FAILED(wicFactory->CreateStream(&wicFileStream)))
	{
		std::cout << "[D3D12_WIC] WIC File Stream creation failed !!!" << std::endl;
		return false;
	}

	if(FAILED(wicFileStream->InitializeFromFilename(imagePath.wstring().c_str(), GENERIC_READ)))
	{
		std::cout << "[D3D12_WIC] WIC File stream initializaion failed !!!" << " file: " << imagePath << " is missing." << std::endl;
		return false;
	}

	ComPointer<IWICBitmapDecoder> wicDecoder;
	if (FAILED(wicFactory->CreateDecoderFromStream(wicFileStream, nullptr, WICDecodeMetadataCacheOnDemand, &wicDecoder)))
	{
		std::cout << "[D3D12_WIC] Unable to create WIC Decoder !!!" << std::endl;
		return false;
	}

	ComPointer<IWICBitmapFrameDecode> wicFrameDecode;

	if (FAILED(wicDecoder->GetFrame(0, &wicFrameDecode)))
	{
		std::cout << "[D3D12_WIC] Unable to create WIC BitMap Frame decode !!!" << std::endl;
		return false;
	}

	if (FAILED(wicFrameDecode->GetSize(&data.width, &data.height)))
	{
		std::cout << "[D3D12_WIC] Unable to get image size !!!" << std::endl;
		return false;
	}

	if (FAILED(wicFrameDecode->GetPixelFormat(&data.wicPixelFormat)))
	{
		std::cout << "[D3D12_WIC] Unable to get GUID pixel format !!!" << std::endl;
		return false;
	}

	ComPointer<IWICComponentInfo> wicComponentInfo;
	if (FAILED(wicFactory->CreateComponentInfo(data.wicPixelFormat, &wicComponentInfo)))
	{
		std::cout << "[D3D12_WIC] Unable to get component info from WIC pixel format !!!" << std::endl;
		return false;
	}

	ComPointer<IWICPixelFormatInfo> wicPixelFormatInfo;
	if (FAILED(wicComponentInfo->QueryInterface(&wicPixelFormatInfo)))
	{
		std::cout << "[D3D12_WIC] Unable to get pixel format info from WIC component !!!" << std::endl;
		return false;
	}

	if (FAILED(wicPixelFormatInfo->GetBitsPerPixel(&data.bpp)))
	{
		std::cout << "[D3D12_WIC] Unable to get bit per pixel from WIC pixel format info !!!" << std::endl;
		return false;
	}

	if (FAILED(wicPixelFormatInfo->GetChannelCount(&data.cc)))
	{
		std::cout << "[D3D12_WIC] Unable to get bit channels count from WIC pixel format info !!!" << std::endl;
		return false;
	}


	auto findIt = std::find_if(s_lookupTable.begin(), s_lookupTable.end(),
			[&](const GUID_to_DXGI& entry)
			{
				return memcmp(&entry.wic, &data.wicPixelFormat, sizeof(GUID)) == 0;
			}
		);

	if (findIt == s_lookupTable.end())
	{
		return false;
	}

	data.giPixelFormat = findIt->gi;

	uint32_t stride = ((data.bpp + 7) / 8) * data.width;
	uint32_t size = stride * data.height;

	data.dataByte.resize(size);

	WICRect copyRect;
	copyRect.X = 0;
	copyRect.Y = 0;
	copyRect.Width = data.width;
	copyRect.Height = data.height;

	if (FAILED(wicFrameDecode->CopyPixels(&copyRect, stride, size, (BYTE*)data.dataByte.data())))
	{
		std::cout << "[D3D12_WIC] Unable to copy pixels !!!" << std::endl;
		return false;
	}

	std::cout << "[D3D12_WIC] Image "  << imagePath << " loaded !!!" << std::endl;

	return true;
}