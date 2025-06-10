GCC=gcc
LD=ld
AR=ar
ASM=nasm

GCCFLAGS=-m64 -fno-exceptions -fno-asynchronous-unwind-tables  -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc  -Wall -ffreestanding -nostdlib -fno-common -std=c99 -Wno-unused-variable -g -fno-pie

ARFLAGS=rvs
ASMFLAGS=-felf64
LDFLAGS=--warn-common -z max-page-size=0x1000
