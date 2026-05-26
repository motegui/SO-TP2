#ifndef PROGRAMS_H
#define PROGRAMS_H
#define EOF -1

int cat(char ** args);
int wc(char **args);
int filter(int argc, char **argv);

int mvar(int argc, char **argv);
int mvar_is_running(void);
void mvar_force_stop(void);

//para phylo.c
void philosopher();
void start_philosopher(int id, int total);
void stop_philosopher(int id);
int phylo(int argc, char **argv);                // Programa principal que lanza la simulación
int philosopher_main(int argc, char **argv);     // Entry point de cada filósofo (proceso hijo)




#endif