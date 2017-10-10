#include "logic_block.h"
#include <exception>

float LogicBlock::block_draw_width;
const float LogicBlock::MIN_DRAW_WIDTH = 100;

LogicBlock::LogicBlock(int x_index, int y_index)
{
	if (SegChannel::NUM_SEGMENTS == -1) throw exception("Initialize NUM_SEGMENTS before creating LogicBlocks");
	
	this->x_index = x_index;
	this->y_index = y_index;
	this->channels = new SegChannel*[4];
}


void LogicBlock::addChannel(int direction, SegChannel* channel)
{
	if (direction < 0 || direction > 3) throw std::exception("received out of bounds direction");

	this->channels[direction] = channel;
}

SegChannel * LogicBlock::getTopCh()
{
	return this->channels[LogicBlock::UP];
}

SegChannel * LogicBlock::getRightCh()
{
	return this->channels[LogicBlock::RIGHT];
}

SegChannel * LogicBlock::getBottomCh()
{
	return this->channels[LogicBlock::DOWN];
}

SegChannel * LogicBlock::getLeftCh()
{
	return this->channels[LogicBlock::LEFT];
}




void LogicBlock::draw()
{
	setcolor(LIGHTGREY);

	float x_offset = this->x_index * (2 * LogicBlock::block_draw_width);
	float y_offset = this->y_index * (2 * LogicBlock::block_draw_width);

	fillrect(x_offset, y_offset, x_offset + LogicBlock::block_draw_width, y_offset + LogicBlock::block_draw_width);

	setcolor(BLACK);
	setlinestyle(SOLID);
	string coords = to_string(this->x_index + 1) + " : " + to_string(this->y_index + 1);
	drawtext(x_offset + (LogicBlock::block_draw_width / 2), y_offset + (LogicBlock::block_draw_width / 2), coords.c_str(), INT_MAX);
	setcolor(DARKGREY);
	drawrect(x_offset, y_offset, x_offset + LogicBlock::block_draw_width, y_offset + LogicBlock::block_draw_width);

	for (int i = 0; i < 4; i++) {
		if (this->pin_nets[i] != -1) {
			int seg_i;
			segment* seg = NULL;
			for (int j = 0; j < this->channels[i]->segments.size(); j++) {
				segment* s = this->channels[i]->segments[j];
				if (s->net == this->pin_nets[i]) {
					seg_i = j;
					seg = s;
				}
			}

			int seg_offset = this->channels[i]->getSegmentOffset(seg_i);

			setlinestyle(SOLID);
			setlinewidth(SegChannel::s_width);
			setcolor((seg->net % 7) + 4);

			if (i == LogicBlock::UP) {
				drawline(
					x_offset + (LogicBlock::block_draw_width / 3), y_offset + LogicBlock::block_draw_width,
					x_offset + (LogicBlock::block_draw_width / 3), y_offset + LogicBlock::block_draw_width + seg_offset
				);
			}
			else if (i == LogicBlock::DOWN) {
				drawline(
					x_offset + (2 * LogicBlock::block_draw_width / 3), y_offset,
					x_offset + (2 * LogicBlock::block_draw_width / 3), y_offset - LogicBlock::block_draw_width + seg_offset
				);
			}
			else if (i == LogicBlock::RIGHT) {
				drawline(
					x_offset + LogicBlock::block_draw_width, y_offset + (2 * LogicBlock::block_draw_width / 3),
					x_offset + LogicBlock::block_draw_width + seg_offset, y_offset + (2 * LogicBlock::block_draw_width / 3)
				);
			}
			else if (i == LogicBlock::LEFT) {
				drawline(
					x_offset, y_offset + (LogicBlock::block_draw_width / 3),
					x_offset - LogicBlock::block_draw_width + seg_offset, y_offset + (LogicBlock::block_draw_width / 3)
				);
			}
		}
	}
}

LogicBlock::~LogicBlock()
{
}
