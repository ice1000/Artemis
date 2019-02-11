//
// Created by ice10 on 2019/2/11.
//

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "texture.h"

#include <d3d11.h>

HRESULT initTexture(void *imageData, ID3D11ShaderResourceView **texture, int width, int height) {
	HRESULT hr{S_OK};

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = 0;

	size_t bpp = 32;
	size_t rowPitch = (width * bpp + 7) / 8;
	size_t imageSize = rowPitch * height;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = imageData;
	initData.SysMemPitch = static_cast<UINT>(rowPitch);
	initData.SysMemSlicePitch = static_cast<UINT>(imageSize);

	ID3D11Texture2D *tex = nullptr;
	hr = g_pd3dDevice->CreateTexture2D(
			&desc,
			&initData,
			&tex);
	if (SUCCEEDED(hr) && tex) {
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = desc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		ID3D11ShaderResourceView *localTexture = nullptr;
		hr = g_pd3dDevice->CreateShaderResourceView(
				tex,
				&SRVDesc,
				&localTexture);
		if (SUCCEEDED(hr) && localTexture)
			*texture = localTexture;
		tex->Release();
	}

	return hr;
}

bool loadTexture(unsigned char *rawData, size_t dataSize, size_t &width, size_t &height, ImTextureID &texture) {
	bool result{false};
	int channels;
	int forceChannels = 4;
	int w, h;
	auto *imageData = stbi_load_from_memory(rawData, static_cast<int>(dataSize), &w, &h, &channels, forceChannels);
	width = static_cast<size_t>(w);
	height = static_cast<size_t>(h);

	if (nullptr != imageData) {
		ID3D11ShaderResourceView *localTexture = nullptr;
		if (SUCCEEDED(initTexture(imageData, &localTexture, w, h))) {
			result = true;
			texture = reinterpret_cast<ImTextureID>(localTexture);
		}

		stbi_image_free(imageData);
	}

	return result;
}

bool loadTexture(const char *fileName, size_t &width, size_t &height, ImTextureID &texture) {
	bool result = false;

	int channels;
	int forceChannels = 4;
	int w, h;
	auto *imageData = stbi_load(fileName, &w, &h, &channels, forceChannels);
	width = static_cast<size_t>(w);
	height = static_cast<size_t>(h);
	if (nullptr != imageData) {
		ID3D11ShaderResourceView *localTexture = nullptr;
		if (SUCCEEDED(initTexture(imageData, &localTexture, w, h))) {
			result = true;
			texture = reinterpret_cast<ImTextureID>(localTexture);
		}

		stbi_image_free(imageData);
	}

	return result;
}
