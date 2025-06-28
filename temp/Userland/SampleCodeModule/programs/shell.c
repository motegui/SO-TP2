#include <colors.h>
#include <userio.h>
#include <usyscalls.h>
#include <colors.h>
#include <sounds.h>
#include <boca.h>
#include <pong.h>
#include <shell.h>
#include <tests.h>
#include <stddef.h>
#include <programs.h>
#include <date_time.h>
#include <process_userland.h>

#define COMMANDS_QUANTITY 22
char * test_mm_args[3];

extern int loop_a_main(int argc, char **argv);

int sscanf(const char *str, const char *format, int *a, int *b) {
    if (format[0] == '%' && format[1] == 'd' && format[2] == ' ' && format[3] == '%' && format[4] == 'd') {
        int sign = 1, val = 0, count = 0;
        //primer numero
        while (*str == ' ' || *str == '\t') str++;
        if (*str == '-') { sign = -1; str++; }
        while (*str >= '0' && *str <= '9') {
            val = val * 10 + (*str - '0');
            str++;
        }
        *a = sign * val;
        count++;
        //segundo numero
        while (*str == ' ' || *str == '\t') str++;
        sign = 1; val = 0;
        if (*str == '-') { sign = -1; str++; }
        if (*str < '0' || *str > '9') return count;
        while (*str >= '0' && *str <= '9') {
            val = val * 10 + (*str - '0');
            str++;
        }
        *b = sign * val;
        count++;
        return count;
    }
    return 0;
}
int wait(int pid, int piped, int background) {
	if (!piped && !background)
		sys_wait_pid(pid);
	return pid;
}



static char *commandsNames[] = {
    "help", "time", "date", "registers", "fillregs", "div0", "invalidop", "pong", "clear",
    "mem", "ps", "kill", "nice", "block", "unblock", "cat","wc", "filter", "test_mm", "test_sync", "test_prio", "test_processes"
};

static char *commands[] = {
    "\thelp: gives you a list of all existent commands.\n",
    "\ttime: prints the time of the OS.\n",
    "\tdate: prints the date of the OS.\n",
    "\tregisters: print the state of the registers at the time you screenshot them with CTRL key.\n",
    "\tfillregs: fill the registers with stepped values for testing.\n",
    "\tdiv0: divide by zero to trigger exception\n",
    "\tinvalidop: trigger invalid operation code exception\n",
    "\tpong: go to play the \"pong\" game.\n",
    "\tclear: clears the OS screen.\n",
    "\tmem: shows used and free memory.\n",
    "\tps: lists all active processes.\n",
    "\tkill <pid>: kills the process with the given pid.\n",
    "\tnice <pid> <priority>: changes the priority of the process.\n",
    "\tblock <pid>: blocks the process with the given pid.\n",
    "\tunblock <pid>: unblocks the process with the given pid.\n",
	"\tcat: prints stdin as it is received.\n",
    "\tfilter: prints stdin without vowels.\n",
    "\twc: counts the number of lines from stdin.\n",
	"\ttest_mm: test memory manager.\n",
	"\ttest_sync: test synchronization (semaphores).\n",
    "\ttest_prio: test process priorities.\n",
    "\ttest_processes: test process creation, kill, block, unblock.\n",
};
char * loop_args[2] = {"loop", NULL};
char * test_mm_args[3];
char * test_pro_args[3];
char * test_sync_args[4];
void shell() {
	printColor("Welcome to HomerOS. Type \"help\" for command list\n", ORANGE);
	printfColor("Shell PID: %d\n", GREEN, sys_get_pid());
	printColor("\nHomerOS: $> ", GREEN);

	int count = 0;	
	char buffer[1024] = {0};
	char oldBuffer[1024] = {0};
	int defaultFds[2] = {0, 1};

	char flag = 0; // Used for up arrow
	while(1) {
		unsigned char c = get_char();
		if (c == '\n') {
			buffer[count] = 0;
			if (count > 0) {
				analizeBuffer(buffer, count, 0, defaultFds);
			}
			printColor("\nHomerOS: $> ", GREEN);
			strcpy(oldBuffer, buffer);
			flag = 1;
			count = 0;
			buffer[0] = 0; // limpiar buffer explícitamente
		} else if (c == '\b') {
			if (count > 0) {
				print_char(c);
				count--;
			}
		} else if (c == '\t') {
			// analize count letters of the buffer and see if match with any command, if so, complete the command
			int i = 0;
			while (i < COMMANDS_QUANTITY && !strncmp(buffer, commandsNames[i], count)) {
				i++;
			}
			if (i < COMMANDS_QUANTITY) {
				while (commandsNames[i][count] != 0) {
					print_char(commandsNames[i][count]);
					buffer[count] = commandsNames[i][count];
					count++;
				}
			}
		} else if (c == 17 && flag) {
			// up arrow
			while (count > 0) {
				print_char('\b');
				count--;
			}
			strcpy(buffer, oldBuffer);
			count = strlen(buffer);
			print(buffer);
			flag = 0;
		} else if (c == 20 && !flag) {
			// down arrow
			while (count > 0) {
				print_char('\b');
				count--;
			}
			flag = 1;
		} else if (c > 20 && c < 127) {
			print_char(c);
			buffer[count++] = c;
			buffer[count] = 0;
		}
	}
}


