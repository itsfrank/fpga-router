#include "router.h"
#include "router.h"

using namespace std;

void router::init(int channel_width)
{
	SegChannel::NUM_SEGMENTS = channel_width;
	SegChannel::s_padding = SegChannel::min_padding;
	
	LogicBlock::block_draw_width = (SegChannel::s_padding * (SegChannel::NUM_SEGMENTS + 1)) + (SegChannel::s_width * SegChannel::NUM_SEGMENTS);

	if (LogicBlock::block_draw_width < LogicBlock::MIN_DRAW_WIDTH) {
		LogicBlock::block_draw_width = LogicBlock::MIN_DRAW_WIDTH;
	}
	float remaining = LogicBlock::block_draw_width - (channel_width * SegChannel::s_width);
	SegChannel::s_padding = remaining / (channel_width + 1);
}

LogicBlock *** router::InitializeObjects(int num_logic_blocks)
{
	LogicBlock*** blocks = new LogicBlock**[num_logic_blocks];

	for (int i = 0; i < num_logic_blocks; i++)
	{
		
		blocks[i] = new LogicBlock*[num_logic_blocks];

		for (int j = 0; j < num_logic_blocks; j++)
		{
			blocks[i][j] = new LogicBlock(i, j);
		}
	}

	return blocks;
}

std::vector<LogicBlock*> router::BlockArrayToVector(LogicBlock *** blocks, int dimension_size)
{
	
	vector<LogicBlock*> blocks_vector;

	for (int i = 0; i < dimension_size; i++)
	{
		for (int j = 0; j < dimension_size; j++)
		{
			blocks_vector.push_back(blocks[i][j]);
		}
	}
	
	return blocks_vector;
}

vector<SegChannel*> router::GenerateChannels(LogicBlock *** blocks, int dimension_size)
{

	vector<SegChannel*> channels;

	//Horizontal Channels
	for (int i = 0; i < dimension_size; i++)
	{
		for (int j = 0; j < dimension_size + 1; j++)
		{
			SegChannel* channel;
			if (j == 0) {
				channel = new SegChannel(NULL, blocks[i][j], false);
				blocks[i][j]->addChannel(LogicBlock::DOWN, channel);
			}
			else if (j == dimension_size) {
				channel = new SegChannel(blocks[i][j - 1], NULL, false);
				blocks[i][j - 1]->addChannel(LogicBlock::UP, channel);
			}
			else {
				channel = new SegChannel(blocks[i][j - 1], blocks[i][j], false);
				blocks[i][j]->addChannel(LogicBlock::DOWN, channel);
				blocks[i][j - 1]->addChannel(LogicBlock::UP, channel);
			}
			channels.push_back(channel);
		}
	}

	//Vertical Channels
	for (int i = 0; i < dimension_size + 1; i++)
	{
		for (int j = 0; j < dimension_size; j++)
		{
			SegChannel* channel;
			if (i == 0) {
				channel = new SegChannel(NULL, blocks[i][j], true);
				blocks[i][j]->addChannel(LogicBlock::LEFT, channel);
			}
			else if (i == dimension_size) {
				channel = new SegChannel(blocks[i - 1][j], NULL, true);
				blocks[i - 1][j]->addChannel(LogicBlock::RIGHT, channel);
			}
			else {
				channel = new SegChannel(blocks[i - 1][j], blocks[i][j], true);
				blocks[i][j]->addChannel(LogicBlock::LEFT, channel);
				blocks[i - 1][j]->addChannel(LogicBlock::RIGHT, channel);
			}
			channels.push_back(channel);
		}
	}

	return channels;
}

