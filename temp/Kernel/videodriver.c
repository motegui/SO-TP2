
#include <stdint.h>
#include <font.h>
#include <videodriver.h>
#include <lib.h>

struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));


typedef struct vbe_mode_info_structure * VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

#define MAX_LINES VBE_mode_info->height / CHAR_HEIGHT
#define MAX_COLUMNS VBE_mode_info->width / CHAR_WIDTH - 1

void putPixel(char r, char g, char b, int x, int y) {
    char * videoPtr = (char *) ((uint64_t)VBE_mode_info->framebuffer);
    int offset = y * VBE_mode_info->pitch + x * (VBE_mode_info->bpp / 8);
    videoPtr[offset] = b;
    videoPtr[offset+1] = g;
    videoPtr[offset+2] = r;
}

void drawWhiteLine() {
    for(int i = 0; i < VBE_mode_info->width * (VBE_mode_info->bpp / 8); i++) {
        putPixel(0xff, 0xff, 0xff, i, 3);
        putPixel(0xff, 0xff, 0xff, i, 4);
        putPixel(0xff, 0xff, 0xff, i, 5);
    }
}

void drawRect(int x, int y, int width, int height, int color) {
	for (int i = y; i < y + height; i++) {
		for (int j = x; j < x + width; j++) {
			putPixel(color >> 16, color >> 8, color, j, i);
		}
	}
}

int line = 1, column = 0;
int showCursor = 1;

char getPixel(int x, int y) {
	char * videoPtr = (char *) ((uint64_t)VBE_mode_info->framebuffer);
	int offset = y * VBE_mode_info->pitch + x * (VBE_mode_info->bpp / 8);
	return videoPtr[offset];
}

char isSpaceEmpty(int x, int y) {
	for (int i = y; i < y + CHAR_HEIGHT; i++) {
		for (int j = x; j < x + CHAR_WIDTH; j++) {
			if (getPixel(j, i) != 0) {
				return 0;
			}
		}
	}
	return 1;
}

void printChar(char c, int x, int y, Color color) {
	if (c == '\b') {
		if (x <= 0 && y <= 0)
			return;
		if (x > CHAR_WIDTH) {
			column -= 2;
			for (int i = y; i < y + CHAR_HEIGHT; i++) {
				for (int j = x - CHAR_WIDTH; j < x; j++) {
					putPixel(0, 0, 0, j, i);
				}
			}
		} else if (line > 0) {
			line--;
			column = MAX_COLUMNS - 2;
			int c = 0;
			for (int i = line * CHAR_HEIGHT; i < (line + 1) * CHAR_HEIGHT; i++) {
				for (int j = (column + 1) * CHAR_WIDTH; j < (MAX_COLUMNS + 2) * CHAR_WIDTH; j++) {
					putPixel(0, 0, 0, j, i);
				}
			}
			while (isSpaceEmpty(column * CHAR_WIDTH, line * CHAR_HEIGHT) && column >= 1) {
				column--;
				c++;
			}
		}
		return;
	} else if (c == '\t') {
		if (column + 4 < MAX_COLUMNS) {
			column += 4;
		} else {
			line++;
			column = 0;
		}
		return;
	}

	if (c < FIRST_CHAR || c > LAST_CHAR )
		return;

	const unsigned char * charMap = font[c-32];
	for (int i = 0; i < CHAR_HEIGHT; i++) {
		char mask = 0b1000000;
		for (int j = 0; j < CHAR_WIDTH; j++) {
			if (*charMap & mask)
				putPixel(color.r, color.g, color.b, x+j, y+i);
			mask >>= 1;
		}
		charMap++;
	}
}

/* void printStr(char * string, int x, int y) {
	int line = 0;
	int i = 0, j = 0;
	while (string[i] != 0) {
		printChar(string[i], x + j * CHAR_WIDTH, y + line * CHAR_HEIGHT);
		i++; j++;
		if (x + j * CHAR_WIDTH > VBE_mode_info->width - 9) {
			line++;
			j = 0;
		}
	}
} */

unsigned int strlen(char * str) {
    unsigned int i = 0;
    while (str[i] != 0) {
        i++;
    }
    return i;
}

void printStringPlace(char * string, int x, int y, Color color) {
	int i = 0;
	int oldColumn = column;
	int oldLine = line;
	column = x / CHAR_WIDTH;
	line = y / CHAR_HEIGHT;
	while (string[i] != 0) {
		printChar(string[i], x + i * CHAR_WIDTH, y, color);
		i++;
	}
	column = oldColumn;
	line = oldLine;
}


void printString(char * string) {
	printStringN(string, strlen(string));
}

void printStringN(char * string, uint64_t length) {
	printStringNColor(string, length, WHITE);
}

void printStringColor(char * string, Color color) {
	printStringNColor(string, strlen(string), color);
}

void printStringNColor(char * string, uint64_t length, Color color) {
	int i = 0;
	eraseCursor();
	while (string[i] != 0 && length > 0) {
		if (string[i] == '\n') {
			line++;
			column = 0;
		} else {
			column++;
			printChar(string[i], column * CHAR_WIDTH, line * CHAR_HEIGHT, color);
			if (column >= MAX_COLUMNS - 1) {
				line++;
				column = 0;
			}
		}
		if (line >= MAX_LINES) {
			moveOneLineUp();
		}
		i++;
		length--;
	}
	moveCursor();
}


void printLn(char * string) {
	printString(string);
	line++;
	column = 0;
	if (line >= MAX_LINES) {
		moveOneLineUp();
	}
	moveCursor();
}

void moveOneLineUp() {
	char * dst = (char *) (uint64_t)(VBE_mode_info->framebuffer);
	char * src = dst + VBE_mode_info->pitch * CHAR_HEIGHT;
	memcpy(dst, src, VBE_mode_info->pitch * (VBE_mode_info->height - CHAR_HEIGHT));
	memset((void *) (uint64_t)(VBE_mode_info->framebuffer + VBE_mode_info->pitch * (VBE_mode_info->height - CHAR_HEIGHT)), 0, VBE_mode_info->pitch * CHAR_HEIGHT);
	line--;
}

void moveCursor() {
	if (showCursor) {
		for (int i = line * CHAR_HEIGHT; i < (line + 1) * CHAR_HEIGHT; i++) {
			for (int j = (column + 1) * CHAR_WIDTH; j < (column + 2) * CHAR_WIDTH; j++) {
				putPixel(0xff, 0xff, 0xff, j, i);
			}
		}
	}
}

void eraseCursor() {
	if (showCursor) {
		for (int i = line * CHAR_HEIGHT; i < (line + 1) * CHAR_HEIGHT; i++) {
			for (int j = (column + 1) * CHAR_WIDTH; j < (column + 2) * CHAR_WIDTH; j++) {
				putPixel(0, 0, 0, j, i);
			}
		}
	}
}

void clearScreen() {
	memset((void *) (uint64_t)(VBE_mode_info->framebuffer), 0, VBE_mode_info->pitch * VBE_mode_info->height);
	line = 1;
	column = 0;
	moveCursor();
}

uint16_t getHeight() {
	return VBE_mode_info->height;
}

uint16_t getWidth() {
	return VBE_mode_info->width;
}

void toggleCursor() {
	if (showCursor)
		eraseCursor();
	showCursor = !showCursor;
}

void drawImage(const unsigned long int * image, int width, int height) {
	for (int i = 0, o = 100; i < height; i++, o += 5) {
		for (int j = 0, k = 230; j < width; j++, k += 5) {
			drawRect(k, o, 5, 5, image[i * width + j]);
		}
	}
}