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
#define COMMANDS_QUANTITY 22
char * testmmArgs[3];

extern int loop_a_main(int argc, char **argv);

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
			analizeBuffer(buffer, count, 0, defaultFds);
			printColor("\nHomerOS: $> ", GREEN);
			strcpy(oldBuffer, buffer);
			flag = 1;
			count = 0;
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
	if (count != strlen(command))
		return 0;
	while (str1[i] != 0 && command[i] != 0 && str1[i] == command[i] && i < count) {
		i++;
	}
	return str1[i] == command[i];
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


void analizeBuffer(char * buffer, int count, int piped, int *fds) {

	if (count <= 0)
		return;

	int background = isBackground(buffer);

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
		char * args[] = { "pong", NULL };
		int pid = sys_create_process("pong", 1, 1, &pong, args);
		sys_wait_pid(pid);
	}
	 else if (commandMatch(buffer, "div0", count)) {
		divideByZero();
	} else if (commandMatch(buffer, "invalidop", count)) {
		invalidOpcode();
	} else if (commandMatch(buffer, "boca", count)) {
		sys_clear_screen();
		sys_draw_image(diego, 100, 100);
		playBSong();
		sys_clear_screen();
	} else if (commandMatch(buffer, "loop", count)) {
        char * args[2] = {"loop", NULL};
        int pid = sys_create_process("loop", 1, 1, &loop_a_main, args);
		sys_wait_pid(pid);
        return;
    } else if (commandMatch(buffer, "ps", count)) {
        char procBuffer[1024];
        sys_list_processes(procBuffer, sizeof(procBuffer));
        printColor(procBuffer, CYAN); 
    } else if (commandMatch(buffer, "cat", count)) {
		char * args[2] = {"cat", NULL};
		int pid = sys_create_process("cat", !background, 1, &cat, args);
		return wait(pid, piped, background);
	}
	
	else if (commandMatch(buffer, "test_mm", count)) {
		char *args[] = { "2048", NULL };
		int pid = sys_create_process("testmm", 1, 1, &test_mm, args);
		if (!background)
			sys_wait_pid(pid);
		return pid;
	}
	else if (commandMatch(buffer, "test_sync", count)) {
        char *args[] = { "1000", "1", NULL }; // ejemplo de argumentos: n=1000, use_sem=1
        int pid = sys_create_process("testsync", 1, 1, &test_sync, args);
        if (!background)
            sys_wait_pid(pid);
        return pid;
    }
    else if (commandMatch(buffer, "test_prio", count)) {
        int pid = sys_create_process("testprio", 1, 1, &test_prio, NULL);
        if (!background)
            sys_wait_pid(pid);
        return pid;
    }
    else if (commandMatch(buffer, "test_processes", count)) {
        char *args[] = { "10", NULL }; // ejemplo: máximo 10 procesos
        int pid = sys_create_process("testprocesses", 1, 1, &test_processes, args);
        if (!background)
            sys_wait_pid(pid);
        return pid;
    }
	else if (commandMatch(buffer, "phylo", count)) {
        char *args[] = { "phylo", NULL };
        int pid = sys_create_process("phylo", 1, 1, &phylo, args);
        if (!background)
            sys_wait_pid(pid);
        return pid;
    }
	 else if (commandMatch(buffer, "cat", count)) {
		char * args[2] = {"cat", NULL};
		int pid = sys_create_process("cat", !background, 1, &cat, args);
		return wait(pid, piped, background);
	 }
	 else if (commandMatch(buffer, "wc", count)) {
		char * args[2] = {"wc", NULL};
		int pid = sys_create_process("wc", !background, &fds, &wc, args);
		return wait(pid, piped, background);
	}
	else if (commandMatch(buffer, "filter", count)) {
		char * args[2] = {"filter", NULL};
		int pid = sys_create_process("filter", !background, fds, &filter, args);
		return wait(pid, piped, background);
	}
	else {
			printfColor("\nCommand not found. Type \"help\" for command list\n", RED);
    }
}
