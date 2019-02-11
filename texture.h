//
// Created by ice10 on 2019/2/11.
//

#ifndef ARTEMIS_TEXTURE_H
#define ARTEMIS_TEXTURE_H

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

extern ID3D11Device *g_pd3dDevice;

bool loadTexture(unsigned char *rawData, size_t dataSize, size_t &width, size_t &height, ImTextureID &texture);
bool loadTexture(const char *fileName, size_t &width, size_t &height, ImTextureID &texture);

#endif //ARTEMIS_TEXTURE_H
