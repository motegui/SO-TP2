#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process_manager.h"

// Llama al scheduler manualmente (por ejemplo desde el timer interrupt handler)
void schedule(void);

// Selecciona el próximo proceso a ejecutar según prioridad (Round Robin)
PCB *pick_next_process(void);

// Inicia la planificación y salta al primer proceso (usado en arranque)
void start_scheduler(void);

<<<<<<< Updated upstream
#endif
=======
// Guarda el contexto del proceso actual (guardando RSP)
void save_context(PCB *pcb);

// Carga el contexto de un proceso (poniendo su RSP y haciendo ret)
void load_context(PCB *pcb);

#endif // SCHEDULER_H
>>>>>>> Stashed changes
