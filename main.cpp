#include <stdio.h>
#include <vector>
#include <fstream>
#include "router.h"
#include "graphics.h"
#include "logic_block.h"
#include "switch.h"

// Callbacks for event-driven window handling.
int main();
void drawscreen(void);
void act_on_button_press(float x, float y);
void act_on_mouse_move(float x, float y);
void act_on_key_press(char c);

// For example showing entering lines and rubber banding
static bool rubber_band_on = false;
static bool have_entered_line, have_rubber_line;
static bool line_entering_demo = false;
static float x_1, y_1, x2, y2;
static int num_new_button_clicks = 0;

static LogicBlock*** blocks;
static std::vector<LogicBlock*> block_list;
static std::vector<SegChannel*> channel_list;
static std::vector<Switch*> switch_list;
static std::vector<vector<int>> seg_adj_list;
static vector<segment*> seg_list;
static int* segment_labels;

struct net_t {
	int lb1_x;
	int lb1_y;
	int lb1_pin;
	int lb2_x;
	int lb2_y;
	int lb2_pin;
};


int main() {

	int l_block_dim;
	int c_width;
	vector<net_t> nets;

	bool minimize_w = false;
	Switch::WILTON = true;

	fstream file("../test-circuits/cc3");

	string line;

	file >> l_block_dim >> c_width;

	while (!file.eof()) {
		int lb1_x;
		int lb1_y;
		int lb1_pin;
		int lb2_x;
		int lb2_y;
		int lb2_pin;

		file >> lb1_x >> lb1_y >> lb1_pin >> lb2_x >> lb2_y >> lb2_pin;

		if (lb1_x == -1) break;

		nets.push_back({ lb1_x - 1, lb1_y - 1, lb1_pin, lb2_x - 1, lb2_y - 1, lb2_pin });
	}


	if (!minimize_w) {
		
		router::init(c_width);
		blocks = router::InitializeObjects(l_block_dim);
		block_list = router::BlockArrayToVector(blocks, l_block_dim);
		channel_list = router::GenerateChannels(blocks, l_block_dim);
		switch_list = router::GenerateSwitches(blocks, l_block_dim);
		seg_adj_list = router::InitializeGraph(seg_list, switch_list);
		segment_labels = router::InitializeSegmentLabels(seg_adj_list.size());



		for (int i = 0; i < nets.size(); i++) {
			net_t net = nets[i];
			router::FindRoute(seg_adj_list, seg_list, segment_labels, blocks[net.lb1_x][net.lb1_y], net.lb1_pin, blocks[net.lb2_x][net.lb2_y], net.lb2_pin, i);
			router::ResetLabels(seg_list, segment_labels);
		}
	}
	else {
		c_width = 0;
		bool success = false;

		while (!success)
		{
			c_width++;
			success = true;
			printf("ATTEMPTING TO ROUTE WIDTH: %d ---------------\n", c_width);

			delete blocks;
			delete segment_labels;
			block_list.clear();
			channel_list.clear();
			switch_list.clear();
			seg_adj_list.clear();
			seg_list.clear();

			router::init(c_width);
			blocks = router::InitializeObjects(l_block_dim);
			block_list = router::BlockArrayToVector(blocks, l_block_dim);
			channel_list = router::GenerateChannels(blocks, l_block_dim);
			switch_list = router::GenerateSwitches(blocks, l_block_dim);
			seg_adj_list = router::InitializeGraph(seg_list, switch_list);
			segment_labels = router::InitializeSegmentLabels(seg_adj_list.size());



			for (int i = 0; i < nets.size(); i++) {
				net_t net = nets[i];
				if (!router::FindRoute(seg_adj_list, seg_list, segment_labels, blocks[net.lb1_x][net.lb1_y], net.lb1_pin, blocks[net.lb2_x][net.lb2_y], net.lb2_pin, i)) {
					success = false;
					break;
				}
				router::ResetLabels(seg_list, segment_labels);
			}

		}
	}

	printf("REPORT -----------------------------------------------\n");
	printf("Min Channel Width Routed: %d\n", c_width);
	printf("Num Segments Used: %d\n", router::NumSegmentsUsed(seg_list));
	
	/* initialize display with WHITE background */

	printf("About to start graphics.\n");
	init_graphics("Some Example Graphics", WHITE);

	/* still picture drawing allows user to zoom, etc. */
	// Set-up coordinates from (xl,ytop) = (0,0) to 
	// (xr,ybot) = (1000,1000)
	init_world(-1 * LogicBlock::block_draw_width, 5000, 5000, -1 * LogicBlock::block_draw_width);
	update_message("Interactive graphics example.");

	// Pass control to the window handling routine.  It will watch for user input
	// and redraw the screen / pan / zoom / etc. the graphics in response to user
	// input or windows being moved around the screen.  This is done by calling the
	// four callbacks below.  mouse movement and key press (keyboard) events aren't 
	// enabled by default, so they aren't on yet.
	event_loop(act_on_button_press, NULL, NULL, drawscreen);

	close_graphics();
	printf("Graphics closed down.\n");

	return (0);
}


void drawscreen(void) {

	/* redrawing routine for still pictures.  Graphics package calls  *
	* this routine to do redrawing after the user changes the window *
	* in any way.                                                    */

	t_point polypts[3] = { { 500.,400. },{ 450.,480. },{ 550.,480. } };
	t_point polypts2[4] = { { 700.,400. },{ 650.,480. },{ 750.,480. },{ 800.,400. } };

	set_draw_mode(DRAW_NORMAL);  // Should set this if your program does any XOR drawing in callbacks.
	clearscreen();  /* Should precede drawing for all drawscreens */

	setfontsize(10);
	setlinestyle(SOLID);
	setlinewidth(2);



	for (auto channel : channel_list) {
		channel->draw();
	}

	for (auto block : block_list) {
		block->draw();
	}

	for (auto sw : switch_list) {
		sw->draw(seg_adj_list);
	}
}



void act_on_button_press(float x, float y) {

	/* Called whenever event_loop gets a button press in the graphics *
	* area.  Allows the user to do whatever he/she wants with button *
	* clicks.                                                        */

	printf("User clicked a button at coordinates (%f, %f)\n", x, y);

	if (line_entering_demo) {
		if (rubber_band_on) {
			rubber_band_on = false;
			x2 = x;
			y2 = y;
			have_entered_line = true;  // both endpoints clicked on --> consider entered.

									   // Redraw screen to show the new line.  Could do incrementally, but this is easier.
			drawscreen();
		}
		else {
			rubber_band_on = true;
			x_1 = x;
			y_1 = y;
			have_entered_line = false;
			have_rubber_line = false;
		}
	}

}



void act_on_mouse_move(float x, float y) {
	// function to handle mouse move event, the current mouse position in the current world coordinate
	// as defined as MAX_X and MAX_Y in init_world is returned

	printf("Mouse move at (%f,%f)\n", x, y);
	if (rubber_band_on) {
		// Go into XOR mode.  Make sure we set the linestyle etc. for xor mode, since it is 
		// stored in different state than normal mode.
		set_draw_mode(DRAW_XOR);
		setlinestyle(SOLID);
		setcolor(WHITE);
		setlinewidth(1);

		if (have_rubber_line) {
			// Erase old line.  
			drawline(x_1, y_1, x2, y2);
		}
		have_rubber_line = true;
		x2 = x;
		y2 = y;
		drawline(x_1, y_1, x2, y2);  // Draw new line
	}
}


void act_on_key_press(char c) {
	// function to handle keyboard press event, the ASCII character is returned
	printf("Key press: %c\n", c);
}
