#include <usyscalls.h>
#include <colors.h>
#include <userio.h>

char * get_time() {
	static char bufferTime[9];
	sys_get_time(bufferTime);
	return bufferTime;
}

char * get_date() {
	static char bufferDate[9];
	sys_get_date(bufferDate);
	return bufferDate;
}

void time_print() {
	printfColor("\n\nTime of OS: ", YELLOW);
	printfColor("%s\n", CYAN, get_time());
}

void date_print() {
	printfColor("\n\nDate of OS: ", YELLOW);
	printfColor("%s\n", CYAN, get_date());
}