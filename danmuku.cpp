//
// Created by ice1000 on 2019/2/11.
//

#include "danmuku.h"

void SpellCard::draw(double time) {
	for (auto &task : tasks) task.draw(time);
}

void Task::draw(double time) {
	if (time > endTime() || time < startTime) return;
	double percentage = (time - startTime) / stayTime;
	ImGui::SetCursorPos((endPos - startPos) * percentage + startPos);
	image.draw((endScale - startScale) * percentage + startScale);
}

double Task::endTime() const {
	return startTime + stayTime;
}

void Task::editor() {
	bool sync = ImGui::GetIO().KeyAlt;
	if (ImGui::TreeNode("Positioning")) {
		ImVec2 originalStart = startPos;
		if (ImGui::SliderFloat2("Start", reinterpret_cast<float *>(&startPos), 0, 300)) {
			if (sync && startPos.x != originalStart.x) endPos.x = startPos.x;
			if (sync && startPos.y != originalStart.y) endPos.y = startPos.y;
		}
		ImVec2 originalEnd = endPos;
		if (ImGui::SliderFloat2("End", reinterpret_cast<float *>(&endPos), 0, 300)) {
			if (sync && endPos.x != originalEnd.x) startPos.x = endPos.x;
			if (sync && endPos.y != originalEnd.y) startPos.y = endPos.y;
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Scaling")) {
		ImVec2 originalStart = startScale;
		if (ImGui::SliderFloat2("Start", reinterpret_cast<float *>(&startScale), -5, 5)) {
			if (sync && startScale.x != originalStart.x) endScale.x = startScale.x;
			if (sync && startScale.y != originalStart.y) endScale.y = startScale.y;
		}
		ImVec2 originalEnd = endScale;
		if (ImGui::SliderFloat2("End", reinterpret_cast<float *>(&endScale), -5, 5)) {
			if (sync && endScale.x != originalEnd.x) startScale.x = endScale.x;
			if (sync && endScale.y != originalEnd.y) startScale.y = endScale.y;
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Timing")) {
		ImGui::SliderDouble("Task Start Time", &startTime, 0, 10);
		ImGui::SliderDouble("Task Stay Time", &stayTime, 0, 10);
		ImGui::TreePop();
	}
}

void Task::setImage(const SubImage &newImage) {
	Task::image = newImage;
}
