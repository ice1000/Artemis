//
// Created by ice1000 on 2019/2/11.
//

#include "danmuku.h"

void SpellCard::draw(double time) {
	for (auto &task : tasks) task->drawOthers();
	AbstractTask *selected = nullptr;
	// Explicit typed to make CLion happy
	for (shared_ptr<AbstractTask> &task : tasks) {
		task->draw(time);
		if (ImGui::IsItemClicked()) {
			if (ImGui::GetIO().KeyCtrl) task->isSelected = true;
			else selected = task.get();
		}
	}
	if (selected) {
		for (auto &task : tasks) task->isSelected = false;
		selected->isSelected = true;
	}
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

void SpellCard::write(FILE *file) {
	fprintf(file, "%i", tasks.size());
	for (const auto &task : tasks) {
		fprintf(file, "\n%i\n", task->type());
		task->write(file);
	}
}

void SpellCard::read(FILE *file, CompleteImage *complete) {
	size_t taskSize;
	FSCANF(file, "%i", &taskSize);
	for (size_t i = 0; i < taskSize; ++i) {
		auto task = AbstractTask::create(file);
		task->read(file, complete);
		addTask(task);
	}
}

void SpellCard::removeTask(size_t index) {
	tasks.erase(tasks.begin() + index);
}

void LinearTask::drawWithoutRotate(double time) {
	if (time > endTime() || time < startTime) return;
	auto percentage = static_cast<float>((time - startTime) / stayTime);
	const ImVec2 &scale = (endScale - startScale) * percentage + startScale;
	if (isSelected) image.drawWithBoarder(scale);
	else image.draw(scale);
}

double LinearTask::endTime() const {
	return startTime + stayTime;
}

void LinearTask::editor() {
	ImGui::Checkbox("Show Path", &showPath);
	bool sync = ImGui::GetIO().KeyAlt;
	if (ImGui::CollapsingHeader("Positioning")) {
		ImVec2 originalStart = startPos;
		if (ImGui::SliderFloat2("Start##Pos", reinterpret_cast<float *>(&startPos), 0, 700)) {
			if (sync && startPos.x != originalStart.x) endPos.x = startPos.x;
			if (sync && startPos.y != originalStart.y) endPos.y = startPos.y;
		}
		ImGui::SameLine();
		if (ImGui::Button("Mouse ...##StartPos")) pendingClick = &startPos;
		ImVec2 originalEnd = endPos;
		if (ImGui::SliderFloat2("End##Pos", reinterpret_cast<float *>(&endPos), 0, 700)) {
			if (sync && endPos.x != originalEnd.x) startPos.x = endPos.x;
			if (sync && endPos.y != originalEnd.y) startPos.y = endPos.y;
		}
		if (ImGui::Button("Mouse ...##EndPos")) pendingClick = &endPos;
	}
	if (ImGui::CollapsingHeader("Scaling")) {
		if (ImGui::Button("Reset##StartScale")) startScale = ImVec2(1, 1);
		ImGui::SameLine();
		ImVec2 originalStart = startScale;
		if (ImGui::SliderFloat2("Start##Scale", reinterpret_cast<float *>(&startScale), -5, 5)) {
			if (sync && startScale.x != originalStart.x) endScale.x = startScale.x;
			if (sync && startScale.y != originalStart.y) endScale.y = startScale.y;
		}
		if (ImGui::Button("Reset##EndScale")) endScale = ImVec2(1, 1);
		ImGui::SameLine();
		ImVec2 originalEnd = endScale;
		if (ImGui::SliderFloat2("End##Scale", reinterpret_cast<float *>(&endScale), -5, 5)) {
			if (sync && endScale.x != originalEnd.x) startScale.x = endScale.x;
			if (sync && endScale.y != originalEnd.y) startScale.y = endScale.y;
		}
	}
	if (ImGui::CollapsingHeader("Timing")) {
		ImGui::SliderDouble("Task Start Time", &startTime, 0, 5);
		ImGui::SliderDouble("Task Stay Time", &stayTime, 0, 5);
	}
}

void LinearTask::write(FILE *file) {
	image.write(file);
	fprintf(file, "%lf,%lf,", startTime, stayTime);
	fprintf(file, "%f,%f,%f,%f,", startScale.x, startScale.y, endScale.x, endScale.y);
	fprintf(file, "%f,%f,%f,%f,", startPos.x, startPos.y, endPos.x, endPos.y);
	fputs("", file);
	fflush(file);
}

void LinearTask::read(FILE *file, CompleteImage *complete) {
	image.read(file, complete);
	FSCANF(file, "%lf,%lf,", &startTime, &stayTime);
	FSCANF(file, "%f,%f,%f,%f,", &startScale.x, &startScale.y, &endScale.x, &endScale.y);
	FSCANF(file, "%f,%f,%f,%f,", &startPos.x, &startPos.y, &endPos.x, &endPos.y);
}

TaskType LinearTask::type() {
	return Linear;
}

ImVec2 LinearTask::calcPos(double time) {
	auto percentage = static_cast<float>((time - startTime) / stayTime);
	return (endPos - startPos) * percentage + startPos;
}

void LinearTask::drawOthers() {
	if (showPath) {
		ImGui::SetCursorPos({});
		ImVec2 &&delta = endPos - startPos;
		ImVec2 offset = startPos;
		float alpha = isSelected ? 1 : .7f;
		float thickness = isSelected ? 2 : .5;
		ImGui::Line(offset + (image.size * startScale) / 2, delta, ImVec4(1, 0, 0, alpha), thickness);
	}
	if (pendingClick) {
		const auto &mouse = ImGui::GetIO().MousePos - ImGui::GetCurrentWindow()->Pos;
		*pendingClick = mouse;
		if (ImGui::IsMouseDoubleClicked(0)) pendingClick = nullptr;
	}
}

shared_ptr<AbstractTask> AbstractTask::create(FILE *file) {
	TaskType taskType;
	FSCANF(file, "%i", &taskType);
	switch (taskType) {
		case Linear:
			return make_shared<LinearTask>();
		default:
			fprintf(stderr, "Unrecognized task type: %i", taskType);
			exit(-1);
	}
}

void AbstractTask::draw(double time) {
	auto rotation_start_index = ImGui::BeginRotate();
	const ImVec2 &currentPos = calcPos(time);
	const ImVec2 &previousPos = calcPos(time - 0.0001);
	ImGui::SetCursorPos(currentPos);
	const ImVec2 &dir = currentPos - previousPos;
	// FIXME wrong calculation
	float rotation = atan(-dir.y / dir.x);
	drawWithoutRotate(time);
	ImGui::EndRotate(rotation, rotation_start_index);
}

void AbstractTask::drawOthers() {}
