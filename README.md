# SO-TP2 — Grupo 20

**72.11 Sistemas Operativos — ITBA (2025)**

Kernel monolítico x86_64. Incluye manejo de procesos, planificador preemptivo con prioridades, semáforos, pipes, dos implementaciones de memory manager (lista enlazada y buddy system) y un shell interactivo (**HomerOS**).

### Integrantes

| Nombre | Padrón | Email |
|--------|--------|-------|
| María Otegui | 61204 | motegui@itba.edu.ar |
| Matías Rinaldo | 60357 | mrinaldo@itba.edu.ar |

---

## Decisiones de diseño

El proyecto parte de la base **x64BareBones** del TP de Arquitectura de Computadoras, extendida con el administrador de memoria, el manejador de procesos y el shell como proceso del kernel al arranque.

### Memory managers

Se implementaron dos administradores, seleccionables en compilación con `#ifdef USE_BUDDY`:

| Modo | Archivo | Descripción |
|------|---------|-------------|
| Lista enlazada (default) | `Kernel/mm_manager.c` | Lista de bloques con dirección, tamaño y estado (libre/ocupado). Simple y adecuado para pocos bloques |
| Buddy system | `Kernel/buddyMemManager.c` | Listas enlazadas por orden (`free_lists[]`); soporta coalescencia y bloques de tamaño variable |

Compilación: `make` (lista enlazada) o `make buddy` / `make BUDDY=1` (buddy).

### Manejador de procesos

- Cada proceso tiene un **PCB** (estado, PID, prioridad, stack, pipes, semáforo propio para `wait_pid`, etc.).
- Procesos activos en **lista enlazada**.
- `process_wrapper` encapsula la ejecución; `create_stack` arma manualmente el stack inicial (RIP, RFLAGS, registros, entry point y argumentos).
- Proceso **idle** (`hlt`) cuando no hay procesos listos.

### Scheduler

Planificador **preemptivo** integrado con la interrupción del timer (`int 0x20` → `_irq00Handler` → `schedule()`):

- Selección **round-robin** entre procesos `READY`.
- **Quantum proporcional a la prioridad** (`quantum = prioridad + 1`): procesos de mayor prioridad reciben más ticks consecutivos antes de ceder.
- `save_context` guarda el stack pointer del proceso actual; el cambio de contexto ocurre en el handler de interrupción vía `iretq`.
- Bloqueo por semáforo: si un proceso está en cola de un semáforo, pasa a estado `BLOCKED`.

### Shell (HomerOS)

Adaptación del shell del TP de Arquitectura como proceso más del SO. Interpreta comandos nativos y se comunica con el kernel mediante syscalls (`sys_create_process`, `sys_wait_pid`, `sys_kill_process`, etc.).

---

## Compilación y ejecución

### Requisitos