vector<Switch*> router::GenerateSwitches(LogicBlock *** blocks, int dimension_size)
{
	vector<Switch*> switches;
	
	for (int i = 0; i < dimension_size + 1; i++)
	{
		for (int j = 0; j < dimension_size + 1; j++)
		{
			LogicBlock* bottom_left_lb = (i == 0 || j == 0) ? NULL : blocks[i - 1][j - 1];
			LogicBlock* top_left_lb = (i == 0 || j == dimension_size) ? NULL : blocks[i - 1][j];
			LogicBlock* bottom_right_lb = (i == dimension_size || j == 0) ? NULL : blocks[i][j - 1];
			LogicBlock* top_right_lb = (i == dimension_size || j == dimension_size) ? NULL : blocks[i][j];

			SegChannel* top_ch = (top_right_lb) ? top_right_lb->getLeftCh() : ((top_left_lb) ? top_left_lb->getRightCh() : NULL);
			SegChannel* right_ch = (top_right_lb) ? top_right_lb->getBottomCh() : ((bottom_right_lb) ? bottom_right_lb->getTopCh() : NULL);
			SegChannel* bottom_ch = (bottom_left_lb) ? bottom_left_lb->getRightCh() : ((bottom_right_lb) ? bottom_right_lb->getLeftCh() : NULL);
			SegChannel* left_ch = (top_left_lb) ? top_left_lb->getBottomCh() : ((bottom_left_lb) ? bottom_left_lb->getTopCh() : NULL);

			Switch* sw = new Switch(i, j, top_ch, right_ch, bottom_ch, left_ch);

			switches.push_back(sw);
		}
	}
	
	return switches;
}

vector<vector<int>> router::InitializeGraph(vector<segment*> &segment_list, vector<Switch*> switches)
{

	int seg_count = 0;
	vector<vector<int>> adjacency_vector;

	for (auto sw : switches) {
		vector<segment*> segments = sw->getSwitchSegments();

		//Initialize all segments that require it
		for (size_t i = 0; i < segments.size(); i++)
		{
			segment* seg = segments.at(i);

			vector<int> adjacency_list;

			if (seg->g_index == -1) {
				seg->g_index = seg_count;
				segment_list.push_back(seg);
				adjacency_vector.push_back(adjacency_list);
				seg_count++;
				if (seg_count != segment_list.size()) throw exception("seg_vector and seg_count index do not match");
			}
		}


		for (size_t i = 0; i < segments.size(); i++)
		{
			segment* seg_this = segments[i];
			for (size_t j = i + 1; j < segments.size(); j++)
			{
				segment* seg_other = segments[j];
				if (seg_this->parent == seg_other->parent) continue;

				adjacency_vector[seg_this->g_index].push_back(seg_other->g_index);
				adjacency_vector[seg_other->g_index].push_back(seg_this->g_index);
			}
		}
	}
	return adjacency_vector;
}

int * router::InitializeSegmentLabels(int num_segments)
{
	int* segment_labels = new int[num_segments];

	for (size_t i = 0; i < num_segments; i++)
	{
		segment_labels[i] = router::AVAILABLE;
	}

	return segment_labels;
}

void router::FindRoute(vector<vector<int>> seg_adj_list, vector<segment*> seg_list, int * segment_labels, LogicBlock* start_block, int start_pin, LogicBlock* end_block, int end_pin, int net_id)
{

	SegChannel* start_channel = start_block->channels[start_pin - 1];
	SegChannel* end_channel = end_block->channels[end_pin - 1];

	queue<int> expansion_list;
	vector<int> target_ids;

	for (auto seg : start_block->channels[start_pin - 1]->segments) {
		if (segment_labels[seg->g_index] == router::UNAVAILABLE) continue;
		segment_labels[seg->g_index] = 0;
		expansion_list.push(seg->g_index);
	}

	for (auto seg : end_block->channels[end_pin - 1]->segments) {
		segment_labels[seg->g_index] = router::TARGET;
		target_ids.push_back(seg->g_index);
	}

	bool target_found = false;
	int target_id;

	while (!expansion_list.empty() && !target_found)
	{
		int current = expansion_list.front();
		int current_value = segment_labels[current];
		expansion_list.pop();
		for (auto neighbour : seg_adj_list[current]) {
			int n_value = segment_labels[neighbour];

			if (n_value == router::UNAVAILABLE) continue;
			if (n_value == router::TARGET) {
				target_id = neighbour;
				target_found = true;
				break;
			}

			if (n_value > current_value) {
				segment_labels[neighbour] = current_value + 1;
				expansion_list.push(neighbour);
			}

		}
	}

	int current_id = target_id;

	if (target_found) {
		while (segment_labels[current_id] > 0) {
			segment* seg = seg_list[current_id];
			seg->net = net_id;

			for (auto neighbour : seg_adj_list[current_id]) {
				if (segment_labels[neighbour] < segment_labels[current_id]) {
					seg_list[current_id]->net = net_id;
					current_id = neighbour;
					break;
				}
			}
		}
		seg_list[current_id]->net = net_id;
	}

}
