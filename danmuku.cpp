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
		task->isRightClicked = ImGui::IsItemClicked(1);
	}
	if (selected) {
		for (auto &task : tasks) task->isSelected = false;
		selected->isSelected = true;
	}
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
		tasks.emplace_back(task);
	}
}

Tasks &SpellCard::getTasks() {
	return tasks;
}

void LinearTask::drawWithoutRotate(double time) {
	if (time > endTime() || time < startTime) return;
	auto percentage = static_cast<float>((time - startTime) / stayTime);
	const ImVec2 &scale = (endScale - startScale) * percentage + startScale;
	if (isSelected) image.drawWithBoarder(scale, ImVec4(1, 0, 0, 1));
	else if (isHovered) image.drawWithBoarder(scale, ImVec4(.9f, 0, 0, .7f));
	else image.draw(scale);
	isHovered = ImGui::IsItemHovered();
}

double LinearTask::endTime() const {
	return startTime + stayTime;
}

void LinearTask::editor() {
	ImGui::Checkbox("Show Path", &showPath);
	bool sync = ImGui::GetIO().KeyAlt;
	if (ImGui::CollapsingHeader("Positioning")) {
		if (ImGui::Button("Mouse ...##StartPos")) pendingClick = &startPos;
		ImGui::SameLine();
		ImVec2 originalStart = startPos;
		if (ImGui::SliderFloat2("Start##Pos", reinterpret_cast<float *>(&startPos),
		                        0, 700)) {
			if (sync && startPos.x != originalStart.x)
				endPos.x += startPos.x - originalStart.x;
			if (sync && startPos.y != originalStart.y)
				endPos.y += startPos.y - originalStart.y;
		}
		if (ImGui::Button("Mouse ...##EndPos")) pendingClick = &endPos;
		ImGui::SameLine();
		ImVec2 originalEnd = endPos;
		if (ImGui::SliderFloat2("End##Pos", reinterpret_cast<float *>(&endPos), 0,
		                        700)) {
			if (sync && endPos.x != originalEnd.x)
				startPos.x += endPos.x - originalEnd.x;
			if (sync && endPos.y != originalEnd.y)
				startPos.y += endPos.y - originalEnd.y;
		}
	}
	if (ImGui::CollapsingHeader("Scaling")) {
		if (ImGui::Button("Reset##StartScale")) startScale = ImVec2(1, 1);
		ImGui::SameLine();
		ImVec2 originalStart = startScale;
		if (ImGui::SliderFloat2("Start##Scale",
		                        reinterpret_cast<float *>(&startScale), -5, 5)) {
			if (sync && startScale.x != originalStart.x) endScale.x = startScale.x;
			if (sync && startScale.y != originalStart.y) endScale.y = startScale.y;
		}
		if (ImGui::Button("Reset##EndScale")) endScale = ImVec2(1, 1);
		ImGui::SameLine();
		ImVec2 originalEnd = endScale;
		if (ImGui::SliderFloat2("End##Scale", reinterpret_cast<float *>(&endScale),
		                        -5, 5)) {
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
	fprintf(file, "%f,%f,%f,%f,", startScale.x, startScale.y, endScale.x,
	        endScale.y);
	fprintf(file, "%f,%f,%f,%f,", startPos.x, startPos.y, endPos.x, endPos.y);
	fputs("", file);
	fflush(file);
}

void LinearTask::read(FILE *file, CompleteImage *complete) {
	image.read(file, complete);
	FSCANF(file, "%lf,%lf,", &startTime, &stayTime);
	FSCANF(file, "%f,%f,%f,%f,", &startScale.x, &startScale.y, &endScale.x,
	       &endScale.y);
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
		float thickness = isSelected ? 2 : .5f;
		ImGui::Line(offset + (image.size * startScale) / 2, delta,
		            ImVec4(1, 0, 0, alpha), thickness);
	}
	if (pendingClick) {
		*pendingClick = ImGui::GetIO().MousePos - ImGui::GetCurrentWindow()->Pos;
		if (ImGui::IsMouseDoubleClicked(0)) pendingClick = nullptr;
	}
}

#define GEN_L_R LinearTask *l, *r; \
if (startPos.x > other->startPos.x) { \
  l = other; \
  r = this; \
} else { \
  l = this; \
  r = other; \
}

void LinearTask::extension(AbstractTask *absOther, Tasks &tasks) {
	auto *other = dynamic_cast<LinearTask *>(absOther);
	if (!other) return;
	if (ImGui::TreeNode("Next Permutation")) {
		if (ImGui::Button("Between")) {
			auto task = make_shared<LinearTask>();
			task->image = image;
			task->startPos = (startPos + other->startPos) / 2;
			task->endPos = (endPos + other->endPos) / 2;
			task->startScale = (startScale + other->startScale) / 2;
			task->endScale = (endScale + other->endScale) / 2;
			tasks.emplace_back(task);
		}
		if (ImGui::Button("Left (Start Pos)")) {
			GEN_L_R
			auto task = make_shared<LinearTask>();
			task->image = l->image;
			task->startPos = l->startPos - (r->startPos - l->startPos);
			task->endPos = l->endPos - (r->endPos - l->endPos);
			task->startScale = l->startScale - (r->startScale - l->startScale);
			task->endScale = l->endScale - (r->endScale - l->endScale);
			tasks.emplace_back(task);
			r->isSelected = false;
			task->isSelected = true;
		}
		if (ImGui::Button("Right (Start Pos)")) {
			GEN_L_R
			auto task = make_shared<LinearTask>();
			task->image = r->image;
			task->startPos = r->startPos + (r->startPos - l->startPos);
			task->endPos = r->endPos + (r->endPos - l->endPos);
			task->startScale = r->startScale + (r->startScale - l->startScale);
			task->endScale = r->endScale + (r->endScale - l->endScale);
			tasks.emplace_back(task);
			l->isSelected = false;
			task->isSelected = true;
		}
		ImGui::TreePop();
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
	drawWithoutRotate(time);
	float rotation = atan(-dir.y / dir.x);
	if (dir.x < 0) rotation += IM_PI;
	ImGui::EndRotate(rotation, rotation_start_index);
}

void AbstractTask::drawOthers() {}

void AbstractTask::extension(AbstractTask *other, Tasks &tasks) {}
