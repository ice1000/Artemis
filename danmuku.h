//
// Created by ice10 on 2019/2/11.
//

#ifndef ARTEMIS_DANMUKU_H
#define ARTEMIS_DANMUKU_H

#include "imgui.h"
#include "texture.h"
#include "image.h"
#include "clone.h"

#include <vector>
#include <cstdio>

using std::vector;

enum TaskType {
	Linear = 0,
	Circular,
};

class AbstractTask;

using Tasks = vector<shared_ptr<AbstractTask>>;

class AbstractTask : public Clone<AbstractTask> {
private:
public:
	~AbstractTask() override = default;

	SubImage image;

	bool isSelected = false;
	bool isHovered = false;
	bool isRightClicked = false;

	virtual void draw(double time);
	virtual void extension(AbstractTask *other, Tasks &tasks);

	virtual void drawWithoutRotate(double time) = 0;
	virtual void drawOthers();
	virtual void editor() = 0;
	virtual void write(FILE *file) = 0;
	virtual void read(FILE *file, CompleteImage *complete) = 0;
	virtual ImVec2 calcPos(double time) = 0;
	virtual TaskType type() = 0;
	static shared_ptr<AbstractTask> create(FILE *file);
};

class LinearTask : public DeriveClone<LinearTask, AbstractTask> {
private:
	ImVec2 startPos, endPos, startScale = ImVec2(1, 1), endScale = ImVec2(1, 1);
	ImVec2 *pendingClick = nullptr;
	bool showPath = false;
	double startTime = 0, stayTime = 1;
	double endTime() const;
public:
	~LinearTask() override = default;
	ImVec2 calcPos(double time) override;
	void extension(AbstractTask *other, Tasks &tasks) override;
	void drawWithoutRotate(double time) override;
	void drawOthers() override;
	void editor() override;
	void write(FILE *file) override;
	void read(FILE *file, CompleteImage *complete) override;
	TaskType type() override;
};

class SpellCard {
private:
	Tasks tasks;

public:
	SpellCard() = default;
	Tasks &getTasks();
	void draw(double time);
	void write(FILE *file);
	void read(FILE *file, CompleteImage *complete);
};

#endif //ARTEMIS_DANMUKU_H
