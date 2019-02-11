//
// Created by ice10 on 2019/2/11.
//

#ifndef ARTEMIS_DANMUKU_H
#define ARTEMIS_DANMUKU_H

#include "imgui.h"
#include "texture.h"
#include "image.h"

#include <vector>

class AbstractTask {
public:
	virtual ~AbstractTask() = default;
	virtual void draw(double time) = 0;
	virtual void editor() = 0;
	virtual void setImage(const SubImage &newImage) = 0;
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
};

class SpellCard {
private:
	std::vector<AbstractTask *> tasks;

public:
	SpellCard() = default;
	void addTask(AbstractTask *task);
	AbstractTask *getTask(size_t index) const;
	size_t taskSize() const;
	void draw(double time);
};

#endif //ARTEMIS_DANMUKU_H
