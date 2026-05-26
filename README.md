# SO-TP2 — Grupo 18 (CDNS)

Kernel monolítico x86_64 para la materia Sistemas Operativos (ITBA).

## Compilación

Requisito: imagen Docker de la cátedra.

```bash
docker pull agodio/itba-so-multi-platform:3.1
cd temp
docker run --rm -it --platform linux/amd64 -v "${PWD}:/root" --privileged agodio/itba-so-multi-platform:3.1
```

Dentro del contenedor:

```bash
cd /root
cd Toolchain && make all && cd ..
make clean && make          # memory manager por defecto
make buddy                  # buddy system
```

## Ejecución

Dentro del contenedor (desde `/root`):

```bash
./run.sh          # ventana gráfica (QEMU)
./run.sh serial   # solo consola (recomendado en Docker/Mac)
```

Desde macOS con QEMU instalado:

```bash
cd temp
./run.sh serial
```

Salir de QEMU en modo serial: `Ctrl+A`, soltar, `X`.

## Atajos de teclado

| Atajo | Acción |
|-------|--------|
| Ctrl+D | EOF (fin de entrada en cat/wc/filter). Usar **Control** izquierdo, foco en la VM |
| Ctrl+C | Mata el proceso en foreground |
| `&` al final | Ejecuta en background |
| `\|` | Pipe entre dos comandos (`cat \| wc`) |

## Comandos y tests

Ver `help` dentro del shell. Tests de cátedra:

| Comando | Ejemplo | Qué verificar |
|---------|---------|---------------|
| testmm | `testmm 102400` | Sin `test_mm ERROR` (solo imprime si falla) |
| testpro | `testpro 5` | Sin errores de creación de procesos |
| testsync | `testsync 1000 1` | `Final value: 0` con semáforos |
| testsync | `testsync 1000 0` | Valor distinto de 0 (sin semáforos) |
| testprio | `testprio` | Mensajes de cambio de prioridades |

## Pipes y IPC

```text
cat | wc
cat | filter
```

Escribir líneas + Enter; Ctrl+D para terminar. `wc` debe mostrar el total de líneas correcto.

## Memory managers

- `make` / `make all`: lista enlazada (`mm_manager.c`)
- `make buddy`: buddy system (`BUDDY=1`)

## Limitaciones conocidas

- Pipes: solo dos procesos (`cmd1 | cmd2`).
- Filósofos (`phylos`): interactivo con `a`/`r`; requiere foreground.
- En Mac, capturar teclado en la ventana QEMU o usar `./run.sh serial`.

## Citas / IA

Completar según política del curso.
