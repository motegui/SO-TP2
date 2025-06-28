#ifndef PROGRAMS_H
#define PROGRAMS_H
#define EOF -1

int cat(char ** args);
int wc(char **args);
int filter(char **args);

//para phylo.c
void philosopher();
void start_philosopher(int id, int total);
void stop_philosopher(int id);
int phylo(int argc, char **argv);                // Programa principal que lanza la simulación
int philosopher_main(int argc, char **argv);     // Entry point de cada filósofo (proceso hijo)




#endif