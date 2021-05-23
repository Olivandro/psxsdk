#include <stdio.h>
#include <psx.h>
#include <psxgpu.h>

// Program variables
/* Screen resolution */
#define SCREEN_XRES		320
#define SCREEN_YRES		240

#define OT_LEN 0x4000 /* 128 kb */

unsigned int prim_list[OT_LEN];
volatile int vblank_happened = 0;

// Current frame buffer indicator
int current_buf = 0;

GsDrawEnv game_drawenv;
GsDispEnv game_dispenv;

GsRectangle game_rect;


// Program functions
/* Dont really understand this function and variable yet... */
static void program_vblank_handler(void) {
	vblank_happened = 1;
}

/* Initialise game environmental variables */ 
void init(void)
{
    /* Initialize PSXSDK Library */
	PSX_InitEx(PSX_INIT_CD);
	/* Initialize the Graphic Synthesizer */
	GsInit();
	/* Clear video memory */
	GsClearMem();

	// Set up drawing environment
	game_drawenv.dither = 0;
	game_drawenv.draw_on_display = 0;
	game_drawenv.x = 0;
	game_drawenv.y = 0;
	game_drawenv.w = SCREEN_XRES;
	game_drawenv.h = SCREEN_YRES;
	game_drawenv.ignore_mask = 0;
	game_drawenv.set_mask = 0;

	GsSetDrawEnv(&game_drawenv);
	
	// Set up display environment
	game_dispenv.x = 0;
	game_dispenv.y = 256;
	
	GsSetDispEnv(&game_dispenv);

    /* Set primitive list pointer */
	GsSetList(prim_list);

    /* Set 320x240 video mode */
	GsSetVideoMode(320, 240, VMODE_NTSC);

	/* Set VBlank Handler */
	SetVBlankHandler( program_vblank_handler );	
}

// MAIN DISPLAY OR DRAW FUNCTION.
// This is where we draw everything
void display(void)
{
    /* Change display and drawing environment settings */
	/* for double buffering */
	if (current_buf) 
	{
		game_drawenv.y = 0;
		GsSetDrawEnv(&game_drawenv);
		game_dispenv.y = 256;	
		GsSetDispEnv(&game_dispenv);
	}
	else 
	{
		game_drawenv.y = 256;
		GsSetDrawEnv(&game_drawenv);
		game_dispenv.y = 0;
		GsSetDispEnv(&game_dispenv);
	}
	current_buf = !current_buf;
	
	
	/* Clear the screen with the specified color */
	GsSortCls(34, 53, 226);

	// Set perameters for window, or rect 1.
	game_rect.x = 10;
	game_rect.y = 20;
	game_rect.w = 300;
	game_rect.h = 200;
	game_rect.r = 0;
	game_rect.g = 0;
	game_rect.b = 0;
	game_rect.attribute = 0;
	
	// Bind parameters to drawing list
	GsSortRectangle(&game_rect);

	// Establish new window or rectangle parameters
	game_rect.x = 20;
	game_rect.y = 40;
	game_rect.w = 280;
	game_rect.h = 160;
	game_rect.r = 255;
	game_rect.g = 255;
	game_rect.b = 255;
	game_rect.attribute = 0;
	
	// Bind parameters to drawing list
	GsSortRectangle(&game_rect);

    /* Draw list */
    GsDrawList();
}

int main(void)
{
    init();

    while (1) 
    {
        display();
    }
    return 0;
}
