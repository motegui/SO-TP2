#include <usyscalls.h>
#include <userio.h>
#include <stdarg.h>
#include <colors.h>
#include <uStrings.h>

extern void fillRegs();

void print(char* string) {
    sys_write(1, string, strlen(string));
}

void printColor(char* string, uint64_t color) {
    sys_write_color(1, string, strlen(string), color);
}

void printChar(char c) {
    char str[] = {c};
    sys_write(1, str, 1);
}

void printColorChar(char c, uint64_t color) {
    char str[] = {c};
    sys_write_color(1, str, 1, color);
}

char getChar() {
    char c[] = {0};
    sys_read(0, c, 1);
    return c[0];
}

void putChar(char c) {
    sys_write(1, &c, 1);
}

void printRegs() {
    uint64_t regs[17];
    char * regsnames[] = {"RAX ", "RBX ", "RCX ", "RDX ", "RSI ", "RDI ", "RBP ", "RSP ", "R8  ", "R9  ", "R10 ", "R11 ", "R12 ",
        "R13 ", "R14 ", "R15 ", "RIP "};
    print("\n\n");
    sys_get_regs(regs);
    for (int i = 0; i < 17; i++) {
        char hex[17];
        intToHex(regs[i], hex);
        printf("%s 0x%s\n", regsnames[i], hex);
    }
}

void printfColor(char* format, uint64_t color, ...) {
    va_list args;
    va_start(args, color);
    int i = 0;
    while (format[i] != 0) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 'd': {
                    int num = va_arg(args, int);
                    char str[11];
                    intToStr(num, str);
                    printColor(str, color);
                    break;
                }
                case 'x': {
                    int num = va_arg(args, int);
                    char str[9];
                    intToHex(num, str);
                    printColor(str, color);
                    break;
                }
                case 's': {
                    char* str = va_arg(args, char*);
                    printColor(str, color);
                    break;
                }
                case 'c': {
                    char c = va_arg(args, int);
                    printColorChar(c, color);
                    break;
                }
                default:
                    printColorChar(format[i], color);
                    break;
            }
        } else {
            printColorChar(format[i], color);
        }
        i++;
    }
    va_end(args);
}

void printf(char* format, ...) {
    va_list args;
    va_start(args, format);
    int i = 0;
    while (format[i] != 0) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 'd': {
                    int num = va_arg(args, int);
                    char str[11];
                    intToStr(num, str);
                    printColor(str, WHITE);
                    break;
                }
                case 'x': {
                    int num = va_arg(args, int);
                    char str[9];
                    intToHex(num, str);
                    printColor(str, WHITE);
                    break;
                }
                case 's': {
                    char* str = va_arg(args, char*);
                    printColor(str, WHITE);
                    break;
                }
                case 'c': {
                    char c = va_arg(args, int);
                    printColorChar(c, WHITE);
                    break;
                }
                default:
                    printColorChar(format[i], WHITE);
                    break;
            }
        } else {
            printColorChar(format[i], WHITE);
        }
        i++;
    }
    va_end(args);
}

void scanf(char * format,...) {
	char buffer[1024];
	if(sys_read(STDIN, buffer, 1024) == -1) {
		return;
	}

	if(*format != '%') {
		sys_write(STDERR, "wrong use for scanf\n", 20);
		return;
	}

	va_list vl;
	va_start(vl, format);
	int buffIdx = 0;
	while (*format != 0) {
		if(*format != '%') {
			if (*format != ' ') {
		        sys_write(STDERR, "wrong use for scanf\n", 20);
				return;
			} else {
				(*format)++;
			}
		}
		else {
			(*format)++;
			switch (*format) {
            	case 'd':
				case 'D':
					*(int *)va_arg( vl, int* ) = strtoi(buffer, &buffIdx);	
                	break;
            	case 'c':
				case 'C':
					*(char *)va_arg( vl, char* ) = buffer[buffIdx++];
                	break;
				case 's':
                case 'S':
                    strcpy((char *)va_arg( vl, char* ), buffer + buffIdx);
                    buffIdx += strlen(buffer + buffIdx) + 1;
                    break;
                default:
                    sys_write(STDERR, "wrong use for scanf\n", 20);
                    return;
			}
			(*format)++;	
		}
	}
	va_end(vl);
}

void fillRegisters() {
	printColor("\n\nFilling registers...\n", YELLOW);
	printColor("Press CTRL to save them.\n", CYAN);
	fillRegs();
}

char * getTime() {
	static char bufferTime[9];
	sys_get_time(bufferTime);
	return bufferTime;
}

char * getDate() {
	static char bufferDate[9];
	sys_get_date(bufferDate);
	return bufferDate;
}
