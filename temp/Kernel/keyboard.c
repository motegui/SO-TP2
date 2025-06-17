#include <string.h>
#include <keyboard.h>
#include <videodriver.h>

#define BUFFER_SIZE 512
#define MAX_KEY_PRESSED 127

static char buffer[BUFFER_SIZE] = {0};
static int elem_count = 0;
static int read_index = 0;
static int write_index = 0;

static char shift_pressed = 0;
static char caps_locked = 0;

static const char charHexMap[256] = {       
        0,  1/*esc*/,  '1',  '2',  '3',  '4',  '5',  '6',   '7',  '8',  '9',   '0',   '\'',  '<', '\b',
    '\t', 'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',   'o',  'p',  '\\',   '+', '\n',   
        0,     'a',  's', 'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  '{',
    '|',  5/*shift*/,  '}',  'z',  'x', 'c', 'v',  'b',  'n',  'm',  ',',  '.',  '-',    0,  
    '*',     0,  ' ',    0,     0,     0/*60*/,    0,       0,         0, 
    0,     0,     0,    0,      0,      0,      0,      0,         17/*up*/,
    0,      0,      18/*left*/,    0,     19/*right*/,     0,      0,      20/*down*/,      0,
};
static const char charCapsHexMap[256] = {       
        0,    1,  '!',  '\"',  '#',  '$',  '%',  '&',   '/',  '(',  ')',   '=',   '?',  '>', '\b',
    '\t', 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',   'O',  'P',  '^',   '*', '\n',   
        0,     'A',  'S', 'D',  'F',  'G',  'H',  'J',  'K',  'L',  '~',  '[',
    '/',  5/*shift*/,  ']',  'Z',  'X', 'C', 'V',  'B',  'N',  'M',  ';',  ':',  '_',    0,  
    '*',     0,  ' ',    0,     0,     0/*60*/,    0,       0,         0, 
    0,     0,     0,    0,      0,      0,      0,      0,         17/*up*/,
    0,      0,      18/*left*/,    0,     19/*right*/,     0,      0,      20/*down*/,      0,
};

char is_letter(unsigned char key) {
    char c = charHexMap[key];
    return (c >= 'a' && c <= 'z');
}

void keyboard_handler() {
    unsigned char key = getKey();
    if (key < 83 || key == 0xAA || key == 0x3A) { 
        if (elem_count >= BUFFER_SIZE) return;  // buffer is full
        
        // make the array circular
        if (write_index >= BUFFER_SIZE)
            write_index = 0;

        if (charHexMap[key] == 5 && !shift_pressed) { 
            shift_pressed = 1;
            return;
        }
        if (key == 0xAA) { 
            shift_pressed = 0;
            return;
        }
        if (key == 0x3A) { 
            caps_locked = !caps_locked;
            return;
        }

        buffer[write_index] = !is_letter(key) ? (shift_pressed ? charCapsHexMap[key] : charHexMap[key]): ((shift_pressed && !caps_locked) || (!shift_pressed && caps_locked)) ? charCapsHexMap[key] : charHexMap[key];

        // update iterators
        elem_count++;
        write_index++;
    } else {
        buffer[write_index] = key;
        elem_count++;
        write_index++;
    }
}

char get_char() {
    if (elem_count == 0) { 
        return 0; // buffer is empty
    }

    char toReturn = buffer[read_index];
    
    elem_count--;
    read_index++;

    if (read_index == BUFFER_SIZE) read_index = 0;
    
    return toReturn;
}

char get_char_no_block() {
	if (elem_count == 0)
		return 0;

	return get_char();
}