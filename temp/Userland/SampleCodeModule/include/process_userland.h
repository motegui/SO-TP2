#ifndef STRUCTS_H
#define STRUCTS_H


#include <stddef.h>
#include <stdint.h>
#include <userio.h>
#include <usyscalls.h>
#include <uStrings.h>

#define NEW     0
#define READY   1
#define RUNNING 2
#define BLOCKED 3
#define DEAD    4
#define ZOMBIE  5


typedef struct memoryData {
	uint64_t free, used, total;
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
int blockProcess(int argc, char ** argv);
int unblockProcess(int argc, char ** argv);
int changePriority(int argc, char ** argv);
void yield();
int killProcess(int argc, char ** argv);

#endif  // STRUCTS_H
