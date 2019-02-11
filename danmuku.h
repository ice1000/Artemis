//
// Created by ice10 on 2019/2/11.
//

#ifndef ARTEMIS_DANMUKU_H
#define ARTEMIS_DANMUKU_H

#include "imgui.h"
#include "texture.h"
#include "image.h"

class Task {
private:
	SubImage image;
	ImVec2 startPos, endPos, startScale = ImVec2(1, 1), endScale = ImVec2(1, 1);
	double startTime = 0, stayTime = 0;
public:
	void draw(double time);
	void editor();
	void setImage(const SubImage &newImage);
	double endTime() const;
};

class SpellCard {
private:
	ImVector<Task> tasks;

public:
	SpellCard() = default;
	void draw(double time);
};

#endif //ARTEMIS_DANMUKU_H
