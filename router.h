#pragma once

#include <climits>
#include <vector>
#include <queue>

#include "logic_block.h"
#include "seg_channel.h"
#include "switch.h"

class LogicBlock;
class SegChannel;
class Switch;
struct segment;

#define FULLY_CONNECTED
//#define WILTON

namespace router {
	const int UNAVAILABLE = INT_MAX;
	const int TARGET = INT_MAX - 1;
	const int AVAILABLE = INT_MAX - 2;

	using namespace std;

	void init(int channel_width);
	LogicBlock*** InitializeObjects(int num_logic_blocks);
	std::vector<LogicBlock*> BlockArrayToVector(LogicBlock*** blocks, int dimension_size);
	vector<SegChannel*> GenerateChannels(LogicBlock*** blocks, int dimension_size);
	vector<Switch*> GenerateSwitches(LogicBlock*** blocks, int dimension_size);
	vector<vector<int>> InitializeGraph(vector<segment*> &segment_list, vector<Switch*> switches);
	int* InitializeSegmentLabels(int num_segments);

	bool FindRoute(vector<vector<int>> seg_adj_list, vector<segment*> seg_list, int* segment_labels, LogicBlock* start_block, int start_pin, LogicBlock* end_block, int end_pin, int net_id);
	void ResetLabels(vector<segment*> seg_list, int* segment_labels);
};

