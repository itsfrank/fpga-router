#include <stdio.h>
#include <vector>

#include "router.h"
#include "graphics.h"
#include "logic_block.h"
#include "switch.h"

// Callbacks for event-driven window handling.
void delay(void);
int main();
void drawscreen(void);
void act_on_new_button_func(void(*drawscreen_ptr) (void));
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

int main() {

	int i;

	int l_block_dim = 3;
	int c_width = 3;

	router::init(c_width);
	blocks = router::InitializeObjects(l_block_dim);
	block_list = router::BlockArrayToVector(blocks, l_block_dim);
	channel_list = router::GenerateChannels(blocks, l_block_dim);
	switch_list = router::GenerateSwitches(blocks, l_block_dim);
	seg_adj_list = router::InitializeGraph(seg_list, switch_list);
	segment_labels = router::InitializeSegmentLabels(seg_adj_list.size());
	router::FindRoute(seg_adj_list, seg_list, segment_labels, blocks[0][0], 1, blocks[2][2], 1, 0);

	/* initialize display with WHITE background */

	printf("About to start graphics.\n");
	init_graphics("Some Example Graphics", WHITE);

	/* still picture drawing allows user to zoom, etc. */
	// Set-up coordinates from (xl,ytop) = (0,0) to 
	// (xr,ybot) = (1000,1000)
	init_world(-1 * LogicBlock::block_draw_width, 1000., 1000., -1 * LogicBlock::block_draw_width);
	update_message("Interactive graphics example.");

	// Pass control to the window handling routine.  It will watch for user input
	// and redraw the screen / pan / zoom / etc. the graphics in response to user
	// input or windows being moved around the screen.  This is done by calling the
	// four callbacks below.  mouse movement and key press (keyboard) events aren't 
	// enabled by default, so they aren't on yet.
	event_loop(act_on_button_press, NULL, NULL, drawscreen);

	/* animation section */
	clearscreen();
	update_message("Non-interactive (animation) graphics example.");
	setcolor(RED);
	setlinewidth(1);
	setlinestyle(DASHED);
	init_world(0., 0., 1000., 1000.);
	for (i = 0; i<50; i++) {
		drawline((float)i, (float)(10.*i), (float)(i + 500.), (float)(10.*i + 10.));
		flushinput();
		delay();
	}

	/* Draw an interactive still picture again.  I'm also creating one new button. */

	init_world(0., 0., 1000., 1000.);
	update_message("Interactive graphics #2. Click in graphics area to rubber band line.");
	create_button("Window", "0 Clicks", act_on_new_button_func);

	// Enable mouse movement (not just button presses) and key board input.
	// The appropriate callbacks will be called by event_loop.
	set_keypress_input(true);
	set_mouse_move_input(true);
	line_entering_demo = true;

	// draw the screen once before calling event loop, so the picture is correct 
	// before we get user input.
	drawscreen();
	event_loop(act_on_button_press, act_on_mouse_move, act_on_key_press, drawscreen);

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


	for (auto block : block_list) {
		block->draw();
	}

	for (auto channel : channel_list) {
		channel->draw();
	}

	for (auto sw : switch_list) {
		sw->draw();
	}
}


void delay(void) {

	/* A simple delay routine for animation. */

	int i, j, k, sum;

	sum = 0;
	for (i = 0; i<100; i++)
		for (j = 0; j<i; j++)
			for (k = 0; k<1000; k++)
				sum = sum + i + j - k;
}


void act_on_new_button_func(void(*drawscreen_ptr) (void)) {

	char old_button_name[200], new_button_name[200];
	printf("You pressed the new button!\n");
	setcolor(MAGENTA);
	setfontsize(12);
	drawtext(500., 500., "You pressed the new button!", 10000.);
	printf(old_button_name, "%d Clicks", num_new_button_clicks);
	num_new_button_clicks++;
	printf(new_button_name, "%d Clicks", num_new_button_clicks);
	change_button_text(old_button_name, new_button_name);
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
