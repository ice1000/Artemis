//
// Created by ice1000 on 2019/2/11.
//

#include "danmuku.h"

void SpellCard::draw(double time) {
	for (auto &task : tasks) task->drawMisc();
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

float AbstractTask::percentage(double time) const {
	return static_cast<float>((time - startTime) / stayTime);
}

double AbstractTask::endTime() const {
	return startTime + stayTime;
}

void LinearTask::editor() {
	AbstractTask::editor();
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

ImVec2 LinearTask::calcPos(float percent) {
	return (endPos - startPos) * percent + startPos;
}

void LinearTask::drawOtherMisc() {
	if (showPath) {
		ImGui::SetCursorPos({});
		ImVec2 &&delta = endPos - startPos;
		ImVec2 offset = startPos;
		float alpha = isSelected ? 1 : .7f;
		float thickness = isSelected ? 2 : .5f;
		ImGui::Line(offset + (image.size * startScale) / 2, delta,
		            ImVec4(1, 0, 0, alpha), thickness);
	}
}

#define INIT_L_R(Base, Ext, op) { \
LinearTask *l, *r; \
if (startPos.x > other->startPos.x) { \
  l = other; \
  r = this; \
} else { \
  l = this; \
  r = other; \
} \
auto task = make_shared<LinearTask>(); \
task->image = (Base)->image; \
task->startPos = (Base)->startPos op (r->startPos - l->startPos); \
task->endPos = (Base)->endPos op (r->endPos - l->endPos); \
task->startScale = (Base)->startScale op (r->startScale - l->startScale); \
task->endScale = (Base)->endScale op (r->endScale - l->endScale); \
tasks.emplace_back(task); \
(Ext)->isSelected = false; \
task->isSelected = true; \
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
		if (ImGui::Button("Left (Start Pos)")) INIT_L_R(l, r, -)
		if (ImGui::Button("Right (Start Pos)")) INIT_L_R(r, l, +)
		ImGui::TreePop();
	}
}

#undef INIT_L_R

shared_ptr<AbstractTask> AbstractTask::create(FILE *file) {
	TaskType taskType;
	FSCANF(file, "%i", &taskType);
	switch (taskType) {
		case Linear:
			return make_shared<LinearTask>();
		case Circular:
			return make_shared<CircularTask>();
		default:
			fprintf(stderr, "Unrecognized task type: %i", taskType);
			exit(-1);
	}
}

void AbstractTask::editor() {
	ImGui::Checkbox("Show Path", &showPath);
	bool sync = ImGui::GetIO().KeyAlt;
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

void AbstractTask::drawMisc() {
	drawOtherMisc();
	if (pendingClick) {
		*pendingClick = ImGui::GetIO().MousePos - ImGui::GetCurrentWindow()->Pos;
		if (ImGui::IsMouseDoubleClicked(0)) pendingClick = nullptr;
	}
}

void AbstractTask::draw(double time) {
	if (time > endTime() || time < startTime) return;
	auto rotation_start_index = ImGui::BeginRotate();
	auto percent = percentage(time);
	const ImVec2 &currentPos = calcPos(percent);
	const ImVec2 &previousPos = calcPos(percent - 0.001);
	ImGui::SetCursorPos(currentPos);
	const ImVec2 &dir = currentPos - previousPos;
	const ImVec2 &scale = (endScale - startScale) * percent + startScale;
	if (isSelected) image.drawWithBoarder(scale, ImVec4(1, 0, 0, 1));
	else if (isHovered) image.drawWithBoarder(scale, ImVec4(.8f, .1f, .1f, .6f));
	else image.draw(scale);
	isHovered = ImGui::IsItemHovered();
	float rotation = atan(-dir.y / dir.x);
	if (dir.x < 0) rotation += IM_PI;
	ImGui::EndRotate(rotation, rotation_start_index);
}

void AbstractTask::drawOtherMisc() {}

void AbstractTask::extension(AbstractTask *, Tasks &) {}

TaskType CircularTask::type() {
	return Circular;
}

double CircularTask::endTheta() const {
	return startTheta + increasingTheta;
}

void CircularTask::write(FILE *file) {
	image.write(file);
	fprintf(file, "%lf,%lf,%f,%f,", startTime, stayTime, startTheta,
	        increasingTheta);
	fprintf(file, "%f,%f,%f,", centerPos.x, centerPos.y, radius);
	fputs("", file);
	fflush(file);
}

void CircularTask::read(FILE *file, CompleteImage *complete) {
	image.read(file, complete);
	FSCANF(file, "%lf,%lf,%f,%f,", &startTime, &stayTime, &startTheta,
	       &increasingTheta);
	FSCANF(file, "%f,%f,%f,", &centerPos.x, &centerPos.y, &radius);
}

ImVec2 CircularTask::calcPos(float percent) {
	auto theta = startTheta + increasingTheta * percent;
	return ImVec2(ImSin(theta), ImCos(theta)) * radius + centerPos;
}

void CircularTask::editor() {
	AbstractTask::editor();
	ImGui::SliderFloat2("Center", reinterpret_cast<float *>(&centerPos),
	                    -1e3, 1e3);
	ImGui::SliderFloat("Radius", &radius, .0f, 300.0f);
}

