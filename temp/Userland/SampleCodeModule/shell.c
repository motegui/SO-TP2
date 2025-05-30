#include <colors.h>
#include <userio.h>
#include <usyscalls.h>
#include <colors.h>
#include <sounds.h>
#include <boca.h>
#include <pong.h>
#include <shell.h>
#include <tests.h>
#define COMMANDS_QUANTITY (sizeof(commandsNames) / sizeof(char *))
char * testmmArgs[3];


int sscanf(const char *str, const char *format, int *a, int *b) {
    if (format[0] == '%' && format[1] == 'd' && format[2] == ' ' && format[3] == '%' && format[4] == 'd') {
        int i = 0, sign = 1, val = 0, count = 0;
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


static char *commandsNames[] = {
    "help", "time", "date", "registers", "fillregs", "div0", "invalidop", "pong", "clear",
    "getmemstatus", "ps", "kill", "nice", "block", "unblock", "testmm", "sem_create", "sem_open", "sem_close", "sem_wait", "sem_post"
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
    "\tgetmemstatus: shows used and free memory.\n",
    "\tps: lists all active processes.\n",
    "\tkill <pid>: kills the process with the given pid.\n",
    "\tnice <pid> <priority>: changes the priority of the process.\n",
    "\tblock <pid>: blocks the process with the given pid.\n",
    "\tunblock <pid>: unblocks the process with the given pid.\n",
	"\ttestmm: test memory manager.\n",
	"\tsem_create <name> <value>: creates a new semaphore.\n",
    "\tsem_open <name>: opens an existing semaphore.\n",
    "\tsem_close <name>: closes an open semaphore.\n",
    "\tsem_wait <name>: does wait (P operation) on semaphore.\n",
    "\tsem_post <name>: does signal (V operation) on semaphore.\n"
};

void shell() {
    printColor("Welcome to HomerOS. Type \"help\" for command list\n", ORANGE);
	printColor("\nHomerOS: $> ", GREEN);

	int count = 0;	
	char buffer[1024] = {0};
	char oldBuffer[1024] = {0};
	char flag = 0; // Used for up arrow
	while(1) {
		unsigned char c = getChar();
		if (c == '\n') {
			buffer[count] = 0;
			analizeBuffer(buffer, count);
			printColor("\nHomerOS: $> ", GREEN);
			strcpy(oldBuffer, buffer);
			flag = 1;
			count = 0;
		} else if (c == '\b') {
			if (count > 0) {
				printChar(c);
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
					printChar(commandsNames[i][count]);
					buffer[count] = commandsNames[i][count];
					count++;
				}
			}
		} else if (c == 17 && flag) {
			// up arrow
			while (count > 0) {
				printChar('\b');
				count--;
			}
			strcpy(buffer, oldBuffer);
			count = strlen(buffer);
			print(buffer);
			flag = 0;
		} else if (c == 20 && !flag) {
			// down arrow
			while (count > 0) {
				printChar('\b');
				count--;
			}
			flag = 1;
		} else if (c > 20 && c < 127) {
			printChar(c);
			buffer[count++] = c;
			buffer[count] = 0;
		}
	}
}


int commandMatch(char * str1, char * command, int count) {
	int i = 0;
	if (count != strlen(command))
		return 0;
	while (str1[i] != 0 && command[i] != 0 && str1[i] == command[i] && i < count) {
		i++;
	}
	return str1[i] == command[i];
}

void analizeBuffer(char * buffer, int count) {
	if (count <= 0)
		return;
	if (commandMatch(buffer, "help", count) || commandMatch(buffer, "HELP", count)) {
		printColor("\n\nComandos disponibles:\n\n", YELLOW);
		for (int i = 0; i < COMMANDS_QUANTITY; i++) {
			printColor(commands[i], CYAN);
		}
	} else if (commandMatch(buffer, "time", count)) {
		printfColor("\n\nTime of OS: ", YELLOW);
		printfColor("%s\n", CYAN, getTime());
	} else if (commandMatch(buffer, "date", count)) {
		printfColor("\n\nDate of OS: ", YELLOW);
		printfColor("%s\n", CYAN, getDate());
	} else if (commandMatch(buffer, "registers", count)) {
		printRegs();
	} else if (commandMatch(buffer, "fillregs", count)) {
		fillRegisters();
	} else if (commandMatch(buffer, "clear", count)) {
		sys_clear_screen();
	} else if (commandMatch(buffer, "pong", count)) {
		pong();
	} else if (commandMatch(buffer, "div0", count)) {
		divideByZero();
	} else if (commandMatch(buffer, "invalidop", count)) {
		invalidOpcode();
	} else if (commandMatch(buffer, "boca", count)) {
		sys_clear_screen();
		sys_draw_image(diego, 100, 100);
		playBSong();
		sys_clear_screen();
	} else if (commandMatch(buffer, "getmemstatus", count)) {
    	uint64_t used = 0, free = 0;
    	sys_get_mem_status(&used, &free);
    	printfColor("\nMemory used: %d bytes\n", YELLOW, used);
    	printfColor("Free Memory: %d bytes\n", YELLOW, free);
	} else if (commandMatch(buffer, "ps", count)) {
    	char procs[1024] = {0};
    	sys_list_processes(procs, sizeof(procs));
    	printfColor("\nActive processes:\n%s\n", CYAN, procs);
	} else if (commandMatch(buffer, "kill", 4) && count > 5 && buffer[4] == ' ') {
		int pid = atoi(buffer + 5);
		sys_kill_process(pid);
		printfColor("Killing process... %d\n", RED, pid);
	} else if (commandMatch(buffer, "nice", 4) && count > 7 && buffer[4] == ' ') {
		int pid, prio;
		if (sscanf(buffer + 5, "%d %d", &pid, &prio) == 2) {
			sys_nice_process(pid, prio);
			printfColor("Priority changed from %d to %d\n", YELLOW, pid, prio);
		} else {
			printfColor("Use: nice <pid> <new_priority>\n", RED);
		}
	} else if (commandMatch(buffer, "block", 5) && count > 6 && buffer[5] == ' ') {
		int pid = atoi(buffer + 6);
		sys_block_process(pid);
		printfColor("Process %d blocked\n", YELLOW, pid);
	} else if (commandMatch(buffer, "unblock", 7) && count > 8 && buffer[7] == ' ') {
		int pid = atoi(buffer + 8);
		sys_unblock_process(pid);
		printfColor("Process %d unlocked\n", YELLOW, pid);
	} else if (commandMatch(buffer, "sem_create", 10) && count > 12 && buffer[10] == ' ') {
		char *name = buffer + 11;
		char *value_str = name;

		while (*value_str && *value_str != ' ') {
			value_str++;
		}

		if (*value_str != 0) {
			*value_str = '\0';
			value_str++;
			uint64_t val = atoi(value_str);
			sys_sem_create((uint64_t)name, val);
			printfColor("Semaphore '%s' created with value %d\n", GREEN, name, val);
		} else {
			printfColor("Usage: sem_create <name> <value>\n", RED);
		}
	} else if (commandMatch(buffer, "sem_open", 8) && count > 9 && buffer[8] == ' ') {
		char *name = buffer + 9;
		sys_sem_open((uint64_t)name);
		printfColor("Semaphore '%s' opened\n", GREEN, name);
	} else if (commandMatch(buffer, "sem_close", 9) && count > 10 && buffer[9] == ' ') {
		char *name = buffer + 10;
		sys_sem_close((uint64_t)name);
		printfColor("Semaphore '%s' closed\n", GREEN, name);
	} else if (commandMatch(buffer, "sem_wait", 8) && count > 9 && buffer[8] == ' ') {
		char *name = buffer + 9;
		sys_sem_wait((uint64_t)name);
		printfColor("Semaphore '%s' wait (P) executed\n", GREEN, name);
	} else if (commandMatch(buffer, "sem_post", 8) && count > 9 && buffer[8] == ' ') {
		char *name = buffer + 9;
		sys_sem_post((uint64_t)name);
		printfColor("Semaphore '%s' post (V) executed\n", GREEN, name);
	}

	// else if (commandMatch(buffer, "testmm", count)) {
	// 	parseCommand(testmmArgs, buffer, 3);
	// 	int pid = sys_create_process("testmm", testmmArgs, &test_mm, !background, fds);
	// 	if (!background)
	// 		sys_wait_pid(pid);
	// 	return pid;
	// }
	else if (count > 0) {
		printColor("\nCommand not found. Type \"help\" for command list\n", RED);
	}
}