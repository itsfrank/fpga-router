#pragma once

#include <vector>

#include "logic_block.h"
#include "router.h"
#include "graphics.h"
#include "easygl_constants.h"


class LogicBlock;
class SegChannel;

using namespace std;

struct segment {
	SegChannel* parent;
	int net = -1;
	int g_index = -1;

	segment(SegChannel* parent);
};

class SegChannel
{
public:
	static int NUM_SEGMENTS;

	const static int LEFT = 0;
	const static int DOWN = 0;
	const static int RIGHT = 1;
	const static int UP = 1;

	static float s_padding;
	const static float s_width;
	const static float min_padding;

	bool is_vertical;
	LogicBlock* l_blocks[2];
	vector<segment*> segments;


	SegChannel(LogicBlock* l1, LogicBlock* l2, bool is_vertical);

	void draw();

	~SegChannel();

private:
	void drawV();
	void drawH();
};



