// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <process_userland.h>
#include<colors.h>
#include <uStrings.h>
#include <usyscalls.h>
void memInfo() {
	MemoryDataPtr memData = (MemoryDataPtr) sys_mem_data();
	printfColor("\n\nMemory info:\n", YELLOW);
	printfColor("\tTotal memory: %d bytes\n", CYAN, memData->total);
	printfColor("\tUsed memory: %d bytes\n", CYAN, memData->used);
	printfColor("\tFree memory: %d bytes\n", CYAN, memData->free);
	return;
}

int processesInfo() {
	ProcessInfoPtr * processesInfo = sys_processes_info();
	if (processesInfo == NULL) {
		printfColor("\n\nNo processes running\n", YELLOW);
		return 1;
	}
	printfColor("\n\nProcesses info:\n", YELLOW);
	int i = 0;
	printColor("  PID\tPriority\tParent\tForeground\tStatus\t  Name\n", CYAN);
	while (processesInfo[i] != NULL) {
		printf("   %d", processesInfo[i]->pid);
		printf(processesInfo[i]->pid > 9 ? "    \t%d" : "     \t%d", processesInfo[i]->priority);
		printf("\t\t %d", processesInfo[i]->parent);
		printf("\t\t  %d", processesInfo[i]->foreground);
		switch (processesInfo[i]->status) {
			case READY:
				printf("\t    Ready ");
				break;
			case BLOCKED:
				printf("\t   Blocked");
				break;
			case RUNNING:
				printf("\t   Running");
				break;
			case ZOMBIE:
				printf("\t Zombie(%d)", processesInfo[i]->exitCode);
				break;
			case DEAD:
				printf("\t   Dead(%d)", processesInfo[i]->exitCode);
				break;
			default:
				printf("\t   Unknown");
				break;
		}
		
		printf("\t%s\n", processesInfo[i]->name);
		i++;
	}
	sys_free_processes_info(processesInfo);
	return 0;
}

int killProcess(char ** argv) {
	if (argv[1] == 0) {
		printfColor("\n\nNo process id specified\n", RED);
		return 1;
	}
	int pid = atoi(argv[1]);
	int response = sys_kill_process(pid);
	if (response != 0) {
		printfColor("\n\nProcess %d not found\n", RED, pid);
		return 1;
	}
	printfColor("\n\nProcess %d killed\n", RED, pid);
	return 0;
}

int blockProcess(char ** argv) {
	if (argv[1] == 0) {
		printfColor("\n\nNo process id specified\n", RED);
		return 1;
	}
	int pid = atoi(argv[1]);
	int result = sys_block_process(pid);
	if (result != 0) {
		printfColor("\n\nProcess %d not found\n", RED, pid);
		return 1;
	}
	printfColor("\n\nProcess %d blocked\n", YELLOW, pid);
	return 0;
}

int unblockProcess(char ** argv) {
	if (argv[1] == 0) {
		printfColor("\n\nNo process id specified\n", RED);
		return 1;
	}
	int pid = atoi(argv[1]);
	int result = sys_unblock_process(pid);
	if (result != 0) {
		printfColor("\n\nProcess %d not found\n", RED, pid);
		return 1;
	}
	printfColor("\n\nProcess %d unblocked\n", CYAN, pid);
	return 0;
}

int changePriority(char ** argv) {
	if (argv[1] == 0) {
		printfColor("\n\nNo process id specified\n", RED);
		return 1;
	}
	if (argv[2] == 0) {
		printfColor("\n\nNo priority specified\n", RED);
		return 1;
	}
	int pid = atoi(argv[1]);
	int newPriority = atoi(argv[2]);
	if (newPriority < 0 || newPriority > 5) {
		printfColor("\n\nInvalid priority\n", RED);
		return 1;
	}
	int result = sys_change_priority(pid, newPriority);
	if (result != 0) {
		printfColor("\n\nProcess %s not found\n", RED, pid);
		return 1;
	}
	printfColor("\n\nProcess %d priority changed to %d\n", YELLOW, pid, newPriority);
	return 0;
}

void yield() {
	sys_yield();
}