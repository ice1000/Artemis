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
	double endTime() const;
protected:
	ImVec2 *pendingClick = nullptr;
	ImVec2 startScale = ImVec2(1, 1), endScale = ImVec2(1, 1);
	bool isHovered = false;
	bool showPath = false;
	double startTime = 0, stayTime = 1;
	float percentage(double time) const;
public:
	~AbstractTask() override = default;

	SubImage image;

	bool isSelected = false;
	bool isRightClicked = false;

	void drawMisc();

	virtual void draw(double time);
	virtual void extension(AbstractTask *other, Tasks &tasks);

	virtual void drawOtherMisc();
	virtual void editor();
	virtual void write(FILE *file) = 0;
	virtual void read(FILE *file, CompleteImage *complete) = 0;
	virtual ImVec2 calcPos(float percent) = 0;
	virtual TaskType type() = 0;
	static shared_ptr<AbstractTask> create(FILE *file);
};

class LinearTask : public DeriveClone<LinearTask, AbstractTask> {
private:
	ImVec2 startPos = ImVec2(50, 50), endPos = ImVec2(150, 150);
public:
	~LinearTask() override = default;
	ImVec2 calcPos(float percent) override;
	void extension(AbstractTask *other, Tasks &tasks) override;
	void drawOtherMisc() override;
	void editor() override;
	void write(FILE *file) override;
	void read(FILE *file, CompleteImage *complete) override;
	TaskType type() override;
};

/// John's appreciation to this little program gives me power to develop this
class CircularTask : public DeriveClone<CircularTask, AbstractTask> {
private:
	ImVec2 centerPos = ImVec2(100, 100);
	float radius = 10, startTheta = 0, increasingTheta = IM_PI;
	double endTheta() const;
public:
	~CircularTask() override = default;
	ImVec2 calcPos(float percent) override;

	// void extension(AbstractTask *other, Tasks &tasks) override;
	void drawOtherMisc() override;
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
