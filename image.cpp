//
// Created by ice10 on 2019/2/11.
//

#include "image.h"

ImVec2 SubImage::uv0() const {
	return pos / fullSize();
}

ImVec2 SubImage::uv1() const {
	return (pos + size) / fullSize();
}

ImVec2 SubImage::fullSize() const {
	return completeImage->fullSize;
}

void SubImage::draw(const ImVec2 &scale) {
	ImGui::Image(completeImage->textureID, size * scale, uv0(), uv1());
}

bool SubImage::drawButton(const ImVec2 &scale) {
	return ImGui::ImageButton(completeImage->textureID, size * scale, uv0(),
	                          uv1());
}

void SubImage::drawWithBoarder(const ImVec2 &scale, const ImVec4 &border_col) {
	ImGui::Image(completeImage->textureID, size * scale, uv0(), uv1(),
	             ImVec4(2, 2, 2, 2),
	             border_col);
}

SubImage::SubImage(const CompleteImage *completeImage) : completeImage(
		completeImage) {
	size = completeImage->fullSize;
}

void SubImage::write(FILE *file) {
	fprintf(file, "%f,%f,", pos.x, pos.y);
	fprintf(file, "%f,%f,", size.x, size.y);
}

void SubImage::read(FILE *file, CompleteImage *complete) {
	this->completeImage = complete;
	FSCANF(file, "%f,%f,", &pos.x, &pos.y);
	FSCANF(file, "%f,%f,", &size.x, &size.y);
}

SubImage::SubImage() : completeImage(nullptr) {}

bool CompleteImage::fromFile(const char *fileName, CompleteImage &subimage) {
	ImTextureID t;
	size_t w, h;
	if (!loadTexture(fileName, w, h, t)) return false;
	subimage.textureID = t;
	subimage.fullSize.x = static_cast<float>(w);
	subimage.fullSize.y = static_cast<float>(h);
	return true;
}

SubImage CompleteImage::toSubImage() const {
	return SubImage(this);
}

void ImGui::Line(const ImVec2 &offset, const ImVec2 &delta, const ImVec4 &color,
                 float thickness) {
	ImGuiWindow *window = GImGui->CurrentWindow;
	if (window->SkipItems) return;
	auto &&cursorPos = window->DC.CursorPos + offset;
	ImRect bb{cursorPos, cursorPos + delta};
	// ItemSize(bb);
	if (!ItemAdd(bb, 0)) return;
	window->DrawList->AddLine(bb.Min, bb.Max, GetColorU32(color), thickness);
}

void ImGui::Line(const ImVec2 &delta, const ImVec4 &color, float thickness) {
	Line({}, delta, color, thickness);
}

void ImGui::Line(const ImVec2 &delta, const float thickness) {
	Line(delta, ImGui::GetStyle().Colors[ImGuiCol_PlotLines], thickness);
}

bool
ImGui::SliderDouble(const char *label, double *v, double v_min, double v_max,
                    const char *format, double power) {
	return SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format,
	                    power);
}

ImVec2 ImGui::RotationCenter(size_t rotation_start_index) {
	ImVec2 l{FLT_MAX, FLT_MAX}, u{-FLT_MAX, -FLT_MAX}; // bounds

	const auto &buf = ImGui::GetWindowDrawList()->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

	return (l + u) / 2; // or use _ClipRectStack?
}

void ImGui::EndRotate(float rad, size_t rotation_start_index) {
	EndRotate(rad, rotation_start_index, RotationCenter(rotation_start_index));
}

void ImGui::EndRotate(float rad, size_t rotation_start_index, ImVec2 center) {
	float s = ImSin(rad), c = ImCos(rad);
	center = ImRotate(center, s, c) - center;

	auto &buf = ImGui::GetWindowDrawList()->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}
