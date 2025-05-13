#include <stdint.h>

void strcpy(char * dest, char * src) {
	int i = 0;
	while (src[i] != 0) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;
}

int strncmp(char * str1, char * str2, int length) {
	int i = 0;
	while (i < length && str1[i] != 0 && str2[i] != 0 && str1[i] == str2[i]) {
		i++;
	}
	return i == length;
}

uint64_t atoi(char * str) {
    uint64_t num = 0;
    int i = 0;
    while (str[i] && (str[i] >= '0' && str[i] <= '9')) {
        num = num * 10 + (str[i] - '0');
        i++;
    }
    return num;
}

int strtoi(char * buffer, int * i) {
	char str[18];
	int size = 0;
	while(buffer[*i] != ' ' && buffer[*i] != '\n' && buffer[*i] != 0) {
		str[size++] = buffer[*i];
		(*i)++;
	}
	uint64_t out = atoi(str);
	return out;
}

void intToStr(int num, char* str) {
    int i = 0;
    if (num < 0) {
        str[i] = '-';
        i++;
        num = -num;
    }
    int aux = num;
    while (aux > 0) {
        aux /= 10;
        i++;
    }
    str[i] = 0;
    i--;
    while (num > 0) {
        str[i] = num % 10 + '0';
        num /= 10;
        i--;
    }
}

// Convert 64 bit integer to hex string
void intToHex(uint64_t num, char* hex) {
    int i = 0;
    for (i = 15; i >= 0; i--) {
        int aux = num & 0xF;
        if (aux < 10) {
            hex[i] = aux + '0';
        } else {
            hex[i] = aux - 10 + 'A';
        }
        num >>= 4;
    }
    hex[16] = 0;
}

unsigned int strlen(char* str) {
    unsigned int i = 0;
    while (str[i] != 0) {
        i++;
    }
    return i;
}