int commandMatch(char * str1, char * command, int count) {
	int i = 0;
	while (*str1 == ' ') {
		str1++;
	}
	while (i < count && str1[i] != 0 && command[i] != 0 && str1[i] == command[i]) {
		i++;
		if (str1[i] == ' ') {
			i--;
			break;
		}
	}
	return str1[i] == command[i];
}

int has_pipe(char *buffer) {
    while (*buffer != 0) {
        if (*buffer == '|')
            return 1;
        buffer++;
    }
    return 0;
}
void parse_command(char **argv, char *buffer, int max_args) {
	int i = 0, j = 0;
	while (buffer[i] != 0 && j < max_args) {
		while (buffer[i] == ' ') i++;
		if (buffer[i] == 0) break;
		argv[j++] = &buffer[i];
		while (buffer[i] != 0 && buffer[i] != ' ') i++;
		if (buffer[i] != 0) buffer[i++] = 0;
	}
	argv[j] = NULL;
}

void analyze_piped_command(char *buffer, int count) {
    char *commands[2];
    int i = 0;
    while (buffer[i] != 0) {
        if (buffer[i] == '|') {
            buffer[i] = 0;
            commands[0] = buffer;
            commands[1] = &buffer[i + 1];
            break;
        }
        i++;
    }

    int pipe_fd = sys_pipe_open("default_pipe");

    int fds1[2] = {0, pipe_fd};
    int fds2[2] = {pipe_fd, 1};

    int pid1 = analizeBuffer(commands[0], strlen(commands[0]), 1, fds1);
    int pid2 = analizeBuffer(commands[1], strlen(commands[1]), 1, fds2);

    if (pid1 > 0 && pid2 > 0) {
        sys_wait_pid(pid1);
        sys_wait_pid(pid2);
    }

    sys_close_pipe(pipe_fd);
}

void print_help(){
	printColor("\n\nLos comandos disponibles son:\n\n", GREEN);
	for(int i=0; i<COMMANDS_QUANTITY; i++){
		printColor(commands[i], YELLOW);
	}
}


int isBackground(char * buff) {
	int j = 0;
	while (buff[j] != 0) {
		if (buff[j] == '&') {
			buff[j] = 0;  // corta el comando justo antes del &
			return 1;
		}
		j++;
	}
	return 0;
}


