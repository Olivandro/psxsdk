#include <stdio.h>
#include <psx.h>
#include <psxgpu.h>
#include <meidogte.h>

// Program variables
/* Screen resolution */
#define SCREEN_XRES		320
#define SCREEN_YRES		240

/* Screen center position */
#define CENTERX			SCREEN_XRES>>1
#define CENTERY			SCREEN_YRES>>1

#define OT_LEN 0x4000 /* 128 kb */

unsigned int prim_list[OT_LEN];
volatile int vblank_happened = 0;

// Current frame buffer indicator
int current_buf = 0;

GsDrawEnv game_drawenv;
GsDispEnv game_dispenv;

/* Number of faces of cube */
#define SQUARES 1

GsPoly4 poly[SQUARES];

int screenXOffset = CENTERX;
int screenYOffset = CENTERY;


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

	// /* Initialize the MeidoGTE library */
	// InitGeom();

	/* Set VBlank Handler */
	SetVBlankHandler( program_vblank_handler );	
}

// MAIN DISPLAY OR DRAW FUNCTION.
// This is where we draw everything
void display(void)
{
	int i;
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
	// This is quite important to do before drawing. There seems to be a glitch 
	// if the background isn't cleared and fulled. 
	GsSortCls(63, 0, 127);

	// Draw parameters
	// ...
	for( i=0; i<SQUARES; i++ ) 
	{
		
		poly[i].x[0] = 200;
		poly[i].y[0] = 50;

		poly[i].x[1] = 100;
		poly[i].y[1] = 50;

		poly[i].x[2] = 200;
		poly[i].y[2] = 150;

		poly[i].x[3] = 100;
		poly[i].y[3] = 150;

		poly[i].r = 63;
		poly[i].g = 63;
		poly[i].b = 63;
		poly[i].attribute = 0;
	}

	GsSortPoly4(&poly[0]);

    /* Draw list */
    GsDrawList();
}

int main(void)
{

    init();

    while (1) 
    {
		// /* Set GTE offset (recommended method  of centering) */
		// gte_SetGeomOffset( screenXOffset, screenYOffset );

		/* Draw function */
        display();

		// /* Wait for VBlank to happen */
		// vblank_happened = 0;
		// while(!vblank_happened);
    }
    return 0;
}
