#ifndef SHELL_H
#define SHELL_H

extern void divideByZero();
extern void invalidOpcode();

void shell();

int commandMatch(char * str1, char * command, int count);

int analizeBuffer(char * buffer, int count, int pipe, int *fds);
int isBackground(char * buff);

#endif