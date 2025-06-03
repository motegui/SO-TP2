#ifndef PROGRAMS_H
#define PROGRAMS_H

int cat(int argc, char **argv);
int wc(int argc, char **argv);
int filter(int argc, char **argv);

//para phylo.c
void philosopher();
void start_philosopher(int id);
void stop_philosopher(int id);
int phylo(int argc, char **argv);


#endif