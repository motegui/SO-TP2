#ifndef STRUCTS_H
#define STRUCTS_H


#include <stddef.h>
#include <stdint.h>
#include <userio.h>
#include <usyscalls.h>
#include <uStrings.h>

#define READY   0
#define BLOCKED 1
#define RUNNING 2
#define ZOMBIE  3
#define DEAD    4


typedef struct memoryData {
	uint32_t free, used, total;
} memoryData;

typedef struct processInfo {
	char * name;
	int pid;
	int parent;
	uint64_t * rsp;
	uint64_t * rbp;
	size_t priority;
	char foreground;
	int status;
	int exitCode;
} processInfo;

void memInfo();
int processesInfo();
int blockProcess(char ** argv);
int unblockProcess(char ** argv);
int changePriority(char ** argv);
void yield();
int killProcess(char ** argv);

#endif  // STRUCTS_H