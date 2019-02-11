//
// Created by ice10 on 2019/2/11.
//

#ifndef ARTEMIS_DANMUKU_H
#define ARTEMIS_DANMUKU_H

#include "imgui.h"
#include "texture.h"
#include "image.h"

#include <vector>
#include <memory>
#include <cstdio>

using std::vector;
using std::shared_ptr;
using std::make_shared;

enum TaskType {
	Linear = 0,
};

class AbstractTask {
public:
	virtual ~AbstractTask() = default;
	virtual void draw(double time) = 0;
	virtual void editor() = 0;
	virtual void setImage(const SubImage &newImage) = 0;
	virtual void write(FILE *file) = 0;
	virtual void read(FILE *file, CompleteImage *complete) = 0;
	virtual TaskType type() = 0;
	static shared_ptr<AbstractTask> create(FILE *file);
};

class LinearTask : public AbstractTask {
private:
	SubImage image;
	ImVec2 startPos, endPos, startScale = ImVec2(1, 1), endScale = ImVec2(1, 1);
	double startTime = 0, stayTime = 1;
	float startRotate = IM_PI / 2, endRotate = IM_PI / 2;
	double endTime() const;
public:
	~LinearTask() override = default;
	void draw(double time) override;
	void editor() override;
	void setImage(const SubImage &newImage) override;
	void write(FILE *file) override;
	void read(FILE *file, CompleteImage *complete) override;
	TaskType type() override;
};

class SpellCard {
private:
	vector<shared_ptr<AbstractTask>> tasks;

public:
	SpellCard() = default;
	void addTask(shared_ptr<AbstractTask> task);
	void removeTask(size_t index);
	shared_ptr<AbstractTask> getTask(size_t index) const;
	size_t taskSize() const;
	void draw(double time);
	void write(FILE *file);
	void read(FILE *file, CompleteImage *complete);
};

#endif //ARTEMIS_DANMUKU_H
