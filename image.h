//
// Created by ice10 on 2019/2/11.
//

#ifndef ARTEMIS_IMAGE_H
#define ARTEMIS_IMAGE_H

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_internal.h"
#include "texture.h"

#ifdef WIN32
#define FSCANF fscanf_s
#else
#define FSCANF fscanf
#endif

namespace ImGui {
	void Line(const ImVec2 &offset, const ImVec2 &delta, const ImVec4 &color,
	          float thickness);
	void LineFromTo(const ImVec2 &from, const ImVec2 &to, float thickness,
	                const ImVec4 &color);
	void Line(const ImVec2 &delta, const ImVec4 &color, float thickness);
	void Line(const ImVec2 &delta, float thickness);
	bool SliderDouble(const char *label, double *v, double v_min, double v_max,
	                  const char *format = "%.6lf", double power = 1.0);
	ImVec2 RotationCenter(size_t rotation_start_index);
	void EndRotate(float rad, size_t rotation_start_index);
	void EndRotate(float rad, size_t rotation_start_index, ImVec2 center);

	inline auto
	BeginRotate() noexcept { return ImGui::GetWindowDrawList()->VtxBuffer.Size; }
}

class CompleteImage;

class SubImage {
private:
	const CompleteImage *completeImage;
public:
	ImVec2 size;
	ImVec2 pos;

	explicit SubImage(const CompleteImage *completeImage);
	explicit SubImage();

	ImVec2 uv0() const;
	ImVec2 uv1() const;
	ImVec2 fullSize() const;
	void draw(const ImVec2 &scale = ImVec2(1, 1));
	bool drawButton(const ImVec2 &scale = ImVec2(1, 1));
	void drawWithBoarder(const ImVec2 &scale = ImVec2(1, 1),
	                     const ImVec4 &border_col = ImVec4(1, 0, 0, 1));
	void write(FILE *file);
	void read(FILE *file, CompleteImage *complete);
};

class CompleteImage {
public:
	ImTextureID textureID = nullptr;
	ImVec2 fullSize;

	static bool fromFile(const char *fileName, CompleteImage &subimage);

	SubImage toSubImage() const;
};

#endif //ARTEMIS_IMAGE_H
