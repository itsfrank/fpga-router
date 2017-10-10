#include "seg_channel.h"
#include <string>

int SegChannel::NUM_SEGMENTS = -1;
float SegChannel::s_padding;
const float SegChannel::s_width = 2;
const float SegChannel::min_padding = 2;

segment::segment(SegChannel* parent) {
	this->parent = parent;
	this->net = -1;
	this->g_index = -1;
}

SegChannel::SegChannel(LogicBlock* l1, LogicBlock* l2, bool is_vertical)
{
	if (NUM_SEGMENTS == -1) throw exception("Initialize NUM_SEGMENTS before creating SegChannels");
	
	this->is_vertical = is_vertical;
	this->l_blocks[0] = l1;
	this->l_blocks[1] = l2;

	for (int i = 0; i < NUM_SEGMENTS; i++)
	{
		this->segments.push_back(new segment(this));
	}
}

void SegChannel::draw()
{
	if (this->is_vertical) this->drawV();
	else this->drawH();
}

float SegChannel::getSegmentOffset(int i)
{
	return ((i + 1) * SegChannel::s_padding) + (i * SegChannel::s_width);
}


SegChannel::~SegChannel()
{
}


void SegChannel::drawV()
{
	float sq_x_offset;
	float sq_y_offset;
	float sq_width = LogicBlock::block_draw_width;

	LogicBlock* ofsset_lb = this->l_blocks[LEFT];
	if (ofsset_lb != NULL) {
		sq_x_offset = (ofsset_lb->x_index * 2 * sq_width) + sq_width;
		sq_y_offset = (ofsset_lb->y_index * 2 * sq_width);
	}
	else {
		ofsset_lb = this->l_blocks[RIGHT];
		sq_x_offset = (ofsset_lb->x_index *  2 * sq_width) - sq_width;
		sq_y_offset = (ofsset_lb->y_index *  2 * sq_width);
	}

	setcolor(LIGHTGREY);
	setlinestyle(SOLID);
	setlinewidth(SegChannel::s_width);

	for (int i = 0; i < SegChannel::NUM_SEGMENTS; i++)
	{
		setcolor(LIGHTGREY);

		if (segments[i]->net != -1) {
			setcolor((segments[i]->net % 7) + 4);
		}

		float inner_offset = ((i + 1) * SegChannel::s_padding) + (i * SegChannel::s_width);
		drawline(inner_offset + sq_x_offset, sq_y_offset, inner_offset + sq_x_offset, sq_y_offset + sq_width);
		setcolor(BLACK);
		drawtext(inner_offset + sq_x_offset, sq_y_offset + sq_width / 2, std::to_string(this->segments[i]->g_index).c_str(), INT_MAX);
	}
}

void SegChannel::drawH()
{
	float sq_x_offset;
	float sq_y_offset;
	float sq_width = LogicBlock::block_draw_width;

	LogicBlock* ofsset_lb = this->l_blocks[DOWN];
	if (ofsset_lb != NULL) {
		sq_x_offset = (ofsset_lb->x_index * 2 * sq_width);
		sq_y_offset = (ofsset_lb->y_index * 2 * sq_width) + sq_width;
	}
	else {
		ofsset_lb = this->l_blocks[UP];
		sq_x_offset = (ofsset_lb->x_index * 2 * sq_width);
		sq_y_offset = (ofsset_lb->y_index * 2 * sq_width) - sq_width;
	}

	setcolor(LIGHTGREY);
	setlinestyle(SOLID);
	setlinewidth(SegChannel::s_width);

	for (int i = 0; i < SegChannel::NUM_SEGMENTS; i++)
	{
		setcolor(LIGHTGREY);

		if (segments[i]->net != -1) {
			setcolor((segments[i]->net % 7) + 4);
		}

		float inner_offset = ((i + 1) * SegChannel::s_padding) + (i * SegChannel::s_width);
		drawline(sq_x_offset, inner_offset + sq_y_offset, sq_x_offset + sq_width, inner_offset + sq_y_offset);
		setcolor(BLACK);
		drawtext(sq_x_offset + sq_width / 2, inner_offset + sq_y_offset, std::to_string(this->segments[i]->g_index).c_str(), INT_MAX);
	}
}
