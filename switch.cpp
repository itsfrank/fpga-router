#include "switch.h"


Switch::Switch(int x_index, int y_index, SegChannel* up, SegChannel* right, SegChannel* down, SegChannel* left)
{
	this->x_index = x_index;
	this->y_index = y_index;

	this->channels[UP] = up;
	this->channels[RIGHT] = right;
	this->channels[DOWN] = down;
	this->channels[LEFT] = left;
}

std::vector<segment*> Switch::getSwitchSegments()
{
	vector<segment*> segments;

	for (int i = 0; i < 4; i++)
	{
		if (this->channels[i]) {
			for (auto seg : this->channels[i]->segments) {
				segments.push_back(seg);
			}
		}
	}

	return segments;
}

void Switch::draw()
{
	setcolor(LIGHTGREY);
	setlinestyle(DASHED);
	
	float x_offset = (this->x_index * 2 * LogicBlock::block_draw_width) - LogicBlock::block_draw_width;
	float y_offset = (this->y_index * 2 * LogicBlock::block_draw_width) - LogicBlock::block_draw_width;

	
	drawrect(x_offset, y_offset, x_offset + LogicBlock::block_draw_width, y_offset + LogicBlock::block_draw_width);
}


Switch::~Switch()
{
}
