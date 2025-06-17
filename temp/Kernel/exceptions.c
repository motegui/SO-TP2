#include <videodriver.h>
#include <colors.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_EXCEPTION_ID 6
#define REGS_AMOUNT 18

extern const uint64_t excepRegs[REGS_AMOUNT];

static void zero_division();

static void invalid_opcode();

void print_registers();

void exceptionDispatcher(int exception) {
	if (exception == ZERO_EXCEPTION_ID)
		zero_division();
    else if (exception == INVALID_OPCODE_EXCEPTION_ID)
        invalid_opcode();
}

static void zero_division() {
	// Handler para manejar excepcion
	print_string_color("\n\nCannot divide by zero.\n\n", RED);
	print_registers();
}

static void invalid_opcode() {
    print_string_color("\n\nInvalid opcode.\n\n", RED);
    print_registers();
}

void int_to_hex(uint64_t num, char* hex) {
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

void print_registers() {
    char * regsnames[] = {"RIP ", "RAX ", "RBX ", "RCX ", "RDX ", "RSI ", "RDI ", "RBP ", "RSP ", "R8  ", "R9  ", "R10 ", "R11 ", "R12 ",
        "R13 ", "R14 ", "R15 ", "RFLAGS "};
	print_string("\n");
    for (int i = 0; i < REGS_AMOUNT; i++) {
        char hex[17];
        int_to_hex(excepRegs[i], hex);
        print_string_color(regsnames[i], RED);
        print_string_color(hex, RED);
        print_string("\n");
    }
	print_string("\n");
}