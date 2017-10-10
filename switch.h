#pragma once

#include<vector>

#include "seg_channel.h"
#include "graphics.h"
#include "easygl_constants.h"

class SegChannel;
struct segment;

struct connection {
	segment* a_ch;
	segment* b_ch;
};

class Switch
{
public:
	static const int UP = 0;
	static const int RIGHT = 1;
	static const int DOWN = 2;
	static const int LEFT = 3;

	int x_index, y_index;

	SegChannel** channels = new SegChannel*[4];
	std::vector<connection> connections;

	Switch(int x_index, int y_index, SegChannel* up, SegChannel* right, SegChannel* down, SegChannel* left);
	std::vector<segment*> getSwitchSegments();

	void draw();

	~Switch();
};

