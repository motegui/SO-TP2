#include <videodriver.h>
#include <colors.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_EXCEPTION_ID 6
#define REGS_AMOUNT 18

extern const uint64_t excepRegs[REGS_AMOUNT];

static void zero_division();

static void invalid_opcode();

void printRegisters();

void exceptionDispatcher(int exception) {
	if (exception == ZERO_EXCEPTION_ID)
		zero_division();
    else if (exception == INVALID_OPCODE_EXCEPTION_ID)
        invalid_opcode();
}

static void zero_division() {
	// Handler para manejar excepcion
	printStringColor("\n\nCannot divide by zero.\n\n", RED);
	printRegisters();
}

static void invalid_opcode() {
    printStringColor("\n\nInvalid opcode.\n\n", RED);
    printRegisters();
}

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

void printRegisters() {
    char * regsnames[] = {"RIP ", "RAX ", "RBX ", "RCX ", "RDX ", "RSI ", "RDI ", "RBP ", "RSP ", "R8  ", "R9  ", "R10 ", "R11 ", "R12 ",
        "R13 ", "R14 ", "R15 ", "RFLAGS "};
	printString("\n");
    for (int i = 0; i < REGS_AMOUNT; i++) {
        char hex[17];
        intToHex(excepRegs[i], hex);
        printStringColor(regsnames[i], RED);
        printStringColor(hex, RED);
        printString("\n");
    }
	printString("\n");
}