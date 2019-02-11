//
// Created by ice1000 on 2019/2/11.
//

#include "danmuku.h"

void SpellCard::draw(double time) {
	for (auto &task : tasks) task->draw(time);
}

void SpellCard::addTask(shared_ptr<AbstractTask> task) {
	tasks.emplace_back(task);
}

shared_ptr<AbstractTask> SpellCard::getTask(size_t index) const {
	return tasks[index];
}

size_t SpellCard::taskSize() const {
	return tasks.size();
}

void LinearTask::draw(double time) {
	if (time > endTime() || time < startTime) return;
	double percentage = (time - startTime) / stayTime;
	ImGui::SetCursorPos((endPos - startPos) * percentage + startPos);
	auto rotation_start_index = ImGui::BeginRotate();
	image.draw((endScale - startScale) * percentage + startScale);
	ImGui::EndRotate((endRotate - startRotate) * percentage + startRotate, rotation_start_index);
}

double LinearTask::endTime() const {
	return startTime + stayTime;
}

void LinearTask::editor() {
	bool sync = ImGui::GetIO().KeyAlt;
	if (ImGui::CollapsingHeader("Positioning")) {
		ImVec2 originalStart = startPos;
		if (ImGui::SliderFloat2("Start##Pos", reinterpret_cast<float *>(&startPos), 0, 700)) {
			if (sync && startPos.x != originalStart.x) endPos.x = startPos.x;
			if (sync && startPos.y != originalStart.y) endPos.y = startPos.y;
		}
		ImVec2 originalEnd = endPos;
		if (ImGui::SliderFloat2("End##Pos", reinterpret_cast<float *>(&endPos), 0, 700)) {
			if (sync && endPos.x != originalEnd.x) startPos.x = endPos.x;
			if (sync && endPos.y != originalEnd.y) startPos.y = endPos.y;
		}
	}
	if (ImGui::CollapsingHeader("Scaling")) {
		ImVec2 originalStart = startScale;
		if (ImGui::SliderFloat2("Start##Scale", reinterpret_cast<float *>(&startScale), -5, 5)) {
			if (sync && startScale.x != originalStart.x) endScale.x = startScale.x;
			if (sync && startScale.y != originalStart.y) endScale.y = startScale.y;
		}
		ImVec2 originalEnd = endScale;
		if (ImGui::SliderFloat2("End##Scale", reinterpret_cast<float *>(&endScale), -5, 5)) {
			if (sync && endScale.x != originalEnd.x) startScale.x = endScale.x;
			if (sync && endScale.y != originalEnd.y) startScale.y = endScale.y;
		}
	}
	if (ImGui::CollapsingHeader("Rotating")) {
		ImGui::SliderFloat("Start##Rotate", &startRotate, -IM_PI, IM_PI);
		ImGui::SliderFloat("End##Rotate", &endRotate, -IM_PI, IM_PI);
		if (ImGui::Button("Don't rotate")) {
			startRotate = IM_PI / 2;
			endRotate = IM_PI / 2;
		}
	}
	if (ImGui::CollapsingHeader("Timing")) {
		ImGui::SliderDouble("Task Start Time", &startTime, 0, 5);
		ImGui::SliderDouble("Task Stay Time", &stayTime, 0, 5);
	}
}

void LinearTask::setImage(const SubImage &newImage) {
	image = newImage;
}
