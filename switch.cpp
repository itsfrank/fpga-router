#include "switch.h"

bool Switch::WILTON = false;

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

static float CalcConnX(float block_offset, float seg_offset, int ch_dir) {
	if (ch_dir == Switch::UP || ch_dir == Switch::DOWN) {
		return block_offset + seg_offset;
	}
	else if (ch_dir == Switch::LEFT) {
		return block_offset;
	}
	else {
		return block_offset + LogicBlock::block_draw_width;
	}
}

static float CalcConnY(float block_offset, float seg_offset, int ch_dir) {
	if (ch_dir == Switch::LEFT || ch_dir == Switch::RIGHT) {
		return block_offset + seg_offset;
	}
	else if (ch_dir == Switch::DOWN) {
		return block_offset;
	}
	else {
		return block_offset + LogicBlock::block_draw_width;
	}
}

static void DrawConnections(SegChannel* ch1, SegChannel* ch2, float x_offset, float y_offset, int ch1_dir, int ch2_dir, vector<vector<int>> seg_adj_list) {
	for (int i = 0; i < ch1->segments.size(); i++) {
		segment* seg = ch1->segments[i];
		if (seg->net == -1) continue;

		for (int j = 0; j < ch2->segments.size(); j++) {
			segment* other = ch2->segments[j];

			bool connected = false;
			for (auto neighbour : seg_adj_list[seg->g_index]) {
				if (neighbour == other->g_index) connected = true;
			}
			if (!connected) continue;

			if (other->net == seg->net) {
				setlinestyle(SOLID);
				setlinewidth(SegChannel::s_width);
				setcolor((seg->net % 7) + 4);

				float seg_offset1 = ch1->getSegmentOffset(i);
				float seg_offset2 = ch2->getSegmentOffset(j);

				drawline(
					CalcConnX(x_offset, seg_offset1, ch1_dir), CalcConnY(y_offset, seg_offset1, ch1_dir),
					CalcConnX(x_offset, seg_offset2, ch2_dir), CalcConnY(y_offset, seg_offset2, ch2_dir)
				);

			}
		}
	}
}



void Switch::draw(vector<vector<int>> seg_adj_list)
{
	setcolor(LIGHTGREY);
	setlinestyle(DASHED);
	
	float x_offset = (this->x_index * 2 * LogicBlock::block_draw_width) - LogicBlock::block_draw_width;
	float y_offset = (this->y_index * 2 * LogicBlock::block_draw_width) - LogicBlock::block_draw_width;

	
	drawrect(x_offset, y_offset, x_offset + LogicBlock::block_draw_width, y_offset + LogicBlock::block_draw_width);

	if (this->channels[UP] && this->channels[RIGHT]) DrawConnections(this->channels[UP], this->channels[RIGHT], x_offset, y_offset, UP, RIGHT, seg_adj_list);
	if (this->channels[UP] && this->channels[DOWN]) DrawConnections(this->channels[UP], this->channels[DOWN], x_offset, y_offset, UP, DOWN, seg_adj_list);
	if (this->channels[UP] && this->channels[LEFT]) DrawConnections(this->channels[UP], this->channels[LEFT], x_offset, y_offset, UP, LEFT, seg_adj_list);
	if (this->channels[RIGHT] && this->channels[DOWN]) DrawConnections(this->channels[RIGHT], this->channels[DOWN], x_offset, y_offset, RIGHT, DOWN, seg_adj_list);
	if (this->channels[RIGHT] && this->channels[LEFT]) DrawConnections(this->channels[RIGHT], this->channels[LEFT], x_offset, y_offset, RIGHT, LEFT, seg_adj_list);
	if (this->channels[DOWN] && this->channels[LEFT]) DrawConnections(this->channels[DOWN], this->channels[LEFT], x_offset, y_offset, DOWN, LEFT, seg_adj_list);

}


Switch::~Switch()
{
}
