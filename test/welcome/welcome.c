#include <stdio.h>
#include <string.h>
#include <psx.h>
#include <psxgpu.h>


/* Screen resolution */
#define SCREEN_XRES		320
#define SCREEN_YRES		240

#define OT_LEN 0x4000

unsigned int prim_list[OT_LEN];
volatile int vblank_happened = 0;

// Current frame buffer indicator
int current_buf = 0;
// Generic string buffer for game image sprite
char string_buf[256];

// Buffer for the data file loading
unsigned char file_buffer[0x30000]; /* 192 kilobytes */

unsigned short game_clut[16];

// Game image buffer
GsImage game_image;

GsDrawEnv game_drawenv;
GsDispEnv game_dispenv;


static void program_vblank_handler(void) {
	vblank_happened = 1;
}

static int load_file_into_buffer(char *fname)
{
	FILE *f;
	int sz;

	f = fopen(fname, "rb");
	
	//f->pos = 0;
	fseek(f, 0, SEEK_END);	

	sz = ftell(f);

	fseek(f, 0, SEEK_SET);

	printf("%s (%d)\n", fname, sz);
	
	fread(file_buffer, sizeof(char), sz, f);
	
	fclose(f);
	
	return sz;
}

void init(void)
{
    /* Initialize PSXSDK Library */
	PSX_InitEx(PSX_INIT_CD);
	/* Initialize the Graphic Synthesizer */
	GsInit();

	// /* setup auto wait for drawing function to finish */
	// GsSetAutoWait();
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

	/* Load in font data files */
	load_file_into_buffer("cdrom:FONT.TIM;1");
	GsImageFromTim(&game_image, file_buffer);
	
	GsUploadImage(&game_image);
	
	game_clut[0] = 0x0;
	game_clut[1] = 0x7fff;
	
	LoadImage(game_clut, 640, 24, 16, 1);

    /* Set 320x240 video mode */
	GsSetVideoMode(320, 240, VMODE_NTSC);

	/* Set VBlank Handler */
	SetVBlankHandler( program_vblank_handler );	
}

// This only works in conjunctions with sprintf()
// 
static void game_print(char *string, int x, int y)
{
	GsSprite print_char;
	char q;
	
	print_char.x = x;
	print_char.y = y;
	print_char.w = 8;
	print_char.h = 8;
	print_char.r = NORMAL_LUMINOSITY;
	print_char.g = NORMAL_LUMINOSITY;
	print_char.b = NORMAL_LUMINOSITY;
	print_char.cx = 640;
	print_char.cy = 24;
	print_char.tpage = 10;
	print_char.attribute = COLORMODE(COLORMODE_4BPP);

	while(*string)
	{
		if(*string >= 0x20 && *string <= 0x7F)
		{
			q = *string;
			q -= 0x20;
			
			print_char.u = (q&0x1f)<<3;
			print_char.v = (q/0x20)<<3;
			GsSortSimpleSprite(&print_char);
		}
		print_char.x+=8;
		string++;
	}
}

// MAIN DISPLAY OR DRAW FUNCTION.
// We create a double buffer, then change the buffer indicator to switch the buffers
// on the next while pass. 
// From there when colour the sceen the background colour we want, purple in this case.
// Then store a string in the string_buf, which is then passed to the function that finally 
// prints the text as sprites. 
// I wonder if sprites are the best way to render text?
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
	GsSortCls(63, 0, 127);

	// We assign the string buffer to our text message.
	sprintf(string_buf, "Hello PSX World!");
	game_print(string_buf, 160, 120);


    /* Draw list */
    GsDrawList();
}


// Main function.
int main(void)
{

    init();

    while (1) {
        display();
    }
    
    return 0;
}