- [Docker](https://www.docker.com/) con soporte para `linux/amd64`
- Imagen de la cátedra: `agodio/itba-so-multi-platform:3.0`

### Compilar

Desde la raíz del repositorio:

```bash
cd temp
docker run --rm -it --platform linux/amd64 -v "${PWD}:/root" --privileged agodio/itba-so-multi-platform:3.0
```

Dentro del contenedor:

```bash
cd /root
cd Toolchain && make all && cd ..
make clean && make          # memory manager por defecto (lista enlazada)
make buddy                  # equivalente a: make clean && make BUDDY=1
```

### Ejecutar

Dentro del contenedor (desde `/root`):

```bash
./run.sh          # ventana gráfica (QEMU)
./run.sh serial   # solo consola (recomendado en Docker / macOS)
./noaudiorun.sh   # ejecución sin audio (script alternativo del informe)
./run.sh gdb      # QEMU pausado para depuración (-s -S)
```

Desde macOS con QEMU instalado localmente:

```bash
cd temp
./run.sh serial
```

**Salir de QEMU** en modo serial: `Ctrl+A`, soltar teclas, `X`.

---

## Instrucciones de replicación

### Comandos del shell

Todos los comandos se ejecutan en el prompt `HomerOS: $>`. Escribir `help` lista la ayuda completa.

| Comando | Parámetros | Descripción |
|---------|------------|-------------|
| `help` | — | Lista todos los comandos disponibles |
| `clear` | — | Limpia la pantalla |
| `time` | — | Muestra la hora del sistema |
| `date` | — | Muestra la fecha del sistema |
| `registers` | — | Imprime el estado de los registros (capturados con Ctrl) |
| `fillregs` | — | Carga registros con valores de prueba |
| `div0` | — | Dispara excepción de división por cero |
| `invalidop` | — | Dispara excepción de operación inválida |
| `mem` | — | Muestra memoria total, usada y libre |
| `ps` | — | Lista procesos activos (PID, prioridad, estado, FG/BG, nombre) |
| `kill` | `<pid>` | Termina el proceso indicado |
| `block` | `<pid>` | Bloquea el proceso indicado |
| `unblock` | `<pid>` | Desbloquea el proceso indicado |
| `nice` | `<pid> <prioridad>` | Cambia la prioridad (0–5) |
| `yield` | — | Cede la CPU al scheduler |
| `loop` | — | Proceso de prueba que imprime `A` en loop |
| `cat` | — | Eco de stdin a stdout (lee hasta EOF) |
| `wc` | — | Cuenta líneas recibidas por stdin |
| `filter` | — | Imprime stdin omitiendo vocales |
| `pong` | — | Juego Pong |
| `mvar` | `<writers> <readers>` | Problema lectores/escritores con semáforos |
| `testmm` | `<bytes>` | Test de memory manager |
| `testpro` | `<max_procs>` | Test de creación/kill/block/unblock (loop infinito) |
| `testsync` | `<n> <use_sem>` | Test de sincronización con semáforos |
| `testprio` | — | Test de prioridades de procesos |

**Nota sobre argumentos:** en los tests (`testmm`, `testpro`, `testsync`) el shell omite el nombre del comando al pasar `argv`; el primer parámetro del test es `argv[0]`. En `kill`, `block` y `nice`, `argv[0]` es el nombre del comando y el PID va en `argv[1]`.

### Tests de la cátedra

| Comando | Ejemplo | Resultado esperado |
|---------|---------|-------------------|
| `testmm` | `testmm 102400` | Sin mensaje `test_mm ERROR` (silencioso si pasa) |
| `testpro` | `testpro 5` | Sin `ERROR creating process`; corre indefinidamente |
| `testsync` | `testsync 1000 1` | `Final value: 0` (con semáforos) |
| `testsync` | `testsync 1000 0` | Valor distinto de 0 (condición de carrera) |
| `testprio` | `testprio` | Fases `CHANGING PRIORITIES`, `BLOCKING`, `UNBLOCKING`, `KILLING` |

Para detener `testpro`: reiniciar QEMU o `Ctrl+C` si corre en foreground.

### Caracteres especiales

| Caracter / secuencia | Uso |
|---------------------|-----|
| `\|` (pipe) | Conecta la salida de `cmd1` con la entrada de `cmd2`. Ejemplo: `cat \| wc` |
| `&` (al final) | Ejecuta el comando en **background**. Ejemplo: `loop &` |
| Espacio | Separa comando y argumentos |

### Atajos de teclado

| Atajo | Acción |
|-------|--------|
| **Ctrl+D** | Envía **EOF** (fin de entrada). Usar **Control** (no Command en Mac). Necesario en `cat`, `wc`, `filter` y pipes |
| **Ctrl+C** | Interrumpe el proceso en **foreground** (o detiene `mvar`). El shell vuelve al prompt |
| **Ctrl+A** `X` | Sale de QEMU en modo `./run.sh serial` |

En la ventana gráfica de QEMU, el teclado debe tener foco en la VM.

---

## Ejemplos (fuera de los tests)

### Memory manager

```text
mem
testmm 102400
```

### Procesos (creación, listado, kill, block, unblock)

```text
loop &
ps
block <pid>
ps
unblock <pid>
kill <pid>
```

### Prioridades y planificación

```text
loop &
nice <pid> 4
yield
testprio
```

### Semáforos y sincronización

```text
testsync 1000 1
testsync 1000 0
mvar 2 2
```

### Pipes (IPC)

```text
cat | wc
cat | filter
```

Escribir líneas y presionar **Ctrl+D** para terminar.

### Foreground / background

```text
loop
loop &
```

### Excepciones del procesador

```text
div0
invalidop
```

### Reloj y fecha

```text
time
date
```

---

## Funcionalidades implementadas

| Área | Qué hace el sistema |
|------|---------------------|
| **Memoria** | Dos memory managers (lista enlazada y buddy system), seleccionables al compilar. Syscalls `malloc`/`free`, comando `mem` |
| **Procesos** | Creación con `sys_create_process`, finalización (`exit`, `kill`), bloqueo/desbloqueo, cambio de prioridad (`nice`), listado (`ps`), espera de hijos (`wait_pid`). Estados: NEW, READY, RUNNING, BLOCKED, ZOMBIE |
| **Planificación** | Scheduler preemptivo por timer. Round-robin entre procesos READY con quantum proporcional a la prioridad (0–5) |
| **Semáforos** | Semáforos con nombre (hash), `sem_create`/`sem_wait`/`sem_post`/`sem_close`. Usados en `testsync` y `mvar` |
| **Pipes** | Pipes con nombre entre procesos. En el shell: `cmd1 \| cmd2` con redirección de stdin/stdout |
| **Shell** | Comandos en foreground y background (`&`). Ctrl+C mata el proceso en foreground; Ctrl+D envía EOF |
| **IPC userland** | `cat`, `wc`, `filter` sobre stdin; pipelines con pipe |
| **Excepciones** | Manejo de división por cero e instrucción inválida (`div0`, `invalidop`) |
| **Tests de cátedra** | `testmm`, `testpro`, `testsync`, `testprio` |

---

## Limitaciones

- **Pipes en el shell:** conexión entre dos comandos (`cmd1 | cmd2`).
- **Prioridades:** rango 0 (más baja) a 5 (más alta).
- **Heap del kernel:** 1 MB administrado (`MANAGED_MEMORY_SIZE = 0x100000`).
- **Semáforos:** hasta 128 simultáneos (`MAX_SEMAPHORES`).
- **Memory manager:** se elige en compilación (`make` o `make buddy`), no en runtime.
- **Procesos:** se crean con puntero a función de entrada (`sys_create_process`), no con carga de binarios externos.
- **macOS / Docker:** se recomienda `./run.sh serial` para captura correcta del teclado.
- **`testpro`:** corre en loop infinito por diseño del test de la cátedra.

---

## Problemas resueltos durante el desarrollo

Resumen de los principales inconvenientes técnicos (detallados en el informe del TP):

1. **Stack inicial de procesos:** orden incorrecto de entry point, argumentos y wrapper en `create_stack`. Corregido verificando pushes y depurando con GDB.
2. **Responsabilidad del scheduler:** inicialmente intentaba el context switch completo desde su función; se simplificó para que solo guarde contexto y devuelva el stack pointer del siguiente proceso, dejando el switch real al `_irq00Handler`.
3. **`irq00Handler` e `iretq`:** reorganizado para que el scheduler devuelva el nuevo stack antes del `iretq`.
5. **`sys_get_ticks`:** escribía en dirección NULL y siempre devolvía 0; corregido para retornar `ticks_elapsed()`.
6. **Ticks del timer:** `timer_handler()` no se invocaba en el path del timer; agregado al inicio de `schedule()`.
7. **Leak de procesos en `kill`:** procesos matados quedaban como ZOMBIE sin liberar semáforos; `kill_process` ahora libera recursos en kills explícitos.
8. **Comandos `kill`/`block`/`nice` del shell:** firmas incorrectas `(argv)` en lugar de `(argc, argv)`; syscall de `nice` apuntaba al número 40 (pipes) en lugar del 19.

---

## Citas de fragmentos de código / uso de IA

### Código base

- Proyecto **x64BareBones** y base del **TP de Arquitectura de Computadoras** (cátedra ITBA).
- Bootloader Pure64, Toolchain, scripts `run.sh` / `noaudiorun.sh`.
- Imagen Docker: `agodio/itba-so-multi-platform:3.0`.
- Tests oficiales de cátedra en `Userland/SampleCodeModule/tests/`.

### Referencias

- Material de la materia e Intel x86_64 (IDT, interrupciones, context switching).
- Buddy allocator: implementación propia con listas por orden.

### Uso de inteligencia artificial

Se utilizó **Cursor (asistente de IA)** como apoyo en depuración y revisión de código. Las correcciones fueron verificadas compilando y probando en QEMU. Las decisiones de diseño y la entrega final son responsabilidad del grupo.