int analizeBuffer(char * buffer, int count, int piped, int * fds) {
	if (count <= 0)
		return -1;

	if (has_pipe(buffer)) {
		analyze_piped_command(buffer, count);
		return 0;
	}

	int background = isBackground(buffer);

	// HELP
	if (commandMatch(buffer, "help", count) || commandMatch(buffer, "HELP", count)) {
		char * args[] = {"help", NULL};
		int pid = sys_create_process("help", 1, !background, &print_help, args);
		return wait(pid, piped, background);
	}

	// TIME
	else if (commandMatch(buffer, "time", count)) {
		char * args[] = {"time", NULL};
		int pid = sys_create_process("time", 1, !background, &time_print, args);
		return wait(pid, piped, background);
	}

	// DATE
	else if (commandMatch(buffer, "date", count)) {
		char * args[] = {"date", NULL};
		int pid = sys_create_process("date", 1, !background, &date_print, args);
		return wait(pid, piped, background);
	}

	// REGISTERS
	else if (commandMatch(buffer, "registers", count)) {
		char * args[] = {"registers", NULL};
		int pid = sys_create_process("registers", 1, !background, &printRegs, args);
		return wait(pid, piped, background);
	}

	// FILLREGS
	else if (commandMatch(buffer, "fillregs", count)) {
		char * args[] = {"fillregs", NULL};
		int pid = sys_create_process("fillregs", 1, !background, &fillRegisters, args);
		return wait(pid, piped, background);
	}

	// CLEAR
	else if (commandMatch(buffer, "clear", count)) {
		sys_clear_screen();
		return 0;
	}

	// DIV0
	else if (commandMatch(buffer, "div0", count)) {
		divideByZero();
		return 0;
	}

	// INVALIDOP
	else if (commandMatch(buffer, "invalidop", count)) {
		invalidOpcode();
		return 0;
	}

	// MEM
	else if (commandMatch(buffer, "mem", count)) {
		char * args[] = {"mem", NULL};
		int pid = sys_create_process("mem", 1, !background, &memInfo, args);
		return wait(pid, piped, background);
	}

	// PS
	else if (commandMatch(buffer, "ps", count)) {
		char * args[] = {"ps", NULL};
		int pid = sys_create_process("ps", 1, !background, &processesInfo, args);
		return wait(pid, piped, background);
	}

	// KILL
	else if (commandMatch(buffer, "kill", count)) {
		char * args[3];
		parse_command(args, buffer, 3);
		int pid = sys_create_process("kill", 1, !background, &killProcess, args);
		return wait(pid, piped, background);
	}

	// BLOCK
	else if (commandMatch(buffer, "block", count)) {
		char * args[3];
		parse_command(args, buffer, 3);
		int pid = sys_create_process("block", 1, !background, &blockProcess, args);
		return wait(pid, piped, background);
	}

	// UNBLOCK
	else if (commandMatch(buffer, "unblock", count)) {
		char * args[3];
		parse_command(args, buffer, 3);
		int pid = sys_create_process("unblock", 1, !background, &unblockProcess, args);
		return wait(pid, piped, background);
	}

	// YIELD
	else if (commandMatch(buffer, "yield", count)) {
		char * args[] = {"yield", NULL};
		int pid = sys_create_process("yield", 1, !background, &yield, args);
		return wait(pid, piped, background);
	}

	// NICE
	else if (commandMatch(buffer, "nice", count)) {
		char * args[4];
		parse_command(args, buffer, 4);
		int pid = sys_create_process("nice", 1, !background, &changePriority, args);
		return wait(pid, piped, background);
	}

	// LOOP
	else if (commandMatch(buffer, "loop", count)) {
		int pid = sys_create_process("loop", 1, !background, &loop_a_main, loop_args);
		return wait(pid, piped, background);
	}

	// TESTMM
	else if (commandMatch(buffer, "testmm", count)) {
		parse_command(test_mm_args, buffer, 3);
		int pid = sys_create_process("testmm", 1, !background, &test_mm, test_mm_args);
		return wait(pid, piped, background);
	}

	// TESTPRO
	else if (commandMatch(buffer, "testpro", count)) {
		parse_command(test_pro_args, buffer, 3);
		int pid = sys_create_process("testpro", 1, !background, &test_processes, test_pro_args);
		return wait(pid, piped, background);
	}

	// TESTPRIO
	else if (commandMatch(buffer, "testprio", count)) {
		int pid = sys_create_process("testprio", 1, !background, &test_prio, NULL);
		return wait(pid, piped, background);
	}

	// TESTSYNC
	else if (commandMatch(buffer, "testsync", count)) {
		parse_command(test_sync_args, buffer, 4);
		int pid = sys_create_process("testsync", 1, !background, &test_sync, test_sync_args);
		return wait(pid, piped, background);
	}

	// PONG
	else if (commandMatch(buffer, "pong", count)) {
		char * args[] = {"pong", NULL};
		int pid = sys_create_process("pong", 1, !background, &pong, args);
		return wait(pid, piped, background);
	}

	// CAT
	else if (commandMatch(buffer, "cat", count)) {
		char * args[] = {"cat", NULL};
		int pid = sys_create_process("cat", 1, !background, &cat, args);
		return wait(pid, piped, background);
	}

	// WC
	else if (commandMatch(buffer, "wc", count)) {
		char * args[] = {"wc", NULL};
		int pid = sys_create_process("wc", 1, !background, &wc, args);
		return wait(pid, piped, background);
	}

	// FILTER
	else if (commandMatch(buffer, "filter", count)) {
		char * args[] = {"filter", NULL};
		int pid = sys_create_process("filter", 1, !background, &filter, args);
		return wait(pid, piped, background);
	}

	// PHILOS
	else if (commandMatch(buffer, "phylos", count)) {
		char * args[] = {"phylos", NULL};
		int pid = sys_create_process("phylos", 1, !background, &phylo, args);
		return wait(pid, piped, background);
	}

	// Default: command not found
	else {
		printColor("\nCommand not found. Type \"help\" for command list\n", RED);
		return -1;
	}
}

