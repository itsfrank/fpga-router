#pragma once

#include <string>

#include "seg_channel.h"
#include "graphics.h"
#include "easygl_constants.h"

class SegChannel;

class LogicBlock
{
public:
	static const int UP = 0;
	static const int RIGHT = 1;
	static const int DOWN = 2;
	static const int LEFT = 3;

	static const float MIN_DRAW_WIDTH;
	static float block_draw_width;


	int x_index;
	int y_index;
	SegChannel** channels;

	int pin_nets[4] = { -1, -1, -1, -1 };

	LogicBlock(int x_index, int y_index);

	void addChannel(int direction, SegChannel* channel);

	SegChannel* getTopCh();
	SegChannel* getRightCh();
	SegChannel* getBottomCh();
	SegChannel* getLeftCh();

	void draw();
	~LogicBlock();
};

